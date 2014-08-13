/**
 * JavaScript wrapper and REPL for the awl programming language
 */
if (typeof Worker !== 'undefined') {
    (function(global, $) {
        // Initialize Awl REPL
        //
        var worker = initializeWorker();
        initializeConsole(worker);

        /**
         * Represents an interactive terminal that communicates with
         * an async web worker
         */
        function AwlTerminal(worker) {
            var self = this;
            self.worker = worker;
            self.echoBuffer = [];
            self.dom = setupTerminalElement();

            function setupTerminalElement() {
                var dom = $('#interpreter');

                if (!dom.length) {
                    var marker = $('h3').first();

                    dom = $('<div />').
                        attr('id', 'interpreter').
                        addClass('terminal').
                        css({ height: 300, marginBottom: '1em' }).
                        text('Loading...').
                        insertBefore(marker);
                }
                return dom;
            }

            self.worker.addHandler('version', self.setupTerminal.bind(self));
            self.worker.addHandler('print', self.terminalPrintFn.bind(self));

            // Query for Awl version
            self.worker.postMessage({ message: 'version' });
        }

        AwlTerminal.prototype.setupTerminal = function(v) {
            var self = this;
            self.dom.text('');
            self.term = self.dom.terminal(function(command, term) {
                self.worker.postMessage({ message: 'eval', value: command });
            }, {
                greetings: 'awl ' + v + '\n',
                name: 'awl',
                height: 300,
                prompt: 'awl> '
            });
        };

        AwlTerminal.prototype.terminalPrintFn = function(s) {
            // Buffer the output, because term.echo() adds newline
            var self = this,
                parts = s.split('\n'),
                i, l;

            // Will always be at least a single element
            self.echoBuffer.push(parts[0]);

            if (parts.length > 1) {
                // At least one newline, begin outputting
                self.term.echo(self.echoBuffer.join(''));

                for (i = 1, l = parts.length; i < l - 1; i++) {
                    self.term.echo(parts[i]);
                }

                // Only echo last element if it isn't empty
                if (parts[parts.length - 1] !== '') {
                    self.term.echo(parts[parts.length - 1]);
                }

                // Reset buffer
                self.echoBuffer = [];
            }
        };

        /**
         * Initialize background worker
         */
        function initializeWorker() {
            // Keep track of registered handlers
            var messageHandlers = {};

            var worker = new Worker('javascripts/workerawl.js');

            worker.addEventListener('message', function(e) {
                if (e.data.message in messageHandlers) {
                    messageHandlers[e.data.message].call(null, e.data.value);
                } else {
                    console.log('message unhandled: ' + e.data.message);
                }
            }, false);

            worker.addEventListener('error', function(e) {
                console.log(e.message, e.filename, e.lineno);
            }, false);

            return {
                addHandler: function(msg, fn) {
                    messageHandlers[msg] = fn;
                },
                postMessage: worker.postMessage.bind(worker)
            };
        }

        /**
         * Initialize the console UI
         */
        function initializeConsole(worker) {
            $(function() {
                var awlTerminal = new AwlTerminal(worker);
            });
        }
    })(this, jQuery);
}
