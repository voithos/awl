/**
 * Maintains state of Awl within the worker
 */
function AwlWorker() {
    this.loaded = false;
    this.awl = null;
    this.queue = [];

    this.messageHandlers = {
        version: function() {
            postMessage({ message: 'version', value: this.awl.version });
        },
        eval: function(s) {
            this.awl.eval(s);
        }
    };
}

AwlWorker.prototype.initializeWorker = function(awl) {
    awl.setupPrintFn(function(s) {
        postMessage({ message: 'print', value: s });
    });

    this.loaded = true;
    this.awl = awl;
    this.executeQueue();
};


AwlWorker.prototype.beginListening = function() {
    function dispatcher(e) {
        if (e.data.message in this.messageHandlers) {
            if (this.loaded) {
                this.messageHandlers[e.data.message].call(this, e.data.value);
            } else {
                this.queue.push([e.data.message, e.data.value]);
            }
        }
    }

    addEventListener('message', dispatcher.bind(this), false);
};

AwlWorker.prototype.executeQueue = function() {
    this.queue.forEach(function(v) {
        this.messageHandlers[v[0]].call(this, v[1]);
    }, this);
};

var worker = new AwlWorker();

// Begin listening and queueing requests because Awl takes a while to load
worker.beginListening();

importScripts('awl.js');

if (typeof Module !== 'undefined') {
    Module.postRun.push(initialize);

    function initialize() {
        var awl = initializeAwl();
        worker.initializeWorker(awl);

        /**
         * Initialize Awl module bindings and evaluation functions
         */
        function initializeAwl() {
            // Util functions
            var extractString = function(ptr) {
                var cstr = [];
                while (true) {
                    var c = Module.getValue(ptr, 'i8');
                    if (c === 0) {
                        break;
                    }
                    cstr.push(String.fromCharCode(c));
                    ptr++;
                }
                return cstr.join('');
            };

            // Wrap exported functions
            var setupAwl = Module.cwrap('setup_awl', null, []);
            var teardownAwl = Module.cwrap('teardown_awl', null, []);
            var getAwlVersion = Module.cwrap('get_awl_version', null, []);
            var registerPrintFn = Module.cwrap('register_print_fn', null, ['number']);
            var awlenvNewTopLevel = Module.cwrap('awlenv_new_top_level', 'number', []);
            var evalReplStr = Module.cwrap('eval_repl_str', 'void', ['number', 'string']);

            // Initialize
            setupAwl();
            var version = extractString(getAwlVersion());
            var env = awlenvNewTopLevel();

            // Return awl API
            return {
                version: version,
                eval: function(s) {
                    evalReplStr(env, s);
                },
                setupPrintFn: function(fn) {
                    var fnPtr = Runtime.addFunction(function(ptr) {
                        fn(extractString(ptr));
                    });
                    registerPrintFn(fnPtr);
                }
            };
        }
    }
}
