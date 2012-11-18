#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fast-aleck/fast-aleck.h>
#include <fast-aleck/test/fast-aleck-test.h>

static void fast_aleck_test(struct fa_test_suite *a_test_suite, char *a_input, char *a_expected_output);

int main(void) {
	struct fa_test_suite test_suite;
	test_suite.count       = 0;
	test_suite.passes      = 0;
	test_suite.fails       = 0;
	test_suite.wrap_amps   = 0;
	test_suite.wrap_caps   = 0;
	test_suite.wrap_quotes = 0;
	test_suite.widont      = 0;

	fast_aleck_test(&test_suite,
		"I am a simple sentence.",
		"I am a simple sentence.");

	fast_aleck_test(&test_suite,
		"I am... a sentence with an ellipsis!",
		"I am… a sentence with an ellipsis!");

	fast_aleck_test(&test_suite,
		"I am a sentence--really---with two dashes!",
		"I am a sentence—really—with two dashes!");

	fast_aleck_test(&test_suite,
		"This sentence ends in two periods..",
		"This sentence ends in two periods..");

	fast_aleck_test(&test_suite,
		"This sentence ends in a number 1. ",
		"This sentence ends in a number 1. ");

	fast_aleck_test(&test_suite,
		"This sentence ends in two periods..</span>",
		"This sentence ends in two periods..</span>");

	fast_aleck_test(&test_suite,
		"This sentence ends in three periods...",
		"This sentence ends in three periods…");

	fast_aleck_test(&test_suite,
		"Weird! This sentence ends in a dash-",
		"Weird! This sentence ends in a dash-");

	fast_aleck_test(&test_suite,
		"Weird! This sentence ends in two dashes--",
		"Weird! This sentence ends in two dashes—");

	fast_aleck_test(&test_suite,
		"Weird! This sentence ends in three dashes---",
		"Weird! This sentence ends in three dashes—");

	fast_aleck_test(&test_suite,
		"My sister's book. My sisters' books.",
		"My sister’s book. My sisters’ books.");

	fast_aleck_test(&test_suite,
		"'Hello', she said. 'Hello,' she said. 'Huh...' she mumbled.",
		"‘Hello’, she said. ‘Hello,’ she said. ‘Huh…’ she mumbled.");

	fast_aleck_test(&test_suite,
		"\"Hello\", she said. \"Hello,\" she said. \"Huh...\" she mumbled.",
		"“Hello”, she said. “Hello,” she said. “Huh…” she mumbled.");

	fast_aleck_test(&test_suite,
		"'That's mine!', she said. \"That's mine!\", she said.",
		"‘That’s mine!’, she said. “That’s mine!”, she said.");

	fast_aleck_test(&test_suite,
		"(\"Amazing!\" she thought.) ('Amazing!' she thought.)",
		"(“Amazing!” she thought.) (‘Amazing!’ she thought.)");

	fast_aleck_test(&test_suite,
		"Before... yes. <![CDATA[Not here! & ] ... ] &amp > --]]> After... yes",
		"Before… yes. <![CDATA[Not here! & ] ... ] &amp > --]]> After… yes");

	fast_aleck_test(&test_suite,
		"Before... yes. <!--Not here! & < > ! -- &amp --x --> After... yes",
		"Before… yes. <!--Not here! & < > ! -- &amp --x --> After… yes");

	fast_aleck_test(&test_suite,
		"Before... yes. <img alt=\"During... no.\"> After... yes.",
		"Before… yes. <img alt=\"During... no.\"> After… yes.");

	fast_aleck_test(&test_suite,
		"Foo... <img alt=\"Bar... > Baz...\"> Quux...",
		"Foo… <img alt=\"Bar... > Baz...\"> Quux…");

	fast_aleck_test(&test_suite,
		"Before... yes. <script>In script... no.</script> After... yes.",
		"Before… yes. <script>In script... no.</script> After… yes.");

	fast_aleck_test(&test_suite,
		"Before... yes. <pre>In pre... no.</pre> After... yes.",
		"Before… yes. <pre>In pre... no.</pre> After… yes.");

	fast_aleck_test(&test_suite,
		"Before... yes. <code>In code... no.</code> After... yes.",
		"Before… yes. <code>In code... no.</code> After… yes.");

	fast_aleck_test(&test_suite,
		"Before... yes. <kbd>In kbd... no.</kbd> After... yes.",
		"Before… yes. <kbd>In kbd... no.</kbd> After… yes.");

	fast_aleck_test(&test_suite,
		"Before... <samp>In samp... </samp> After...",
		"Before… <samp>In samp... </samp> After…");

	fast_aleck_test(&test_suite,
		"Before... <var>In var... </var> After...",
		"Before… <var>In var... </var> After…");

	fast_aleck_test(&test_suite,
		"Before... <math>In math... </math> After...",
		"Before… <math>In math... </math> After…");

	fast_aleck_test(&test_suite,
		"Before... <textarea>In textarea... </textarea> After...",
		"Before… <textarea>In textarea... </textarea> After…");

	fast_aleck_test(&test_suite,
		"Before... yes. <p>In something else (like p)... yes!</p> After... yes.",
		"Before… yes. <p>In something else (like p)… yes!</p> After… yes.");

	fast_aleck_test(&test_suite,
		"Before... <pre>In pre... <code>In code...</code> In pre...</pre> After...",
		"Before… <pre>In pre... <code>In code...</code> In pre...</pre> After…");

	fast_aleck_test(&test_suite,
		"Some <em>text</em> wrapped in <span class=\"meh\">elements</span>.",
		"Some <em>text</em> wrapped in <span class=\"meh\">elements</span>.");

	fast_aleck_test(&test_suite,
		"Before... <codezzz>during...</codezzz> after...",
		"Before… <codezzz>during…</codezzz> after…");

	fast_aleck_test(&test_suite,
		"Before... <kbdzzz>during...</kbdzzz> after...",
		"Before… <kbdzzz>during…</kbdzzz> after…");

	fast_aleck_test(&test_suite,
		"Before... <prezzz>during...</prezzz> after...",
		"Before… <prezzz>during…</prezzz> after…");

	fast_aleck_test(&test_suite,
		"Before... <scriptzzz>during...</scriptzzz> after...",
		"Before… <scriptzzz>during…</scriptzzz> after…");

	fast_aleck_test(&test_suite,
		"Before... <zzzcode>during...</zzzcode> after...",
		"Before… <zzzcode>during…</zzzcode> after…");

	fast_aleck_test(&test_suite,
		"Before... <zzzkbd>during...</zzzkbd> after...",
		"Before… <zzzkbd>during…</zzzkbd> after…");

	fast_aleck_test(&test_suite,
		"Before... <zzzpre>during...</zzzpre> after...",
		"Before… <zzzpre>during…</zzzpre> after…");

	fast_aleck_test(&test_suite,
		"Before... <zzzscript>during...</zzzscript> after...",
		"Before… <zzzscript>during…</zzzscript> after…");

	fast_aleck_test(&test_suite,
		"<script>",
		"<script>");

	fast_aleck_test(&test_suite,
		"</script>",
		"</script>");

	fast_aleck_test(&test_suite,
		"<script>a",
		"<script>a");

	fast_aleck_test(&test_suite,
		"\"<a href=\"#\">blah</a>\"",
		"“<a href=\"#\">blah</a>”");

	fast_aleck_test(&test_suite,
		"<a href=\"#\">\"blah\"</a>",
		"<a href=\"#\">“blah”</a>");

	fast_aleck_test(&test_suite,
		"\"blah <a href=\"#\">blah</a>\"",
		"“blah <a href=\"#\">blah</a>”");

	fast_aleck_test(&test_suite,
		"\"<div>blah</div>\"",
		"“<div>blah</div>“");

	fast_aleck_test(&test_suite,
		"<div>\"blah\"</div>",
		"<div>“blah”</div>");

	fast_aleck_test(&test_suite,
		"\"blah <div>blah</div>\"",
		"“blah <div>blah</div>“");

	fast_aleck_test(&test_suite,
		"foo </div",
		"foo </div");

	fast_aleck_test(&test_suite,
		"foo <div class='meh",
		"foo <div class='meh");

	fast_aleck_test(&test_suite,
		"foo <div class=\"meh",
		"foo <div class=\"meh");

	// WRAP AMPS TESTS

	test_suite.wrap_amps = 1;

	fast_aleck_test(&test_suite,
		"Four &lt; Seventeen",
		"Four &lt; Seventeen");

	fast_aleck_test(&test_suite,
		"Ampersands &amp; More",
		"Ampersands <span class=\"amp\">&amp;</span> More");

	fast_aleck_test(&test_suite,
		"Ampersands &</span>",
		"Ampersands &</span>");

	fast_aleck_test(&test_suite,
		"Ampersands &a</span>",
		"Ampersands &a</span>");

	fast_aleck_test(&test_suite,
		"Ampersands &am</span>",
		"Ampersands &am</span>");

	fast_aleck_test(&test_suite,
		"Ampersands &amp</span>",
		"Ampersands &amp</span>");

	fast_aleck_test(&test_suite,
		"Ampersands &amp;</span>",
		"Ampersands <span class=\"amp\">&amp;</span></span>");

	fast_aleck_test(&test_suite,
		"A &amp; B <title>C &amp; D</title> E &amp; F",
		"A <span class=\"amp\">&amp;</span> B <title>C &amp; D</title> E <span class=\"amp\">&amp;</span> F");

	test_suite.wrap_amps = 0;

	// WRAP QUOTES TESTS

	test_suite.wrap_quotes = 1;

	fast_aleck_test(&test_suite,
		"There's a hole in the sky.",
		"There’s a hole in the sky.");

	fast_aleck_test(&test_suite,
		"'There's a hole in the sky', he said. 'Don't be silly', she said.",
		"<span class=\"quo\">‘</span>There’s a hole in the sky’, he said. ‘Don’t be silly’, she said.");

	fast_aleck_test(&test_suite,
		"\"There's a hole in the sky\", he said. \"Don't be silly\", she said.",
		"<span class=\"dquo\">“</span>There’s a hole in the sky”, he said. “Don’t be silly”, she said.");

	fast_aleck_test(&test_suite,
		"\"Here.\"<p>\"Here.\" \"Not here.\"<p>\"Here.\"",
		"<span class=\"dquo\">“</span>Here.”<p><span class=\"dquo\">“</span>Here.” “Not here.”<p><span class=\"dquo\">“</span>Here.”");

	fast_aleck_test(&test_suite,
		"\"Here.\"<li>\"Here.\" \"Not here.\"<li>\"Here.\"",
		"<span class=\"dquo\">“</span>Here.”<li><span class=\"dquo\">“</span>Here.” “Not here.”<li><span class=\"dquo\">“</span>Here.”");

	fast_aleck_test(&test_suite,
		"\"Here.\"<div>\"Here.\" \"Not here.\"<div>\"Here.\"",
		"<span class=\"dquo\">“</span>Here.”<div><span class=\"dquo\">“</span>Here.” “Not here.”<div><span class=\"dquo\">“</span>Here.”");

	fast_aleck_test(&test_suite,
		"<title>'There's a hole in the sky'</title>",
		"<title>‘There’s a hole in the sky’</title>");

	// TODO h1, h2, ..., h6

	test_suite.wrap_quotes = 0;

	// WIDONT TESTS

	test_suite.widont = 1;

	fast_aleck_test(&test_suite,
		"<p>Foo bar baz. Woof meow moo.</p>",
		"<p>Foo bar baz. Woof meow&nbsp;moo.</p>");

	fast_aleck_test(&test_suite,
		"<p>Foo-bar baz. </p>",
		"<p>Foo-bar&nbsp;baz. </p>");

	fast_aleck_test(&test_suite,
		"<p>Foo bar baz.</p><p>Woof meow moo.</p>",
		"<p>Foo bar&nbsp;baz.</p><p>Woof meow&nbsp;moo.</p>");

	fast_aleck_test(&test_suite,
		"<li>Foo bar baz.</li><li>Woof meow moo.</li>",
		"<li>Foo bar&nbsp;baz.</li><li>Woof meow&nbsp;moo.</li>");

	fast_aleck_test(&test_suite,
		"<div>Foo bar baz.</div><div>Woof meow moo.</div>",
		"<div>Foo bar&nbsp;baz.</div><div>Woof meow&nbsp;moo.</div>");

	fast_aleck_test(&test_suite,
		"<h1>Foo bar baz.</h1><h1>Woof meow moo.</h1>",
		"<h1>Foo bar&nbsp;baz.</h1><h1>Woof meow&nbsp;moo.</h1>");

	fast_aleck_test(&test_suite,
		"<h2>Foo bar baz.</h2><h2>Woof meow moo.</h2>",
		"<h2>Foo bar&nbsp;baz.</h2><h2>Woof meow&nbsp;moo.</h2>");

	fast_aleck_test(&test_suite,
		"<h3>Foo bar baz.</h3><h3>Woof meow moo.</h3>",
		"<h3>Foo bar&nbsp;baz.</h3><h3>Woof meow&nbsp;moo.</h3>");

	fast_aleck_test(&test_suite,
		"<h4>Foo bar baz.</h4><h4>Woof meow moo.</h4>",
		"<h4>Foo bar&nbsp;baz.</h4><h4>Woof meow&nbsp;moo.</h4>");

	fast_aleck_test(&test_suite,
		"<h5>Foo bar baz.</h5><h5>Woof meow moo.</h5>",
		"<h5>Foo bar&nbsp;baz.</h5><h5>Woof meow&nbsp;moo.</h5>");

	fast_aleck_test(&test_suite,
		"<h6>Foo bar baz.</h6><h6>Woof meow moo.</h6>",
		"<h6>Foo bar&nbsp;baz.</h6><h6>Woof meow&nbsp;moo.</h6>");

	fast_aleck_test(&test_suite,
		"<blockquote>Foo bar baz.</blockquote><blockquote>Woof meow moo.</blockquote>",
		"<blockquote>Foo bar&nbsp;baz.</blockquote><blockquote>Woof meow&nbsp;moo.</blockquote>");

	fast_aleck_test(&test_suite,
		"<dd>Foo bar baz.</dd><dd>Woof meow moo.</dd>",
		"<dd>Foo bar&nbsp;baz.</dd><dd>Woof meow&nbsp;moo.</dd>");

	fast_aleck_test(&test_suite,
		"<dt>Foo bar baz.</dt><dt>Woof meow moo.</dt>",
		"<dt>Foo bar&nbsp;baz.</dt><dt>Woof meow&nbsp;moo.</dt>");

	fast_aleck_test(&test_suite,
		"<ol>\n<li>This is a list item</li>\n</ol>",
		"<ol>\n<li>This is a list&nbsp;item</li>\n</ol>");

	fast_aleck_test(&test_suite,
		"<section>\n<h1>Title!</h1>\n</section>",
		"<section>\n<h1>Title!</h1>\n</section>");

	fast_aleck_test(&test_suite,
		"<p>Paragraph one</p>\n<p>Paragraph two</p>",
		"<p>Paragraph&nbsp;one</p>\n<p>Paragraph&nbsp;two</p>");

	fast_aleck_test(&test_suite,
		"<dt>Foo bar baz.</dt>\n<dt>Woof meow moo.</dt>",
		"<dt>Foo bar&nbsp;baz.</dt>\n<dt>Woof meow&nbsp;moo.</dt>");

	fast_aleck_test(&test_suite,
		"<li><a href=\"../\"><span>Home</span></a></li>\n<li><a href=\"../blog/\"><span>Blog</span></a></li>",
		"<li><a href=\"../\"><span>Home</span></a></li>\n<li><a href=\"../blog/\"><span>Blog</span></a></li>");

	fast_aleck_test(&test_suite,
		"<li><span>line one</span></li>\n<li><span>line two</span></li>",
		"<li><span>line&nbsp;one</span></li>\n<li><span>line&nbsp;two</span></li>");

	fast_aleck_test(&test_suite,
		"A B C D",
		"A B C&nbsp;D");

	fast_aleck_test(&test_suite,
		"<h1><a href='#'>A B C D</a></h1>",
		"<h1><a href='#'>A B C&nbsp;D</a></h1>");

	fast_aleck_test(&test_suite,
		"<p>This paragraph should <em>not at all</em> be interrupted!</p>",
		"<p>This paragraph should <em>not at all</em> be&nbsp;interrupted!</p>");

	fast_aleck_test(&test_suite,
		"<h1><a href='#'>why won't this widon't</a></h1>",
		"<h1><a href='#'>why won’t this&nbsp;widon’t</a></h1>");

	fast_aleck_test(&test_suite,
		"<p>foo bar<br>baz qux</p>",
		"<p>foo&nbsp;bar<br>baz&nbsp;qux</p>");

	fast_aleck_test(&test_suite,
		"<p>one<br>\ntwo</p>",
		"<p>one<br>\ntwo</p>");

	fast_aleck_test(&test_suite,
		"<p>foo bar\n<br>\nbaz</p>",
		"<p>foo&nbsp;bar\n<br>\nbaz</p>");

	fast_aleck_test(&test_suite,
		"<p>foo bar </p>",
		"<p>foo&nbsp;bar </p>");

	fast_aleck_test(&test_suite,
		"<p>foo  bar</p>",
		"<p>foo&nbsp;bar</p>");

	fast_aleck_test(&test_suite,
		"<p> foo</p>",
		"<p> foo</p>");

	fast_aleck_test(&test_suite,
		"<p>foo &amp;</p>",
		"<p>foo&nbsp;&amp;</p>");

	fast_aleck_test(&test_suite,
		"<p>foo &amp; bar</p>",
		"<p>foo &amp;&nbsp;bar</p>");

	test_suite.widont = 0;

	fast_aleck_test(&test_suite,
		"<dt>We don't Widon't.</dt>",
		"<dt>We don’t Widon’t.</dt>");

	// WRAP CAPS TEST

	test_suite.wrap_caps = 1;

	fast_aleck_test(&test_suite,
		"Hello, this is DENIS speaking!",
		"Hello, this is <span class=\"caps\">DENIS</span> speaking!");

	fast_aleck_test(&test_suite,
		"DENIS's pants.",
		"<span class=\"caps\">DENIS</span>’s pants.");

	fast_aleck_test(&test_suite,
		"I have 13 EC2 instances but no static AMIs.",
		"I have 13 <span class=\"caps\">EC2</span> instances but no static <span class=\"caps\">AMI</span>s.");

	fast_aleck_test(&test_suite,
		"<title>Hello, this is DENIS speaking!</title>",
		"<title>Hello, this is DENIS speaking!</title>");

	fast_aleck_test(&test_suite,
		"<p>MongoDB is better than PostgreSQL</p>",
		"<p>Mongo<span class=\"caps\">DB</span> is better than Postgre<span class=\"caps\">SQL</span></p>");

	// COMBINATION TESTS

	test_suite.wrap_caps   = 1;
	test_suite.widont      = 1;
	test_suite.wrap_amps   = 0;
	test_suite.wrap_quotes = 0;
	fast_aleck_test(&test_suite,
		"<p>One Two <b>THR</b>EE!</p>",
		"<p>One Two <b><span class=\"caps\">THR</span></b><span class=\"caps\">EE</span>!</p>");

	test_suite.wrap_caps = 0;

	fast_aleck_test(&test_suite,
		"Do NOT wrap caps if I don't ask to!",
		"Do NOT wrap caps if I don’t ask to!");

	fprintf(stdout, "1..%i\n", test_suite.count);

	return (test_suite.fails > 0 ? 1 : 0);
}

