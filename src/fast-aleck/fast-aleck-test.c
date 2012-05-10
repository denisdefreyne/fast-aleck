#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fast-aleck/fast-aleck.h>

struct fast_aleck_test_case {
	int count;
	int passes;
	int fails;

	char wrap_amps;
	char wrap_caps;
	char wrap_quotes;
	char widont;
};

void fast_aleck_test(struct fast_aleck_test_case *a_test_case, char *a_input, char *a_expected_output);

int main(void)
{
	struct fast_aleck_test_case test_case;
	test_case.count       = 0;
	test_case.passes      = 0;
	test_case.fails       = 0;
	test_case.wrap_amps   = 0;
	test_case.wrap_caps   = 0;
	test_case.wrap_quotes = 0;
	test_case.widont      = 0;

	fprintf(stdout, "1..%i\n", test_case.count);

	fast_aleck_test(&test_case,
		"I am a simple sentence.",
		"I am a simple sentence.");

	fast_aleck_test(&test_case,
		"I am... a sentence with an ellipsis!",
		"I am… a sentence with an ellipsis!");

	fast_aleck_test(&test_case,
		"I am a sentence--really---with two dashes!",
		"I am a sentence—really—with two dashes!");

	fast_aleck_test(&test_case,
		"This sentence ends in two periods..",
		"This sentence ends in two periods..");

	fast_aleck_test(&test_case,
		"This sentence ends in three periods...",
		"This sentence ends in three periods…");

	fast_aleck_test(&test_case,
		"Weird! This sentence ends in a dash-",
		"Weird! This sentence ends in a dash-");

	fast_aleck_test(&test_case,
		"Weird! This sentence ends in two dashes--",
		"Weird! This sentence ends in two dashes—");

	fast_aleck_test(&test_case,
		"Weird! This sentence ends in three dashes---",
		"Weird! This sentence ends in three dashes—");

	fast_aleck_test(&test_case,
		"My sister's book. My sisters' books.",
		"My sister’s book. My sisters’ books.");

	fast_aleck_test(&test_case,
		"'Hello', she said. 'Hello,' she said. 'Huh...' she mumbled.",
		"‘Hello’, she said. ‘Hello,’ she said. ‘Huh…’ she mumbled.");

	fast_aleck_test(&test_case,
		"\"Hello\", she said. \"Hello,\" she said. \"Huh...\" she mumbled.",
		"“Hello”, she said. “Hello,” she said. “Huh…” she mumbled.");

	fast_aleck_test(&test_case,
		"'That's mine!', she said. \"That's mine!\", she said.",
		"‘That’s mine!’, she said. “That’s mine!”, she said.");

	fast_aleck_test(&test_case,
		"(\"Amazing!\" she thought.) ('Amazing!' she thought.)",
		"(“Amazing!” she thought.) (‘Amazing!’ she thought.)");

	fast_aleck_test(&test_case,
		"Before... yes. <img alt=\"During... no.\"> After... yes.",
		"Before… yes. <img alt=\"During... no.\"> After… yes.");

	fast_aleck_test(&test_case,
		"Before... yes. <script>In script... no.</script> After... yes.",
		"Before… yes. <script>In script... no.</script> After… yes.");

	fast_aleck_test(&test_case,
		"Before... yes. <pre>In pre... no.</pre> After... yes.",
		"Before… yes. <pre>In pre... no.</pre> After… yes.");

	fast_aleck_test(&test_case,
		"Before... yes. <code>In code... no.</code> After... yes.",
		"Before… yes. <code>In code... no.</code> After… yes.");

	fast_aleck_test(&test_case,
		"Before... yes. <kbd>In kbd... no.</kbd> After... yes.",
		"Before… yes. <kbd>In kbd... no.</kbd> After… yes.");

	fast_aleck_test(&test_case,
		"Before... yes. <p>In something else (like p)... yes!</p> After... yes.",
		"Before… yes. <p>In something else (like p)… yes!</p> After… yes.");

	fast_aleck_test(&test_case,
		"Before... <pre>In pre... <code>In code...</code> In pre...</pre> After...",
		"Before… <pre>In pre... <code>In code...</code> In pre...</pre> After…");

	fast_aleck_test(&test_case,
		"Some <em>text</em> wrapped in <span class=\"meh\">elements</span>.",
		"Some <em>text</em> wrapped in <span class=\"meh\">elements</span>.");

	fast_aleck_test(&test_case,
		"Before... <codezzz>during...</codezzz> after...",
		"Before… <codezzz>during…</codezzz> after…");

	fast_aleck_test(&test_case,
		"Before... <kbdzzz>during...</kbdzzz> after...",
		"Before… <kbdzzz>during…</kbdzzz> after…");

	fast_aleck_test(&test_case,
		"Before... <prezzz>during...</prezzz> after...",
		"Before… <prezzz>during…</prezzz> after…");

	fast_aleck_test(&test_case,
		"Before... <scriptzzz>during...</scriptzzz> after...",
		"Before… <scriptzzz>during…</scriptzzz> after…");

	fast_aleck_test(&test_case,
		"Before... <zzzcode>during...</zzzcode> after...",
		"Before… <zzzcode>during…</zzzcode> after…");

	fast_aleck_test(&test_case,
		"Before... <zzzkbd>during...</zzzkbd> after...",
		"Before… <zzzkbd>during…</zzzkbd> after…");

	fast_aleck_test(&test_case,
		"Before... <zzzpre>during...</zzzpre> after...",
		"Before… <zzzpre>during…</zzzpre> after…");

	fast_aleck_test(&test_case,
		"Before... <zzzscript>during...</zzzscript> after...",
		"Before… <zzzscript>during…</zzzscript> after…");

	fast_aleck_test(&test_case,
		"<script>",
		"<script>");

	fast_aleck_test(&test_case,
		"</script>",
		"</script>");

	fast_aleck_test(&test_case,
		"<script>a",
		"<script>a");

	fast_aleck_test(&test_case,
		"\"<a href=\"#\">blah</a>\"",
		"“<a href=\"#\">blah</a>”");

	fast_aleck_test(&test_case,
		"<a href=\"#\">\"blah\"</a>",
		"<a href=\"#\">“blah”</a>");

	fast_aleck_test(&test_case,
		"\"blah <a href=\"#\">blah</a>\"",
		"“blah <a href=\"#\">blah</a>”");

	fast_aleck_test(&test_case,
		"\"<div>blah</div>\"",
		"“<div>blah</div>“");

	fast_aleck_test(&test_case,
		"<div>\"blah\"</div>",
		"<div>“blah”</div>");

	fast_aleck_test(&test_case,
		"\"blah <div>blah</div>\"",
		"“blah <div>blah</div>“");

	// WRAP AMPS TESTS

	test_case.wrap_amps = 1;

	fast_aleck_test(&test_case,
		"Four &lt; Seventeen",
		"Four &lt; Seventeen");

	fast_aleck_test(&test_case,
		"Ampersands &amp; More",
		"Ampersands <span class=\"amp\">&amp;</span> More");

	fast_aleck_test(&test_case,
		"A &amp; B <title>C &amp; D</title> E &amp; F",
		"A <span class=\"amp\">&amp;</span> B <title>C &amp; D</title> E <span class=\"amp\">&amp;</span> F");

	test_case.wrap_amps = 0;

	// WRAP QUOTES TESTS

	test_case.wrap_quotes = 1;

	fast_aleck_test(&test_case,
		"There's a hole in the sky.",
		"There’s a hole in the sky.");

	fast_aleck_test(&test_case,
		"'There's a hole in the sky', he said. 'Don't be silly', she said.",
		"<span class=\"quo\">‘</span>There’s a hole in the sky’, he said. ‘Don’t be silly’, she said.");

	fast_aleck_test(&test_case,
		"\"There's a hole in the sky\", he said. \"Don't be silly\", she said.",
		"<span class=\"dquo\">“</span>There’s a hole in the sky”, he said. “Don’t be silly”, she said.");

	fast_aleck_test(&test_case,
		"\"Here.\"<p>\"Here.\" \"Not here.\"<p>\"Here.\"",
		"<span class=\"dquo\">“</span>Here.”<p><span class=\"dquo\">“</span>Here.” “Not here.”<p><span class=\"dquo\">“</span>Here.”");

	fast_aleck_test(&test_case,
		"\"Here.\"<li>\"Here.\" \"Not here.\"<li>\"Here.\"",
		"<span class=\"dquo\">“</span>Here.”<li><span class=\"dquo\">“</span>Here.” “Not here.”<li><span class=\"dquo\">“</span>Here.”");

	fast_aleck_test(&test_case,
		"\"Here.\"<div>\"Here.\" \"Not here.\"<div>\"Here.\"",
		"<span class=\"dquo\">“</span>Here.”<div><span class=\"dquo\">“</span>Here.” “Not here.”<div><span class=\"dquo\">“</span>Here.”");

	fast_aleck_test(&test_case,
		"<title>'There's a hole in the sky'</title>",
		"<title>‘There’s a hole in the sky’</title>");

	// TODO h1, h2, ..., h6

	test_case.wrap_quotes = 0;

	// WIDONT TESTS

	test_case.widont = 1;

	fast_aleck_test(&test_case,
		"<p>Foo bar baz.</p><p>Woof meow moo.</p>",
		"<p>Foo bar&nbsp;baz.</p><p>Woof meow&nbsp;moo.</p>");

	fast_aleck_test(&test_case,
		"<li>Foo bar baz.</li><li>Woof meow moo.</li>",
		"<li>Foo bar&nbsp;baz.</li><li>Woof meow&nbsp;moo.</li>");

	fast_aleck_test(&test_case,
		"<div>Foo bar baz.</div><div>Woof meow moo.</div>",
		"<div>Foo bar&nbsp;baz.</div><div>Woof meow&nbsp;moo.</div>");

	fast_aleck_test(&test_case,
		"<h1>Foo bar baz.</h1><h1>Woof meow moo.</h1>",
		"<h1>Foo bar&nbsp;baz.</h1><h1>Woof meow&nbsp;moo.</h1>");

	fast_aleck_test(&test_case,
		"<h2>Foo bar baz.</h2><h2>Woof meow moo.</h2>",
		"<h2>Foo bar&nbsp;baz.</h2><h2>Woof meow&nbsp;moo.</h2>");

	fast_aleck_test(&test_case,
		"<h3>Foo bar baz.</h3><h3>Woof meow moo.</h3>",
		"<h3>Foo bar&nbsp;baz.</h3><h3>Woof meow&nbsp;moo.</h3>");

	fast_aleck_test(&test_case,
		"<h4>Foo bar baz.</h4><h4>Woof meow moo.</h4>",
		"<h4>Foo bar&nbsp;baz.</h4><h4>Woof meow&nbsp;moo.</h4>");

	fast_aleck_test(&test_case,
		"<h5>Foo bar baz.</h5><h5>Woof meow moo.</h5>",
		"<h5>Foo bar&nbsp;baz.</h5><h5>Woof meow&nbsp;moo.</h5>");

	fast_aleck_test(&test_case,
		"<h6>Foo bar baz.</h6><h6>Woof meow moo.</h6>",
		"<h6>Foo bar&nbsp;baz.</h6><h6>Woof meow&nbsp;moo.</h6>");

	fast_aleck_test(&test_case,
		"<blockquote>Foo bar baz.</blockquote><blockquote>Woof meow moo.</blockquote>",
		"<blockquote>Foo bar&nbsp;baz.</blockquote><blockquote>Woof meow&nbsp;moo.</blockquote>");

	fast_aleck_test(&test_case,
		"<dd>Foo bar baz.</dd><dd>Woof meow moo.</dd>",
		"<dd>Foo bar&nbsp;baz.</dd><dd>Woof meow&nbsp;moo.</dd>");

	fast_aleck_test(&test_case,
		"<dt>Foo bar baz.</dt><dt>Woof meow moo.</dt>",
		"<dt>Foo bar&nbsp;baz.</dt><dt>Woof meow&nbsp;moo.</dt>");

	test_case.widont = 0;

	// WRAP CAPS TEST

	test_case.wrap_caps = 1;

	fast_aleck_test(&test_case,
		"Hello, this is DENIS speaking!",
		"Hello, this is <span class=\"caps\">DENIS</span> speaking!");

	fast_aleck_test(&test_case,
		"I have 13 EC2 instances but no static AMIs.",
		"I have 13 <span class=\"caps\">EC2</span> instances but no static <span class=\"caps\">AMI</span>s.");

	fast_aleck_test(&test_case,
		"<title>Hello, this is DENIS speaking!</title>",
		"<title>Hello, this is DENIS speaking!</title>");

	test_case.wrap_caps = 0;

	return (test_case.fails > 0 ? 1 : 0);
}

