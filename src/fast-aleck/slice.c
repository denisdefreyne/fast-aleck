#include <fast-aleck/slice.h>

const fa_slice fa_null_slice = { .start = NULL, .length = 0 };

void fa_slice_print(fa_slice slice, FILE *file) {
	fputs("fa_slice(\"", file);

	for (char *s = slice.start; slice.length > 0; --slice.length, ++s) {
		fputc(*s, file);
	}

	fputs("\")", file);
}
