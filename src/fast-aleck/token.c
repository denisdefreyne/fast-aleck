#include <fast-aleck/token.h>

const fa_token fa_null_token = { .slice = { .start = NULL, .length = 0 }, .type = fa_token_type_undefined };

void fa_token_print(fa_token token, FILE *file) {
	fputs("fa_token(", file);

	char *type;
	switch (token.type) {
		case fa_token_type_block:
			type = "block";
			break;

		case fa_token_type_inline:
			type = "inline";
			break;

		case fa_token_type_text:
			type = "text";
			break;

		case fa_token_type_text_raw:
			type = "text (raw)";
			break;

		case fa_token_type_text_no_html:
			type = "text (no html)";
			break;

		case fa_token_type_end:
			type = "end";
			break;

		default:
			type = "???";
			break;
	}
	fprintf(file, "type = %s, ", type);

	fputs("data = \"", file);
	char *s_end = token.slice.start + token.slice.length;
	for (char *s = token.slice.start; s < s_end; ++s) {
		fputc(*s, file);
	}

	fputs("\")", file);
}
