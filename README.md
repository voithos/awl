# Awl

_Lisp_: from late Old English _awlyspian_, meaning "to lisp."

Awl is an experimental mini-language based on the Lisp family of programming
languages.

It was written for fun and ~~profit~~ to learn more about interpreter design,
programming in C, and using [emscripten](https://github.com/kripken/emscripten)
to transpile to JavaScript.

**Note**: This naturally goes without saying, but Awl is just an experimental
learning project. It should *not* be used for production code. That being said,
experimenting and hacking on non-production-ready code just for fun is usually
worthwhile!

## Compiling

Most of awl's dependencies are included in the repository, so you shouldn't
need to install anything other than the build tools. Awl takes advantage of
some new features in C11, so you will need a fairly recent C compiler.

- Both `clang` (tested with version 3.5.0) and `gcc` (tested with version
  4.7.2) are known to successfully compile
- To transpile to JavaScript, you'll need the emscripten toolkit, including
  `emcc`.
- You'll also need `make`

First, clone the repository, and then compile using `make`:

    $ git clone https://github.com/voithos/awl.git
    $ cd awl
    $ make
    
This will create an `awl` binary under `bin/`.

You can also compile and execute tests:

	$ make test

Or transpile to JavaScript (`emcc` will need to be in your `$PATH`):

	$ make web
    
You can also clean up:

	$ make clean

## Usage

The `awl` binary can take a single argument - a path to a file to execute.

	$ ./bin/awl [file]

If no argument is given, then it will drop into an interactive interpreter
([REPL](http://en.wikipedia.org/wiki/Read%E2%80%93eval%E2%80%93print_loop)).

	$ ./bin/awl
    awl v0.x.y
    Ctrl+D to exit

    awl>

## Features

Awl is a mini-language that is inspired by the Lisp family of languages. Thus,
it shares most of its features with Lisp and Scheme. These include:

- Dynamic typing
- First-class functions, including [anonymous (lambda)
  functions](http://en.wikipedia.org/wiki/Anonymous_function)
- Function
  [closures](http://en.wikipedia.org/wiki/Closure_(computer_programming))
- [Tail-call optimization](http://en.wikipedia.org/wiki/Tail_recursion)
- Immutability (although variables can be redefined currently)
- Lists as the primary data structure
- [Homoiconicity](http://en.wikipedia.org/wiki/Homoiconicity) - that is,
  similar representations of code and data
- Metaprogramming in the form of simple macros

Currently, Awl's data definition and manipulation capabilities are lacking, but
this will hopefully be changed in the future.

## Open Source

Thanks goes to the following awesome libraries and open source projects, and
their creators:

- mpc.c
- ptest.c
- linenoise
- clang / LLVM
- emscripten
