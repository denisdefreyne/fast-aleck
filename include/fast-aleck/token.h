#ifndef __FAST_ALECK_TOKEN_H__
#define __FAST_ALECK_TOKEN_H__

// TODO remove me
#include <stdio.h>

#include <fast-aleck/slice.h>

// Token type
enum _fa_token_type {
	fa_token_type_undefined = -1,
	fa_token_type_block,
	fa_token_type_inline,
	fa_token_type_text,
	fa_token_type_text_raw,
	fa_token_type_text_no_html
};
typedef enum _fa_token_type fa_token_type;

// Token
struct _fa_token {
	fa_slice      slice;
	fa_token_type type;
};
typedef struct _fa_token fa_token;

const fa_token fa_null_token;

// TODO remove me
void fa_token_print(fa_token token, FILE *file);

#endif
