# Fast Aleck [![Build Status](https://secure.travis-ci.org/ddfreyne/fast-aleck.png)](http://travis-ci.org/ddfreyne/fast-aleck)

Fast Aleck is a tool for making text look smarter by using proper Unicode characters. For example, ... becomes … (`&hellip;`) and -- becomes — (`&mdash;`). Its goal is to be the fastest text cleaning tool out there. It is HTML-aware and also supports UTF-8.

Fast Aleck is **alpha quality**. Expect things to break. If it breaks, please do let me know! Crash reports and examples of incorrect output are *greatly* appreciated!

How fast is it really?
----------------------

Here are the results of a benchmark in which a copy of Alice's Adventures in Wonderland is typographically enhanced (see ruby/bench.rb):

                                          user          system      total        real
    alice-rubypants              (50)     13.160000   0.390000  13.550000 ( 13.565909)
    alice-typogruby              (500)    30.380000   0.080000  30.460000 ( 30.449274)
    alice-fast-aleck-all-options (5000)   11.450000   0.260000  11.710000 ( 11.696178)
    alice-fast-aleck-no-options  (5000)   10.910000   0.250000  11.160000 ( 11.164061)

In this benchmark, Fast Aleck is run 5000 times, RubyPants is run 500 times (10x less) and Typogrify is run 50 times (100x) less. Fast Aleck is over 10 times as fast as Rubypants and 200x-300x as fast as Typogruby.

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

* fast-aleck-ruby - Ruby bindings for Fast Aleck (see the ruby/ dir)
* [fast-aleck-hs](https://github.com/jaspervdj/fast-aleck-hs) - Haskell bindings for Fast Aleck