void fast_aleck_test(struct fast_aleck_test_case *a_test_case, char *a_input, char *a_expected_output)
{
	fast_aleck_config config;
	fast_aleck_config_init(&config);
	config.wrap_amps   = a_test_case->wrap_amps;
	config.wrap_caps   = a_test_case->wrap_caps;
	config.wrap_quotes = a_test_case->wrap_quotes;
	config.widont      = a_test_case->widont;

	size_t out_len;
	char *actual_output = fast_aleck(config, a_input, strlen(a_input), &out_len);

	if (0 != strcmp(a_expected_output, actual_output))
	{
		++a_test_case->fails;
		fprintf(stdout, "not ok %i %s\n", a_test_case->count+1, a_input);
		fprintf(stdout, "  Expected: %s\n", a_expected_output);
		fprintf(stdout, "  Actual:   %s\n", actual_output);
	}
	else if(strlen(actual_output) != out_len)
	{
		++a_test_case->fails;
		fprintf(stdout, "not ok %i %s\n", a_test_case->count+1, a_input);
		fprintf(stdout, "  Length of returned string: %lu\n", strlen(actual_output));
		fprintf(stdout, "  Returned length of string: %lu\n", out_len);

	}
	else
	{
		++a_test_case->passes;
		fprintf(stdout, "ok %i %s\n", a_test_case->count+1, a_input);
	}

	++a_test_case->count;

	free(actual_output);
}
