/**
 * JavaScript wrapper and REPL for the awl programming language
 */
if (typeof Module !== 'undefined') {
    (function(global, Module, Runtime) {
        Module.postRun.push(function() {

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

            // Add printing function
            var printFn = Runtime.addFunction(function(ptr) {
                console.log(extractString(ptr));
            });

            // Initialize
            setupAwl();
            registerPrintFn(printFn);
            var env = awlenvNewTopLevel();

            global.awlEval = function(s) {
                evalReplStr(env, s);
            };

        });
    })(this, Module, Runtime);
}
