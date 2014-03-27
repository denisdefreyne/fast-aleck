#include <fast-aleck/widont-processor.h>

#include <ctype.h>

static const fa_token _nbsp_token = { .slice = { .start = "&nbsp;", .length = 6 }, .type = fa_token_type_inline };

static inline void fa_widont_processor_pass_on_token(fa_state *state, fa_token token) {
	fa_token_buffer_append(&state->token_buffer, token);
}

// the                             /     / the   /
// the _                           /     / the   _
// the X quick                     the   X quick /
// the X quick _                   the   X quick _
// the _ quick X brown             quick X brown /
// the _ quick X brown _           quick X brown _
// the _ quick _ brown X fox       brown X fox   /

static void fa_widont_processor_converter_handle_token(fa_state *state, fa_token token, bool blank) {
	size_t len;

	// pass through initial blank tokens
	if (!state->widont_processor_state.have_words && blank) {
		fa_widont_processor_pass_on_token(state, token);
		return;
	}

	// Pass on and shift when expectation is wrong
	if (state->widont_processor_state.expect_word == blank) {
		// pass on 1
		len = state->widont_processor_state.words1.size;
		for (size_t i = 0; i < len; ++i) {
			fa_widont_processor_pass_on_token(state, fa_token_buffer_at(&state->widont_processor_state.words1, i));
		}

		// pass on 2
		len = state->widont_processor_state.spaces1.size;
		for (size_t i = 0; i < len; ++i) {
			fa_widont_processor_pass_on_token(state, fa_token_buffer_at(&state->widont_processor_state.spaces1, i));
		}

		// shift 1 <- 3
		fa_token_buffer words_token_buffer = state->widont_processor_state.words1;
		state->widont_processor_state.words1 = state->widont_processor_state.words2;
		state->widont_processor_state.words2 = words_token_buffer;
		fa_token_buffer_clear(&state->widont_processor_state.words2);

		// shift 2 <- 4
		fa_token_buffer spaces_token_buffer = state->widont_processor_state.spaces1;
		state->widont_processor_state.spaces1 = state->widont_processor_state.spaces2;
		state->widont_processor_state.spaces2 = spaces_token_buffer;
		fa_token_buffer_clear(&state->widont_processor_state.spaces2);
	}

	if (blank) {
		fa_token_buffer_append(&state->widont_processor_state.spaces2, token);
	} else {
		fa_token_buffer_append(&state->widont_processor_state.words2, token);
	}

	state->widont_processor_state.expect_word = !blank;
	state->widont_processor_state.have_words = true;
}

static void _fa_widont_processor_flush(fa_state *state) {
	if (!state->config.widont) {
		return;
	}

	// if spaces1 is not empty, replace it with nbsp

	size_t len;

	// words1
	len = state->widont_processor_state.words1.size;
	for (size_t i = 0; i < len; ++i) {
		fa_widont_processor_pass_on_token(state, fa_token_buffer_at(&state->widont_processor_state.words1, i));
	}

	// spaces1 - magic happens here
	if (0 < state->widont_processor_state.spaces1.size) {
		fa_widont_processor_pass_on_token(state, _nbsp_token);
	}

	// words2
	len = state->widont_processor_state.words2.size;
	for (size_t i = 0; i < len; ++i) {
		fa_widont_processor_pass_on_token(state, fa_token_buffer_at(&state->widont_processor_state.words2, i));
	}

	// spaces2
	len = state->widont_processor_state.spaces2.size;
	for (size_t i = 0; i < len; ++i) {
		fa_widont_processor_pass_on_token(state, fa_token_buffer_at(&state->widont_processor_state.spaces2, i));
	}

	// reset
	fa_token_buffer_clear(&state->widont_processor_state.words1);
	fa_token_buffer_clear(&state->widont_processor_state.spaces1);
	fa_token_buffer_clear(&state->widont_processor_state.words2);
	fa_token_buffer_clear(&state->widont_processor_state.spaces2);
	state->widont_processor_state.have_words = false;
	state->widont_processor_state.expect_word = true;
}

void fa_widont_processor_state_init(fa_widont_processor_state *state) {
	// 4 is a random guess
	fa_token_buffer_init(&state->words1,  4);
	fa_token_buffer_init(&state->spaces1, 4);
	fa_token_buffer_init(&state->words2,  4);
	fa_token_buffer_init(&state->spaces2, 4);

	state->have_words = false;
	state->expect_word = true;
}

void fa_widont_processor_handle_token(fa_state *state, fa_token token) {
	if (!state->config.widont) {
		fa_widont_processor_pass_on_token(state, token);
		return;
	}

	switch (token.type) {
		case fa_token_type_inline:
		case fa_token_type_text_raw:
			fa_widont_processor_converter_handle_token(state, token, false);
			return;

		case fa_token_type_text_no_html:
		case fa_token_type_text:
			break;

		case fa_token_type_block:
		case fa_token_type_undefined:
		case fa_token_type_end:
			_fa_widont_processor_flush(state);
			fa_widont_processor_pass_on_token(state, token);
			return;
	}

	if (0 == token.slice.length) {
		return;
	}

	char *prev_s = token.slice.start;
	bool prev_blank = isspace(*prev_s);
	char *max_s = token.slice.start + token.slice.length;

	for (char *s = prev_s + 1; s <= max_s; ++s) {
		bool cur_blank = isspace(*s);
		if (prev_blank != cur_blank || s == max_s) {
			fa_token t = { .slice = { .start = prev_s, .length = s - prev_s }, .type = fa_token_type_text };
			fa_widont_processor_converter_handle_token(state, t, prev_blank);
			prev_s = s;
			prev_blank = cur_blank;
		}
	}
}
