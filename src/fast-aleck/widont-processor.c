#include <fast-aleck/widont-processor.h>

#include <ctype.h>

// static const fa_token _nbsp_token = { .slice = { .start = "&nbsp;", .length = 6 }, .type = fa_token_type_inline };

static inline void fa_widont_processor_pass_on_token(fa_state *state, fa_token token) {
	fa_token_buffer_append(&state->token_buffer, token);
}

// the                             /     / the   /
// the _                           /     / the   _
// the _ quick                     the   _ quick /
// the _ quick _                   the   _ quick _
// the _ quick _ brown             quick _ brown /
// the _ quick _ brown _           quick _ brown _
// the _ quick _ brown _ fox       brown _ fox   /


static void fa_widont_processor_converter_handle_token(fa_state *state, fa_token token, bool blank) {
	// pass through initial blank tokens
	if (!state->widont_processor_state.have_words && blank) {
		fa_widont_processor_pass_on_token(state, token);
		return;
	}

	// pass on 1
	size_t len_words  = fa_token_buffer_get_length(&state->widont_processor_state.words1);
	for (size_t i = 0; i < len_words; ++i) {
		fa_widont_processor_pass_on_token(state, fa_token_buffer_at(&state->widont_processor_state.words1, i));
	}

	// pass on 2
	size_t len_spaces = fa_token_buffer_get_length(&state->widont_processor_state.spaces1);
	for (size_t i = 0; i < len_spaces; ++i) {
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

	// if (blank) {
		// fa_widont_processor_pass_on_token(state, _nbsp_token);
	// } else {
		state->widont_processor_state.have_words = true;
		fa_widont_processor_pass_on_token(state, token);
	// }
}

void fa_widont_processor_state_init(fa_widont_processor_state *state) {
	// 4 is a random guess
	fa_token_buffer_init(&state->words1,  4);
	fa_token_buffer_init(&state->spaces1, 4);
	fa_token_buffer_init(&state->words2,  4);
	fa_token_buffer_init(&state->spaces2, 4);

	state->have_words = false;
}

void fa_widont_processor_handle_token(fa_state *state, fa_token token) {
	if (fa_token_type_text != token.type) {
		// TODO flush if token type is block OR end
		fa_widont_processor_pass_on_token(state, token);
		return;
	}

	char *prev_s = token.slice.start;
	bool prev_blank = isspace(*prev_s); // FIXME this assumes token.slice.length > 0
	size_t length = token.slice.length;
	for (char *s = prev_s + 1; length > 0; --length, ++s) {
		bool cur_blank = isspace(*s);
		if (prev_blank != cur_blank || 1 == length) {
			fa_token t = { .slice = { .start = prev_s, .length = s - prev_s }, .type = fa_token_type_inline };
			fa_widont_processor_converter_handle_token(state, t, prev_blank);
			prev_s = s;
			prev_blank = cur_blank;
		}
	}
}
