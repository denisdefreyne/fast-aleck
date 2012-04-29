Fast Aleck
==========

Fast Aleck is a tool for making text look smarter by using proper Unicode characters. For example, ... becomes … (`&hellip;`) and -- becomes — (`&mdash;`). Its goal is to be the fastest text cleaning tool out there.

Despite its name, it is not intended to be API-compatible with [Smartypants](http://daringfireball.net/projects/smartypants/). It is not, and likely never will be, customisable like Smartypants. Perhaps there never will be any options at all. It will also not do just what Smartypants does; its aim is to be a replacement for everything [Typogrify](http://code.google.com/p/typogrify/) does.

To do
-----

* Implement remaining features listed below
* Get a better name

Features
--------

* ... → …
* -- → —
* --- → —
* '' → ‘’
* "" → “”
* Skips text inside `code`, `kbd`, `pre` and `script` elements
* Optionally wraps `&amp;` in `<span class="amp">&amp;</span>` so you can pick the font with the prettiest ampersand

Not yet implemented but coming up:

* Widon't
* class="dquo" for double quotes
* class="quo" for single quotes
* class="caps" for multiple capital letters
