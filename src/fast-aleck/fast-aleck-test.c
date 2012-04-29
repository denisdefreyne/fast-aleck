#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fast-aleck/fast-aleck.h>

char *tests[] = {
	"I am a simple sentence.",
	"I am a simple sentence.",

	"I am... a sentence with an ellipsis!",
	"I am… a sentence with an ellipsis!",

	"I am a sentence--really---with two dashes!",
	"I am a sentence—really—with two dashes!",

	"This sentence ends in two periods..",
	"This sentence ends in two periods..",

	"This sentence ends in three periods...",
	"This sentence ends in three periods…",

	"Weird! This sentence ends in a dash-",
	"Weird! This sentence ends in a dash-",

	"Weird! This sentence ends in two dashes--",
	"Weird! This sentence ends in two dashes—",

	"Weird! This sentence ends in three dashes---",
	"Weird! This sentence ends in three dashes—",
};
int tests_count = 8;

int main(void)
{
	int passes = 0, fails = 0;

	for (int i = 0; i < tests_count; ++i)
	{
		char *input    = tests[2*i];
		char *expected = tests[2*i+1];
		char *actual   = NULL;

		fast_aleck_config config;
		actual = fast_aleck(config, input, strlen(input));

		if (0 == strcmp(expected, actual))
		{
			++passes;
			fprintf(stderr, "PASS\n");
		}
		else
		{
			++fails;
			fprintf(stderr, "FAIL\n");
			fprintf(stderr, "  Expected: %s\n", expected);
			fprintf(stderr, "  Actual:   %s\n", actual);
		}

		free(actual);
	}

	fprintf(stderr, "%i tests with %i failures\n", passes+fails, fails);

	return 0;
}
