# Fast Aleck [![Build Status](https://secure.travis-ci.org/ddfreyne/fast-aleck.png)](http://travis-ci.org/ddfreyne/fast-aleck)

Fast Aleck is a tool for making text look smarter by using proper Unicode characters. For example, ... becomes … (`&hellip;`) and -- becomes — (`&mdash;`). Its goal is to be the fastest text cleaning tool out there.

How fast is it really?
----------------------

Here are the results of a benchmark in which a copy of Alice's Adventures in Wonderland is typographically enhanced a thousand times (see ruby/bench.rb):

                           user     system      total        real
    alice-rubypants    2.730000   0.090000   2.820000 (  2.811613)
    alice-typogruby   62.050000   0.180000  62.230000 ( 62.232689)
    alice-fast-aleck   0.140000   0.010000   0.150000 (  0.152693)

Fast Aleck is over 15 times as fast as Rubypants and about 400x as fast as Typogruby. Admittedly, this benchmark is flawed because Fast Aleck does not do everything Typogruby does, but the order of magnitude should remain correct once Fast Aleck is complete.

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