static void _fa_puts_escaped(char *a_s) {
	for (char *s = a_s; *s; ++s) {
		if ('\n' == *s)
			fputs("\\n", stdout);
		else if ('\v' == *s)
			fputs("\\v", stdout);
		else
			fputc(*s, stdout);
	}
	fputc('\n', stdout);
}

static void fast_aleck_test(struct fa_test_suite *a_test_suite, char *a_input, char *a_expected_output) {
	fast_aleck_config config;
	fast_aleck_config_init(&config);
	config.wrap_amps   = a_test_suite->wrap_amps;
	config.wrap_caps   = a_test_suite->wrap_caps;
	config.wrap_quotes = a_test_suite->wrap_quotes;
	config.widont      = a_test_suite->widont;

	size_t out_len;
	char *actual_output = fast_aleck(config, a_input, strlen(a_input), &out_len);

	if (0 != strcmp(a_expected_output, actual_output)) {
		++a_test_suite->fails;
		fprintf(stdout, "not ok %i ", a_test_suite->count+1);
		_fa_puts_escaped(a_input);
		fprintf(stdout, "  Expected: ");
		_fa_puts_escaped(a_expected_output);
		fprintf(stdout, "  Actual:   ");
		_fa_puts_escaped(actual_output);
	} else if(strlen(actual_output) != out_len) {
		++a_test_suite->fails;
		fprintf(stdout, "not ok %i %s\n", a_test_suite->count+1, a_input);
		fprintf(stdout, "  Length of returned string: %lu\n", strlen(actual_output));
		fprintf(stdout, "  Returned length of string: %lu\n", out_len);
	} else {
		++a_test_suite->passes;
		fprintf(stdout, "ok %i ", a_test_suite->count+1);
		_fa_puts_escaped(a_input);
	}

	++a_test_suite->count;

	free(actual_output);
}
