# Fast Aleck [![Build Status](https://secure.travis-ci.org/ddfreyne/fast-aleck.png)](http://travis-ci.org/ddfreyne/fast-aleck)

Fast Aleck is a tool for making text look smarter by using proper Unicode characters. For example, ... becomes … (`&hellip;`) and -- becomes — (`&mdash;`). Its goal is to be the fastest text cleaning tool out there. It is HTML-aware and also supports UTF-8.

Status
------

Fast Aleck is **alpha quality**. Expect things to break. If it breaks, please do let me know! Crash reports and examples of incorrect output are *greatly* appreciated!

Required re-engineering
-----------------------

At this moment, there are two branches, `master` and `cleanup`. Since the code in `master` was becoming hard to maintain, work on a cleaner codebase was started in the `cleanup` branch. This branch (as well as `master`) suffers from conceptual problems which will need to tackled in order for Fast Aleck to become usable. The follow problems need to be tackled:

* **Erroneous reordering** - Since functions can both write to the final output buffer as well as pass on data to the next function to process, and because functions can buffer data before writing it to the output stream, it is possible to arrive at situations in which output is written in an incorrect order.

* **Slowness due to char processing** - Fast Aleck currently feeds char by char, which is inefficient since large amounts of texts that do not require processing are not handled as a single blob but rather as a collection of chars. Making use of blobs will speed things up, but may cause cache misses if the blobs are too large.

* **Composability** - Individual components of the Fast Aleck conversion workflow should be easily composable. The `cleanup` branch already divides the old code in components, but these are still fairly hardcoded. These components are `TAGS`, `BODY`, `CAPS` and `WIDONT`. It should be possible to construct a workflow which consists of only the required components (e.g. only `TAGS` and `BODY`).

* **Size of the code base** - There is currently only a single file that contains all the processing code. If Fast Aleck is split into multiple independent components, these components could each live in their own source file.

How fast is it really?
----------------------

Here are the results of a benchmark in which a copy of Alice's Adventures in Wonderland is typographically enhanced 5000 times (see ruby/bench.rb):

	Rubypants                   2min 15.77s
	Typogruby                  50min 23.29s
	Fast Aleck (all options)         10.75s
	Fast Aleck (no options)           9.30s

As you can see, Fast Aleck is over 10 times as fast as Rubypants and about 300x as fast as Typogruby.

Features
--------

* ... → …
* -- → —
* --- → —
* '' → ‘’
* "" → “”
* Skips text inside `code`, `kbd`, `pre` and `script` elements
* Optionally wraps `&amp;` in `<span class="amp">&amp;</span>` so you can pick the font with the prettiest ampersand
* Optionally wraps `'` in a span with class `quo`
* Optionally wraps `"` in a span with class `dquo`
* Widon't
* class="caps" for multiple capital letters

Installation
------------

Building Fast Aleck requires CMake. Once you have that:

	% mkdir build
	% cd build
	% cmake ..
	% make

There will be three executables in the build directory once that is finished:

* `fast-aleck` is the commandline tool (try `fast-aleck --help` for help)
* `libfast-aleck.so` is the shared library (the exact name might differ depending on the OS)
* `fast-aleck-test` is a tool that runs the tests

If you intend to use the Ruby bindings, copy `libfast-aleck.so` to a public location (such as `/usr/local/lib`) or into the directory where you are executing Ruby from. For instance, if you want to use Fast Aleck with nanoc, you can copy `libfast-aleck.so` into the site directory (not in `./lib`).

Related Projects
----------------

* [fast-aleck-ruby](https://github.com/ddfreyne/fast-aleck-ruby) - Ruby bindings for Fast Aleck
* [fast-aleck-hs](https://github.com/jaspervdj/fast-aleck-hs) - Haskell bindings for Fast Aleck
