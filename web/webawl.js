/**
 * JavaScript wrapper and REPL for the awl programming language
 */
if (typeof Module !== 'undefined') {
    (function(global, $, Module, Runtime) {
        Module.postRun.push(initialize);

        function initialize() {
            var awl = initializeAwl();
            initializeConsole(awl);

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
                var registerPrintFn = Module.cwrap('register_print_fn', null, ['number']);
                var awlenvNewTopLevel = Module.cwrap('awlenv_new_top_level', 'number', []);
                var evalReplStr = Module.cwrap('eval_repl_str', 'void', ['number', 'string']);

                // Initialize
                setupAwl();
                var env = awlenvNewTopLevel();

                // Return awl API
                return {
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

            /**
             * Initialize the console UI
             */
            function initializeConsole(awl) {
                $(function() {
                    var term = $('#interpreter').terminal(function(command, term) {
                        awl.eval(command);
                    }, {
                        greetings: 'awl v0.2.0\n',
                        name: 'awl',
                        height: 300,
                        prompt: 'awl> '
                    });

                    var echoBuffer = [];

                    awl.setupPrintFn(function(s) {
                        // Buffer the output, because term.echo() adds newline
                        var parts = s.split('\n'),
                            i, l;

                        // Will always be at least a single element
                        echoBuffer.push(parts[0]);

                        if (parts.length > 1) {
                            // At least one newline, begin outputting
                            term.echo(echoBuffer.join(''));

                            for (i = 1, l = parts.length; i < l - 1; i++) {
                                term.echo(parts[i]);
                            }

                            // Only echo last element if it isn't empty
                            if (parts[parts.length - 1] !== '') {
                                term.echo(parts[parts.length - 1]);
                            }

                            // Reset buffer
                            echoBuffer = [];
                        }
                    });
                });
            }
        }
    })(this, jQuery, Module, Runtime);
}
