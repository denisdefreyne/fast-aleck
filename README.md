# Fast Aleck [![Build Status](https://secure.travis-ci.org/ddfreyne/fast-aleck.png)](http://travis-ci.org/ddfreyne/fast-aleck)

Fast Aleck is a tool for making text look smarter by using proper Unicode characters. For example, ... becomes … (`&hellip;`) and -- becomes — (`&mdash;`). Its goal is to be the fastest text cleaning tool out there.

How fast is it really?
----------------------

Here are the results of a benchmark in which a copy of Alice's Adventures in Wonderland is typographically enhanced a thousand times (see ruby/bench.rb):

                           user     system      total        real
    alice-rubypants   27.020000   0.780000  27.800000 ( 27.800711)
    alice-fast-aleck   1.470000   0.050000   1.520000 (  1.520092)

Fast Aleck is 18 times as fast as Rubypants.

To do
-----

* Implement remaining features listed below

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

Not yet implemented but coming up:

* Widon't
* class="caps" for multiple capital letters
