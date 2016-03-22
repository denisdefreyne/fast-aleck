# Fast Aleck [![Build Status](https://secure.travis-ci.org/ddfreyne/fast-aleck.png)](http://travis-ci.org/ddfreyne/fast-aleck)

**IMPORTANT: Fast Aleck is deprecated and no longer in development.** I stopped working on Fast Aleck for three reasons:

1. Typographical enhancement is an intrinsically difficult task, and I believe it cannot be done reliably.

2. I’ve never managed to get Fast Aleck to the point where it correctly handles all test cases, which I attribute to the same cause as point 1. It is also not as fast as I had hoped it to be (although with more work, this could be solved).

3.  I don’t use any typographical enhancement tool anymore myself; I type the proper characters (smart quotes, em dashes, ellipses) manually. On OS X:

    <table>
		  <thead>
			  <tr>
				  <th>key combo</th>
					<th>result</th>
				</tr>
			</head>
			<tbody>
			  <tr>
				  <td>⌥ ;</td>
					<td>…</td>
				</tr>
			  <tr>
				  <td>⌥ ]</td>
					<td>‘ (left single quotation mark)</td>
				</tr>
			  <tr>
				  <td>⌥ ⇧ ]</td>
					<td>’ (right single quotation mark)</td>
				</tr>
			  <tr>
				  <td>⌥ [</td>
					<td>“ (left double quotation mark)</td>
				</tr>
			  <tr>
				  <td>⌥ ⇧ [</td>
					<td>” (right double quotation mark)</td>
				</tr>
			  <tr>
				  <td>⌥ -</td>
					<td>– (en dash)</td>
				</tr>
			  <tr>
				  <td>⌥ ⇧ -</td>
					<td>— (em dash)</td>
				</tr>
			</tbody>
		</table>

* * *

Fast Aleck is a tool for making text look smarter by using proper Unicode characters. For example, ... becomes … (`&hellip;`) and -- becomes — (`&mdash;`). Its goal is to be the fastest text cleaning tool out there. It is HTML-aware and also supports UTF-8.

How fast is it really?
----------------------

Here are the results of a benchmark in which a copy of Alice's Adventures in Wonderland is typographically enhanced (see ruby/bench.rb):

	                                   user     system      total        real
	alice-rubypants (x10)         14.620000   0.470000  15.090000 ( 15.088082)
	alice-typogruby (x100)        32.820000   0.120000  32.940000 ( 32.957644)
	alice-fast-aleck-all-options  36.110000   5.040000  41.150000 ( 42.310613)
	alice-fast-aleck-no-options   12.880000   1.940000  14.820000 ( 14.932533)

Fast Aleck runs 5000 times, Rubypants 500 times and Typogruby 50 times (hence the x10 and x100 indicators).

`no-options` has the same features as Rubypants, while `all-options` has the same features as Typogruby. These benchmarks indicate that Fast Aleck in its current state is about one order of magnitude faster than Rubypants, and about two orders of magnitude faster than Typogruby.

No development time has been spent on optimising Fast Aleck yet, but this will hopefully happen Soon™.

Features
--------

* ... → …
* -- → —
* --- → —
* '' → ‘’
* "" → “”
* Skips text inside `code`, `kbd`, `pre` and `script` elements
* Optionally wraps `&amp;` in `<span class="amp">` so you can pick the font with the prettiest ampersand
* Optionally wraps `'` in a span with class `quo`
* Optionally wraps `"` in a span with class `dquo`
* Optionally replaces last significant whitespace in a block with a non-breaking space (Widon’t)
* Optionally wraps sequences of capitals in `<span class="caps">` so you can use small caps.

Example input:

	This... CANNOT be true--it's impossible! "I had five EC2 instances!"

Example output:

	This… <span class="caps">CANNOT</span> be true&mdash;it’s impossible! “I had
	five <span class="caps">EC2</span>&nbsp;instances!”

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
