#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fast-aleck/fast-aleck.h>

struct fast_aleck_test_case {
	int count;
	int passes;
	int fails;

	char wrap_amps;
	char wrap_quotes;
};

void fast_aleck_test(struct fast_aleck_test_case *a_test_case, char *a_input, char *a_expected_output);

int main(void)
{
	struct fast_aleck_test_case test_case;
	test_case.count       = 0;
	test_case.passes      = 0;
	test_case.fails       = 0;
	test_case.wrap_amps   = 0;
	test_case.wrap_quotes = 0;

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

	test_case.wrap_amps = 1;

	fast_aleck_test(&test_case,
		"Four &lt; Seventeen",
		"Four &lt; Seventeen");

	fast_aleck_test(&test_case,
		"Ampersands &amp; More",
		"Ampersands <span class=\"amp\">&amp;</span> More");

	return (test_case.fails > 0 ? 1 : 0);
}

void fast_aleck_test(struct fast_aleck_test_case *a_test_case, char *a_input, char *a_expected_output)
{
	fast_aleck_config config;
	config.wrap_amps   = a_test_case->wrap_amps;
	config.wrap_quotes = a_test_case->wrap_quotes;

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
