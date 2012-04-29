#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fast-aleck/fast-aleck.h>

struct fast_aleck_test_results {
	int passes;
	int fails;
};

void fast_aleck_test(struct fast_aleck_test_results *a_res, char *a_input, char *a_expected_output);

int main(void)
{
	struct fast_aleck_test_results res;
	res.passes = 0;
	res.fails  = 0;

	fast_aleck_test(&res,
		"I am a simple sentence.",
		"I am a simple sentence.");

	fast_aleck_test(&res,
		"I am... a sentence with an ellipsis!",
		"I am… a sentence with an ellipsis!");

	fast_aleck_test(&res,
		"I am a sentence--really---with two dashes!",
		"I am a sentence—really—with two dashes!");

	fast_aleck_test(&res,
		"This sentence ends in two periods..",
		"This sentence ends in two periods..");

	fast_aleck_test(&res,
		"This sentence ends in three periods...",
		"This sentence ends in three periods…");

	fast_aleck_test(&res,
		"Weird! This sentence ends in a dash-",
		"Weird! This sentence ends in a dash-");

	fast_aleck_test(&res,
		"Weird! This sentence ends in two dashes--",
		"Weird! This sentence ends in two dashes—");

	fast_aleck_test(&res,
		"Weird! This sentence ends in three dashes---",
		"Weird! This sentence ends in three dashes—");

	fprintf(stderr, "%i tests with %i failures\n", res.passes+res.fails, res.fails);
}

void fast_aleck_test(struct fast_aleck_test_results *a_res, char *a_input, char *a_expected_output)
{
	fast_aleck_config config;
	config.wrap_amps   = 0;
	config.wrap_quotes = 0;
	char *actual_output = fast_aleck(config, a_input, strlen(a_input));

	if (0 == strcmp(a_expected_output, actual_output))
	{
		++a_res->passes;
		fprintf(stderr, "PASS\n");
	}
	else
	{
		++a_res->fails;
		fprintf(stderr, "FAIL\n");
		fprintf(stderr, "  Expected: %s\n", a_expected_output);
		fprintf(stderr, "  Actual:   %s\n", actual_output);
	}

	free(actual_output);
}
