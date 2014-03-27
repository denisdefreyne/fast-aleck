#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include <fast-aleck/fast-aleck.h>
#include <fast-aleck/state.h>
#include <fast-aleck/tokenizer.h>
#include <fast-aleck/token.h>
#include <fast-aleck/token-buffer.h>
#include <fast-aleck/test/fast-aleck-test.h>

#define FAST_ALECK_TEST_CONFIG_CHECK_TOKENIZATION (false)

static void fast_aleck_test(struct fa_test_suite *a_test_suite, char *a_input, char *a_expected_output, ...);

static inline fa_token _tok(fa_token_type type, char *s) {
	fa_token tok;
	tok.type = type;
	tok.slice.start = s;
	tok.slice.length = strlen(s);
	return tok;
}

#define TOK(t, s) _tok(fa_token_type_ ## t, s)

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
		"I am a simple sentence.",
		TOK(text, "I am a simple sentence."), fa_null_token);

	fast_aleck_test(&test_suite,
		"I am... a sentence with an ellipsis!",
		"I am… a sentence with an ellipsis!",
		TOK(text, "I am... a sentence with an ellipsis!"), fa_null_token);

	fast_aleck_test(&test_suite,
		"I am a sentence--really---with two dashes!",
		"I am a sentence—really—with two dashes!",
		TOK(text, "I am a sentence--really---with two dashes!"), fa_null_token);

	fast_aleck_test(&test_suite,
		"This sentence ends in two periods..",
		"This sentence ends in two periods..",
		TOK(text, "This sentence ends in two periods.."), fa_null_token);

	fast_aleck_test(&test_suite,
		"This sentence ends in a number 1. ",
		"This sentence ends in a number 1. ",
		TOK(text, "This sentence ends in a number 1. "), fa_null_token);

	fast_aleck_test(&test_suite,
		"This sentence ends in two periods..</span>",
		"This sentence ends in two periods..</span>",
		TOK(text, "This sentence ends in two periods.."), TOK(inline, "</span>"), fa_null_token);

	fast_aleck_test(&test_suite,
		"This sentence ends in three periods...",
		"This sentence ends in three periods…",
		TOK(text, "This sentence ends in three periods..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Weird! This sentence ends in a dash-",
		"Weird! This sentence ends in a dash-",
		TOK(text, "Weird! This sentence ends in a dash-"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Weird! This sentence ends in two dashes--",
		"Weird! This sentence ends in two dashes—",
		TOK(text, "Weird! This sentence ends in two dashes--"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Weird! This sentence ends in three dashes---",
		"Weird! This sentence ends in three dashes—",
		TOK(text, "Weird! This sentence ends in three dashes---"), fa_null_token);

	fast_aleck_test(&test_suite,
		"My sister's book. My sisters' books.",
		"My sister’s book. My sisters’ books.",
		TOK(text, "My sister's book. My sisters' books."), fa_null_token);

	fast_aleck_test(&test_suite,
		"'Hello', she said. 'Hello,' she said. 'Huh...' she mumbled.",
		"‘Hello’, she said. ‘Hello,’ she said. ‘Huh…’ she mumbled.",
		TOK(text, "'Hello', she said. 'Hello,' she said. 'Huh...' she mumbled."), fa_null_token);

	fast_aleck_test(&test_suite,
		"\"Hello\", she said. \"Hello,\" she said. \"Huh...\" she mumbled.",
		"“Hello”, she said. “Hello,” she said. “Huh…” she mumbled.",
		TOK(text, "\"Hello\", she said. \"Hello,\" she said. \"Huh...\" she mumbled."), fa_null_token);

	fast_aleck_test(&test_suite,
		"'That's mine!', she said. \"That's mine!\", she said.",
		"‘That’s mine!’, she said. “That’s mine!”, she said.",
		TOK(text, "'That's mine!', she said. \"That's mine!\", she said."), fa_null_token);

	fast_aleck_test(&test_suite,
		"(\"Amazing!\" she thought.) ('Amazing!' she thought.)",
		"(“Amazing!” she thought.) (‘Amazing!’ she thought.)",
		TOK(text, "(\"Amazing!\" she thought.) ('Amazing!' she thought.)"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... yes. <![CDATA[Not here! & ] ... ] &amp > --]]> After... yes",
		"Before… yes. <![CDATA[Not here! & ] ... ] &amp > --]]> After… yes",
		TOK(text, "Before... yes. "), TOK(text_raw, "<![CDATA[Not here! & ] ... ] &amp > --]]>"), TOK(text, " After... yes"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... yes. <!--Not here! & < > ! -- &amp --x --> After... yes",
		"Before… yes. <!--Not here! & < > ! -- &amp --x --> After… yes",
		TOK(text, "Before... yes. "), TOK(text_raw, "<!--Not here! & < > ! -- &amp --x -->"), TOK(text, " After... yes"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... yes. <img alt=\"During... no.\"> After... yes.",
		"Before… yes. <img alt=\"During... no.\"> After… yes.",
		TOK(text, "Before... yes. "), TOK(inline, "<img alt=\"During... no.\">"), TOK(text, " After... yes."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Foo... <img alt=\"Bar... > Baz...\"> Quux...",
		"Foo… <img alt=\"Bar... > Baz...\"> Quux…",
		TOK(text, "Foo... "), TOK(inline, "<img alt=\"Bar... > Baz...\">"), TOK(text, " Quux..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... yes. <script>In script... no.</script> After... yes.",
		"Before… yes. <script>In script... no.</script> After… yes.",
		TOK(text, "Before... yes. "), TOK(inline, "<script>"), TOK(text_raw, "In script... no."), TOK(inline, "</script>"), TOK(text, " After... yes."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... yes. <pre>In pre... no.</pre> After... yes.",
		"Before… yes. <pre>In pre... no.</pre> After… yes.",
		TOK(text, "Before... yes. "), TOK(block, "<pre>"), TOK(text_raw, "In pre... no."), TOK(block, "</pre>"), TOK(text, " After... yes."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... yes. <code>In code... no.</code> After... yes.",
		"Before… yes. <code>In code... no.</code> After… yes.",
		TOK(text, "Before... yes. "), TOK(block, "<code>"), TOK(text_raw, "In code... no."), TOK(block, "</code>"), TOK(text, " After... yes."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... yes. <kbd>In kbd... no.</kbd> After... yes.",
		"Before… yes. <kbd>In kbd... no.</kbd> After… yes.",
		TOK(text, "Before... yes. "), TOK(block, "<kbd>"), TOK(text_raw, "In kbd... no."), TOK(block, "</kbd>"), TOK(text, " After... yes."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... <samp>In samp... </samp> After...",
		"Before… <samp>In samp... </samp> After…",
		TOK(text, "Before... "), TOK(block, "<samp>"), TOK(text_raw, "In samp... "), TOK(block, "</samp>"), TOK(text, " After..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... <var>In var... </var> After...",
		"Before… <var>In var... </var> After…",
		TOK(text, "Before... "), TOK(block, "<var>"), TOK(text_raw, "In var... "), TOK(block, "</var>"), TOK(text, " After..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... <math>In math... </math> After...",
		"Before… <math>In math... </math> After…",
		TOK(text, "Before... "), TOK(block, "<math>"), TOK(text_raw, "In math... "), TOK(block, "</math>"), TOK(text, " After..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... <textarea>In textarea... </textarea> After...",
		"Before… <textarea>In textarea... </textarea> After…",
		TOK(text, "Before... "), TOK(block, "<textarea>"), TOK(text_raw, "In textarea... "), TOK(block, "</textarea>"), TOK(text, " After..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... yes. <p>In something else (like p)... yes!</p> After... yes.",
		"Before… yes. <p>In something else (like p)… yes!</p> After… yes.",
		TOK(text, "Before... yes. "), TOK(block, "<p>"), TOK(text, "In something else (like p)... yes!"), TOK(block, "</p>"), TOK(text, " After... yes."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... <pre>In pre... <code>In code...</code> In pre...</pre> After...",
		"Before… <pre>In pre... <code>In code...</code> In pre...</pre> After…",
		TOK(text, "Before... "), TOK(block, "<pre>"), TOK(text_raw, "In pre... "), TOK(inline, "<code>"), TOK(text_raw, "In code..."), TOK(inline, "</code>"), TOK(text_raw, " In pre..."), TOK(block, "</pre>"), TOK(text, " After..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Some <em>text</em> wrapped in <span class=\"meh\">elements</span>.",
		"Some <em>text</em> wrapped in <span class=\"meh\">elements</span>.",
		TOK(text, "Some "), TOK(inline, "<em>"), TOK(text, "text"), TOK(inline, "</em>"), TOK(text, " wrapped in "), TOK(inline, "<span class=\"meh\">"), TOK(text, "elements"), TOK(inline, "</span>"), TOK(text, "."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... <codezzz>during...</codezzz> after...",
		"Before… <codezzz>during…</codezzz> after…",
		TOK(text, "Before... "), TOK(inline, "<codezzz>"), TOK(text, "during..."), TOK(inline, "</codezzz>"), TOK(text, " after..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... <kbdzzz>during...</kbdzzz> after...",
		"Before… <kbdzzz>during…</kbdzzz> after…",
		TOK(text, "Before... "), TOK(inline, "<kbdzzz>"), TOK(text, "during..."), TOK(inline, "</kbdzzz>"), TOK(text, " after..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... <prezzz>during...</prezzz> after...",
		"Before… <prezzz>during…</prezzz> after…",
		TOK(text, "Before... "), TOK(inline, "<prezzz>"), TOK(text, "during..."), TOK(inline, "</prezzz>"), TOK(text, " after..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... <scriptzzz>during...</scriptzzz> after...",
		"Before… <scriptzzz>during…</scriptzzz> after…",
		TOK(text, "Before... "), TOK(inline, "<scriptzzz>"), TOK(text, "during..."), TOK(inline, "</scriptzzz>"), TOK(text, " after..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... <zzzcode>during...</zzzcode> after...",
		"Before… <zzzcode>during…</zzzcode> after…",
		TOK(text, "Before... "), TOK(inline, "<zzzcode>"), TOK(text, "during..."), TOK(inline, "</zzzcode>"), TOK(text, " after..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... <zzzkbd>during...</zzzkbd> after...",
		"Before… <zzzkbd>during…</zzzkbd> after…",
		TOK(text, "Before... "), TOK(inline, "<zzzkbd>"), TOK(text, "during..."), TOK(inline, "</zzzkbd>"), TOK(text, " after..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... <zzzpre>during...</zzzpre> after...",
		"Before… <zzzpre>during…</zzzpre> after…",
		TOK(text, "Before... "), TOK(inline, "<zzzpre>"), TOK(text, "during..."), TOK(inline, "</zzzpre>"), TOK(text, " after..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"Before... <zzzscript>during...</zzzscript> after...",
		"Before… <zzzscript>during…</zzzscript> after…",
		TOK(text, "Before... "), TOK(inline, "<zzzscript>"), TOK(text, "during..."), TOK(inline, "</zzzscript>"), TOK(text, " after..."), fa_null_token);

	fast_aleck_test(&test_suite,
		"<script>",
		"<script>",
		TOK(inline, "<script>"), fa_null_token);

	fast_aleck_test(&test_suite,
		"</script>",
		"</script>",
		TOK(inline, "</script>"), fa_null_token);

	fast_aleck_test(&test_suite,
		"<script>a",
		"<script>a",
		TOK(inline, "<script>"), TOK(text, "a"), fa_null_token);

	fast_aleck_test(&test_suite,
		"\"<a href=\"#\">blah</a>\"",
		"“<a href=\"#\">blah</a>”",
		TOK(text, "\""), TOK(inline, "<a href=\"#\">"), TOK(text, "blah"), TOK(inline, "</a>"), TOK(text, "\""), fa_null_token);

	fast_aleck_test(&test_suite,
		"<a href=\"#\">\"blah\"</a>",
		"<a href=\"#\">“blah”</a>",
		TOK(inline, "<a href=\"#\">"), TOK(text, "\"blah\""), TOK(inline, "</a>"), fa_null_token);

	fast_aleck_test(&test_suite,
		"\"blah <a href=\"#\">blah</a>\"",
		"“blah <a href=\"#\">blah</a>”",
		TOK(text, "\"blah "), TOK(inline, "<a href=\"#\">"), TOK(text, "blah"), TOK(inline, "</a>"), TOK(text, "\""), fa_null_token);

	fast_aleck_test(&test_suite,
		"\"<div>blah</div>\"",
		"“<div>blah</div>“",
		TOK(text, "\""), TOK(block, "<div>"), TOK(text, "blah"), TOK(block, "</div>"), TOK(text, "\""), fa_null_token);

	fast_aleck_test(&test_suite,
		"<div>\"blah\"</div>",
		"<div>“blah”</div>",
		TOK(block, "<div>"), TOK(text, "\"blah\""), TOK(block, "</div>"), fa_null_token);

	fast_aleck_test(&test_suite,
		"\"blah <div>blah</div>\"",
		"“blah <div>blah</div>“",
		TOK(text, "\"blah "), TOK(block, "<div>"), TOK(text, "blah"), TOK(block, "</div>"), TOK(text, "\""), fa_null_token);

	fast_aleck_test(&test_suite,
		"foo </div",
		"foo </div",
		TOK(text, "foo "), TOK(block, "</div"), fa_null_token);

	fast_aleck_test(&test_suite,
		"foo <div class='meh",
		"foo <div class='meh",
		TOK(text, "foo "), TOK(block, "<div class='meh"), fa_null_token);

	fast_aleck_test(&test_suite,
		"foo <div class=\"meh",
		"foo <div class=\"meh",
		TOK(text, "foo "), TOK(block, "<div class=\"meh"), fa_null_token);

	fast_aleck_test(&test_suite,
		"&' / &a' / &am' / &amp' / &amp;'",
		"&’ / &a’ / &am’ / &amp’ / &amp;’",
		TOK(text, "&' / &a' / &am' / &amp' / &amp;'"), fa_null_token);

	fast_aleck_test(&test_suite,
		".' / ..'",
		".’ / ..’",
		TOK(text, ".' / ..'"), fa_null_token);

	fast_aleck_test(&test_suite,
		"-' / --' / ---'",
		"-’ / —’ / —’",
		TOK(text, "-' / --' / ---'"), fa_null_token);

	// WRAP AMPS TESTS

	test_suite.wrap_amps = 1;

	fast_aleck_test(&test_suite,
		"Four &lt; Seventeen",
		"Four &lt; Seventeen",
		TOK(text, "Four &lt; Seventeen"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Four &alt; Seventeen",
		"Four &alt; Seventeen",
		TOK(text, "Four &alt; Seventeen"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Four &amt; Seventeen",
		"Four &amt; Seventeen",
		TOK(text, "Four &amt; Seventeen"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Four &ampx; Seventeen",
		"Four &ampx; Seventeen",
		TOK(text, "Four &ampx; Seventeen"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Four &amp Seventeen",
		"Four &amp Seventeen",
		TOK(text, "Four &amp Seventeen"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Four &amp; Seventeen",
		"Four <span class=\"amp\">&amp;</span> Seventeen",
		TOK(text, "Four &amp; Seventeen"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Ampersands &amp; More",
		"Ampersands <span class=\"amp\">&amp;</span> More",
		TOK(text, "Ampersands &amp; More"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Ampersands &</span>",
		"Ampersands &</span>",
		TOK(text, "Ampersands &"), TOK(inline, "</span>"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Ampersands &a</span>",
		"Ampersands &a</span>",
		TOK(text, "Ampersands &a"), TOK(inline, "</span>"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Ampersands &am</span>",
		"Ampersands &am</span>",
		TOK(text, "Ampersands &am"), TOK(inline, "</span>"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Ampersands &amp</span>",
		"Ampersands &amp</span>",
		TOK(text, "Ampersands &amp"), TOK(inline, "</span>"), fa_null_token);

	fast_aleck_test(&test_suite,
		"Ampersands &amp;</span>",
		"Ampersands <span class=\"amp\">&amp;</span></span>",
		TOK(text, "Ampersands &amp;"), TOK(inline, "</span>"), fa_null_token);

	fast_aleck_test(&test_suite,
		"A &amp; B <title>C &amp; D</title> E &amp; F",
		"A <span class=\"amp\">&amp;</span> B <title>C &amp; D</title> E <span class=\"amp\">&amp;</span> F",
		TOK(text, "A &amp; B "), TOK(inline, "<title>"), TOK(text, "C &amp; D"), TOK(inline, "</title>"), TOK(text, " E &amp; F"), fa_null_token);

	test_suite.wrap_amps = 0;

	// WRAP QUOTES TESTS

	test_suite.wrap_quotes = 1;

	fast_aleck_test(&test_suite,
		"There's a hole in the sky.",
		"There’s a hole in the sky.",
		TOK(text, "There's a hole in the sky."), fa_null_token);

	fast_aleck_test(&test_suite,
		"'There's a hole in the sky', he said. 'Don't be silly', she said.",
		"<span class=\"quo\">‘</span>There’s a hole in the sky’, he said. ‘Don’t be silly’, she said.",
		TOK(text, "'There's a hole in the sky', he said. 'Don't be silly', she said."), fa_null_token);

	fast_aleck_test(&test_suite,
		"\"There's a hole in the sky\", he said. \"Don't be silly\", she said.",
		"<span class=\"dquo\">“</span>There’s a hole in the sky”, he said. “Don’t be silly”, she said.",
		TOK(text, "\"There's a hole in the sky\", he said. \"Don't be silly\", she said."), fa_null_token);

	fast_aleck_test(&test_suite,
		"\"Here.\"<p>\"Here.\" \"Not here.\"<p>\"Here.\"",
		"<span class=\"dquo\">“</span>Here.”<p><span class=\"dquo\">“</span>Here.” “Not here.”<p><span class=\"dquo\">“</span>Here.”",
		TOK(text, "\"Here.\""), TOK(block, "<p>"), TOK(text, "\"Here.\" \"Not here.\""), TOK(block, "<p>"), TOK(text, "\"Here.\""), fa_null_token);

	fast_aleck_test(&test_suite,
		"\"Here.\"<li>\"Here.\" \"Not here.\"<li>\"Here.\"",
		"<span class=\"dquo\">“</span>Here.”<li><span class=\"dquo\">“</span>Here.” “Not here.”<li><span class=\"dquo\">“</span>Here.”",
		TOK(text, "\"Here.\""), TOK(block, "<li>"), TOK(text, "\"Here.\" \"Not here.\""), TOK(block, "<li>"), TOK(text, "\"Here.\""), fa_null_token);

	fast_aleck_test(&test_suite,
		"\"Here.\"<div>\"Here.\" \"Not here.\"<div>\"Here.\"",
		"<span class=\"dquo\">“</span>Here.”<div><span class=\"dquo\">“</span>Here.” “Not here.”<div><span class=\"dquo\">“</span>Here.”",
		TOK(text, "\"Here.\""), TOK(block, "<div>"), TOK(text, "\"Here.\" \"Not here.\""), TOK(block, "<div>"), TOK(text, "\"Here.\""), fa_null_token);

	fast_aleck_test(&test_suite,
		"<title>'There's a hole in the sky'</title>",
		"<title>‘There’s a hole in the sky’</title>",
		TOK(inline, "<title>"), TOK(text, "'There's a hole in the sky'"), TOK(inline, "</title>"), fa_null_token);

	// TODO h1, h2, ..., h6

	test_suite.wrap_quotes = 0;

	// WIDONT TESTS

	test_suite.widont = 1;

	fast_aleck_test(&test_suite,
		"<p>Foo bar baz. Woof meow moo.</p>",
		"<p>Foo bar baz. Woof meow&nbsp;moo.</p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>Foo-bar baz. </p>",
		"<p>Foo-bar&nbsp;baz. </p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>Foo bar baz.</p><p>Woof meow moo.</p>",
		"<p>Foo bar&nbsp;baz.</p><p>Woof meow&nbsp;moo.</p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<li>Foo bar baz.</li><li>Woof meow moo.</li>",
		"<li>Foo bar&nbsp;baz.</li><li>Woof meow&nbsp;moo.</li>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<div>Foo bar baz.</div><div>Woof meow moo.</div>",
		"<div>Foo bar&nbsp;baz.</div><div>Woof meow&nbsp;moo.</div>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<h1>Foo bar baz.</h1><h1>Woof meow moo.</h1>",
		"<h1>Foo bar&nbsp;baz.</h1><h1>Woof meow&nbsp;moo.</h1>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<h2>Foo bar baz.</h2><h2>Woof meow moo.</h2>",
		"<h2>Foo bar&nbsp;baz.</h2><h2>Woof meow&nbsp;moo.</h2>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<h3>Foo bar baz.</h3><h3>Woof meow moo.</h3>",
		"<h3>Foo bar&nbsp;baz.</h3><h3>Woof meow&nbsp;moo.</h3>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<h4>Foo bar baz.</h4><h4>Woof meow moo.</h4>",
		"<h4>Foo bar&nbsp;baz.</h4><h4>Woof meow&nbsp;moo.</h4>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<h5>Foo bar baz.</h5><h5>Woof meow moo.</h5>",
		"<h5>Foo bar&nbsp;baz.</h5><h5>Woof meow&nbsp;moo.</h5>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<h6>Foo bar baz.</h6><h6>Woof meow moo.</h6>",
		"<h6>Foo bar&nbsp;baz.</h6><h6>Woof meow&nbsp;moo.</h6>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<blockquote>Foo bar baz.</blockquote><blockquote>Woof meow moo.</blockquote>",
		"<blockquote>Foo bar&nbsp;baz.</blockquote><blockquote>Woof meow&nbsp;moo.</blockquote>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<dd>Foo bar baz.</dd><dd>Woof meow moo.</dd>",
		"<dd>Foo bar&nbsp;baz.</dd><dd>Woof meow&nbsp;moo.</dd>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<dt>Foo bar baz.</dt><dt>Woof meow moo.</dt>",
		"<dt>Foo bar&nbsp;baz.</dt><dt>Woof meow&nbsp;moo.</dt>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<ol>\n<li>This is a list item</li>\n</ol>",
		"<ol>\n<li>This is a list&nbsp;item</li>\n</ol>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<section>\n<h1>Title!</h1>\n</section>",
		"<section>\n<h1>Title!</h1>\n</section>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>Paragraph one</p>\n<p>Paragraph two</p>",
		"<p>Paragraph&nbsp;one</p>\n<p>Paragraph&nbsp;two</p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<dt>Foo bar baz.</dt>\n<dt>Woof meow moo.</dt>",
		"<dt>Foo bar&nbsp;baz.</dt>\n<dt>Woof meow&nbsp;moo.</dt>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<li><a href=\"../\"><span>Home</span></a></li>\n<li><a href=\"../blog/\"><span>Blog</span></a></li>",
		"<li><a href=\"../\"><span>Home</span></a></li>\n<li><a href=\"../blog/\"><span>Blog</span></a></li>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<li><span>line one</span></li>\n<li><span>line two</span></li>",
		"<li><span>line&nbsp;one</span></li>\n<li><span>line&nbsp;two</span></li>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"A B C D",
		"A B C&nbsp;D",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<h1><a href='#'>A B C D</a></h1>",
		"<h1><a href='#'>A B C&nbsp;D</a></h1>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>This paragraph should <em>not at all</em> be interrupted!</p>",
		"<p>This paragraph should <em>not at all</em> be&nbsp;interrupted!</p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<h1><a href='#'>why won't this widon't</a></h1>",
		"<h1><a href='#'>why won’t this&nbsp;widon’t</a></h1>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<h1><a href='blablabla'>why won't this widon't</a></h1>",
		"<h1><a href='blablabla'>why won’t this&nbsp;widon’t</a></h1>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>foo bar<br>baz qux</p>",
		"<p>foo&nbsp;bar<br>baz&nbsp;qux</p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>one<br>\ntwo</p>",
		"<p>one<br>\ntwo</p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>one<br>two<br>three</p>",
		"<p>one<br>two<br>three</p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>one\ntwo\nthree</p>",
		"<p>one\ntwo&nbsp;three</p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<pre>pre blocks are sacred</pre>",
		"<pre>pre blocks are sacred</pre>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<a href='blah' title='blah' more='stuff'></pre>",
		"<a href='blah' title='blah' more='stuff'></pre>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>foo bar\n<br>\nbaz</p>",
		"<p>foo bar\n<br>\nbaz</p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>foo bar </p>",
		"<p>foo bar </p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>foo bar baz </p>",
		"<p>foo bar&nbsp;baz </p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>foo  bar</p>",
		"<p>foo&nbsp;bar</p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p> foo</p>",
		"<p> foo</p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>foo &amp;</p>",
		"<p>foo&nbsp;&amp;</p>",
		fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>foo &amp; bar</p>",
		"<p>foo &amp;&nbsp;bar</p>",
		fa_null_token);

	test_suite.widont = 0;

	fast_aleck_test(&test_suite,
		"<dt>We don't Widon't.</dt>",
		"<dt>We don’t Widon’t.</dt>",
		fa_null_token);

	// WRAP CAPS TEST

	test_suite.wrap_caps = 1;

	fast_aleck_test(&test_suite,
		"Hello, this is DENIS speaking!",
		"Hello, this is <span class=\"caps\">DENIS</span> speaking!",
		TOK(text, "Hello, this is DENIS speaking!"), fa_null_token);

	fast_aleck_test(&test_suite,
		"DENIS's pants.",
		"<span class=\"caps\">DENIS</span>’s pants.",
		TOK(text, "DENIS's pants."), fa_null_token);

	fast_aleck_test(&test_suite,
		"I have 13 EC2 instances but no static AMIs.",
		"I have 13 <span class=\"caps\">EC2</span> instances but no static <span class=\"caps\">AMI</span>s.",
		TOK(text, "I have 13 EC2 instances but no static AMIs."), fa_null_token);

	fast_aleck_test(&test_suite,
		"<title>Hello, this is DENIS speaking!</title>",
		"<title>Hello, this is DENIS speaking!</title>",
		TOK(inline, "<title>"), TOK(text_no_html, "Hello, this is DENIS speaking!"), TOK(inline, "</title>"), fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>MongoDB is better than PostgreSQL</p>",
		"<p>Mongo<span class=\"caps\">DB</span> is better than Postgre<span class=\"caps\">SQL</span></p>",
		TOK(block, "<p>"), TOK(text, "MongoDB is better than PostgreSQL"), TOK(block, "</p>"), fa_null_token);

	fast_aleck_test(&test_suite,
		"<p>HTML entities! &copy; &COPY; &#x21A9;</p>",
		"<p><span class=\"caps\">HTML</span> entities! &copy; &COPY; &#x21A9;</p>",
		TOK(block, "<p>"), TOK(text, "MongoDB is better than PostgreSQL"), TOK(block, "</p>"), fa_null_token);

	// COMBINATION TESTS

	test_suite.wrap_caps   = 1;
	test_suite.widont      = 1;
	test_suite.wrap_amps   = 0;
	test_suite.wrap_quotes = 0;
	fast_aleck_test(&test_suite,
		"<p>One Two <b>THR</b>EE!</p>",
		"<p>One Two <b><span class=\"caps\">THR</span></b><span class=\"caps\">EE</span>!</p>",
		TOK(block, "<p>"), TOK(text, "One Two "), TOK(inline, "<b>"), TOK(text, "THR"), TOK(inline, "</b>"), TOK(text, "EE!"), TOK(block, "</p>"), fa_null_token);

	test_suite.wrap_caps = 0;

	fast_aleck_test(&test_suite,
		"Do NOT wrap caps if I don't ask to!",
		"Do NOT wrap caps if I don’t ask&nbsp;to!",
		TOK(text, "Do NOT wrap caps if I don't ask to!"), fa_null_token);

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

static void fast_aleck_test(struct fa_test_suite *a_test_suite, char *a_input, char *a_expected_output, ...) {
	fast_aleck_config config;
	fast_aleck_config_init(&config);
	config.wrap_amps   = a_test_suite->wrap_amps;
	config.wrap_caps   = a_test_suite->wrap_caps;
	config.wrap_quotes = a_test_suite->wrap_quotes;
	config.widont      = a_test_suite->widont;

	size_t out_len;
	char *actual_output = fast_aleck(config, a_input, strlen(a_input), &out_len);

	// check content
	if (0 != strcmp(a_expected_output, actual_output)) {
		++a_test_suite->fails;
		fprintf(stdout, "not ok %i ", a_test_suite->count+1);
		_fa_puts_escaped(a_input);
		fprintf(stdout, "  Expected: ");
		_fa_puts_escaped(a_expected_output);
		fprintf(stdout, "  Actual:   ");
		_fa_puts_escaped(actual_output);
		fprintf(stdout, "            ");
		size_t min_len = strlen(a_input);
		if (out_len < min_len)
			min_len = out_len;
		int special_char_len = 0;
		for (size_t i = 0; i < min_len; ++i) {
			bool is_special_char = a_expected_output[i] < 0;
			bool should_print = !is_special_char || special_char_len == 1;
			if (is_special_char) {
				special_char_len++;
			} else {
				special_char_len = 0;
			}
			if (should_print) {
				if (a_expected_output[i] != actual_output[i]) {
					fputs("^-----", stdout);
					break;
				} else {
					fputc(' ', stdout);
				}
			}
		}
		puts("");
		goto bail;
	}

	// check length
	if (strlen(actual_output) != out_len) {
		++a_test_suite->fails;
		fprintf(stdout, "not ok %i ", a_test_suite->count+1);
		_fa_puts_escaped(a_input);
		fprintf(stdout, "  Length of returned string: %lu\n", (unsigned long) strlen(actual_output));
		fprintf(stdout, "  Returned length of string: %lu\n", (unsigned long) out_len);
		goto bail;
	}

#if FAST_ALECK_TEST_CONFIG_CHECK_TOKENIZATION
	// tokenize
	fa_token_buffer actual_token_buffer;
	fa_token_buffer_init(&actual_token_buffer, 10);
	fa_state state; // FIXME don't require a whole state! ugly! ew ew ew!
	fa_tokenizer_state_init(&state.tokenizer_state, a_input, strlen(a_input));
	state.tokenizer_state._token_buffer = &actual_token_buffer;
	fa_tokenizer_run(&state);

	// build token buffer from arguments
	fa_token_buffer expected_token_buffer;
	fa_token_buffer_init(&expected_token_buffer, 10);
	va_list ap;
	va_start(ap, a_expected_output);
	for (;;) {
		fa_token expected_token = va_arg(ap, fa_token);
		if (NULL == expected_token.slice.start) {
			break;
		}
		fa_token_buffer_append(&expected_token_buffer, expected_token);
	}
	va_end(ap);

	// check tokenization
	size_t expected_count = fa_token_buffer_count(&expected_token_buffer);
	size_t actual_count   = fa_token_buffer_count(&actual_token_buffer);
	bool has_no_end_token = actual_count - 1 != expected_count || (actual_count > 0 && fa_token_buffer_at(&actual_token_buffer, actual_count - 1).type != fa_token_type_end);
	if (expected_count != actual_count && has_no_end_token) {
		++a_test_suite->fails;
		fprintf(stdout, "not ok %i ", a_test_suite->count+1);
		_fa_puts_escaped(a_input);
		if (0 == expected_count) {
			fprintf(stdout, "  Result OK, but no expected tokenization given\n");
		} else {
			fprintf(stdout, "  Expected number of tokens: %lu\n", (unsigned long) expected_count);
			fprintf(stdout, "  Expected tokens:\n");
			for (size_t i = 0; i < expected_count; ++i) {
				fa_token expected_token = fa_token_buffer_at(&expected_token_buffer, i);
				fprintf(stdout, "    ");
				fa_token_print(expected_token, stdout);
				fprintf(stdout, " ");
				puts("");
			}
			fprintf(stdout, "  Actual number of tokens:   %lu\n", (unsigned long) actual_count);
			fprintf(stdout, "  Actual tokens:  \n");
			for (size_t i = 0; i < actual_count; ++i) {
				fa_token actual_token = fa_token_buffer_at(&actual_token_buffer, i);
				fprintf(stdout, "    ");
				fa_token_print(actual_token, stdout);
				fprintf(stdout, " ");
				puts("");
			}
		}
		goto bail;
	}
	for (size_t i = 0; i < actual_count; ++i) {
		fa_token expected_token = fa_token_buffer_at(&expected_token_buffer, i);
		fa_token actual_token   = fa_token_buffer_at(&actual_token_buffer, i);
		// FIXME assert token types equal
		if (expected_token.slice.length != actual_token.slice.length || 0 != strncmp(expected_token.slice.start, actual_token.slice.start, expected_token.slice.length)) {
			++a_test_suite->fails;
			fprintf(stdout, "not ok %i ", a_test_suite->count+1);
			_fa_puts_escaped(a_input);
			fprintf(stdout, "  Expected token %lu: ", (unsigned long) i);
			fa_token_print(expected_token, stdout);
			puts("");
			fprintf(stdout, "  Actual token %lu:   ", (unsigned long) i);
			fa_token_print(actual_token, stdout);
			puts("");
			goto bail;
		}
	}
#endif

	// ok
	++a_test_suite->passes;
	fprintf(stdout, "ok %i ", a_test_suite->count+1);
	_fa_puts_escaped(a_input);

bail:
	++a_test_suite->count;
	free(actual_output);
}
