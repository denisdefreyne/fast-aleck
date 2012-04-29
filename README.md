Fast Aleck
==========

Fast Aleck is a tool for making text look smarter by using proper Unicode characters. For example, ... becomes … (`&hellip;`) and -- becomes — (`&mdash;`). Its goal is to be the fastest text cleaning tool out there.

[![Build Status](https://secure.travis-ci.org/ddfreyne/fast-aleck.png)](http://travis-ci.org/ddfreyne/fast-aleck)

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
