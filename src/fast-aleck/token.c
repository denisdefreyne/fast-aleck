#include <fast-aleck/token.h>

const fa_token null_token = { .slice = { .start = NULL, .length = 0 }, .type = fa_token_type_undefined };

void fa_token_print(fa_token token, FILE *file) {
	fputs("fa_token(", file);

	char *type;
	switch (token.type) {
		case fa_token_type_text:
			type = "text";
			break;

		case fa_token_type_block:
			type = "block";
			break;

		case fa_token_type_inline:
			type = "inline";
			break;

		default:
			type = "???";
			break;
	}
	fprintf(file, "type = %6s, ", type);

	fputs("data = \"", file);
	for (char *s = token.slice.start; token.slice.length > 0; --token.slice.length, ++s) {
		fputc(*s, file);
	}

	fputs("\")", file);
}
