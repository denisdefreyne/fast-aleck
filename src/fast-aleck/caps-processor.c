#include <fast-aleck/caps-processor.h>

#include <fast-aleck/widont-processor.h>

const fa_token _caps_start_token = { .slice = { .start = "<span class=\"caps\">", .length = 19 }, .type = fa_token_type_inline };
const fa_token _caps_end_token   = { .slice = { .start = "</span>",               .length = 7  }, .type = fa_token_type_inline };

static inline void _fa_caps_processor_pass_on_token(fa_state *state, fa_token token) {
	fa_widont_processor_handle_token(state, token);
}

static inline void _fa_caps_processor_pass_on_caps_token(fa_state *state, fa_token token) {
	if (token.slice.length >= 2 && state->caps_processor_state.letter_found) {
		_fa_caps_processor_pass_on_token(state, _caps_start_token);
		_fa_caps_processor_pass_on_token(state, token);
		_fa_caps_processor_pass_on_token(state, _caps_end_token);
	} else {
		_fa_caps_processor_pass_on_token(state, token);
	}
}

void fa_caps_processor_state_init(fa_caps_processor_state *state) {
	state->normal_token.slice.start  = NULL;
	state->normal_token.slice.length = 0;
	state->caps_token.slice.start    = NULL;
	state->caps_token.slice.length   = 0;
	state->letter_found              = false;
}

void fa_caps_processor_handle_token(fa_state *state, fa_token token) {
	if (!state->config.wrap_caps) {
		_fa_caps_processor_pass_on_token(state, token);
		return;
	}

	switch (token.type) {
		case fa_token_type_inline:
		case fa_token_type_block:
		case fa_token_type_text_no_html:
		case fa_token_type_text_raw:
			_fa_caps_processor_pass_on_token(state, token);
			return;

		case fa_token_type_undefined:
		case fa_token_type_text:
			break;
	}

	fa_caps_processor_state *s = &state->caps_processor_state;

	s->normal_token.slice.start  = token.slice.start;
	s->normal_token.slice.length = 0;
	s->caps_token.slice.start    = NULL;
	s->caps_token.slice.length   = 0;

	for (; token.slice.length > 0; ++token.slice.start, --token.slice.length) {
		char c = *token.slice.start;

		bool is_caps_candidate = (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
		bool is_letter         = (c >= 'A' && c <= 'Z');
		bool is_writing_caps   = s->caps_token.slice.length > 0;

		if (is_caps_candidate) {
			if (!s->caps_token.slice.start) {
				s->caps_token.slice.start  = token.slice.start;
				s->caps_token.slice.length = 0;
			}
			if (is_letter) {
				s->letter_found = true;
			}
			++s->caps_token.slice.length;
		} else if (is_writing_caps) {
			// quit writing caps
			if (s->letter_found) {
				_fa_caps_processor_pass_on_token(state, s->normal_token);
				_fa_caps_processor_pass_on_caps_token(state, s->caps_token);
				s->normal_token.slice.start  = token.slice.start;
				s->normal_token.slice.length = 0;
			} else {
				s->normal_token.slice.length += s->caps_token.slice.length;
			}
			++s->normal_token.slice.length;
			s->caps_token.slice.start  = NULL;
			s->caps_token.slice.length = 0;
			s->letter_found = false;
		} else {
			++s->normal_token.slice.length;
		}
	}

	_fa_caps_processor_pass_on_token(state, s->normal_token);
	if (s->caps_token.slice.length > 0) {
		_fa_caps_processor_pass_on_caps_token(state, s->caps_token);
	}
}
