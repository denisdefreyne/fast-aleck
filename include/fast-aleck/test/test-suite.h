#ifndef __FAST_ALECK_TEST_TEST_SUITE_H__
#define __FAST_ALECK_TEST_TEST_SUITE_H__

#include <stdbool.h>

struct fa_test_suite {
	int count;
	int passes;
	int fails;

	char wrap_amps;
	char wrap_caps;
	char wrap_quotes;
	char widont;
};

void succeed(struct fa_test_suite *test_suite);
void fail(struct fa_test_suite *test_suite);
void expect(bool condition, struct fa_test_suite *test_suite);

#endif
