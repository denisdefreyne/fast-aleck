#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <fast-aleck/text-processor.h>
#include <fast-aleck/caps-processor.h>

//TODO when appending quotes, check whether type is html-restricted (e.g. for <title>)

const fa_token _ellipsis_token           = { .slice = { .start = "\xE2\x80\xA6",                             .length = 3  }, .type = fa_token_type_text   };
const fa_token _mdash_token              = { .slice = { .start = "\xE2\x80\x94",                             .length = 3  }, .type = fa_token_type_text   };
const fa_token _nbsp_token               = { .slice = { .start = "&nbsp;",                                   .length = 6  }, .type = fa_token_type_text   };
const fa_token _squo_start_token         = { .slice = { .start = "\xE2\x80\x98",                             .length = 3  }, .type = fa_token_type_text   };
const fa_token _squo_start_wrapped_token = { .slice = { .start = "<span class=\"quo\">\xE2\x80\x98</span>",  .length = 28 }, .type = fa_token_type_inline };
const fa_token _squo_end_token           = { .slice = { .start = "\xE2\x80\x99",                             .length = 3  }, .type = fa_token_type_text   };
const fa_token _squo_end_wrapped_token   = { .slice = { .start = "<span class=\"quo\">\xE2\x80\x99</span>",  .length = 28 }, .type = fa_token_type_inline };
const fa_token _dquo_start_token         = { .slice = { .start = "\xE2\x80\x9C",                             .length = 3  }, .type = fa_token_type_text   };
const fa_token _dquo_start_wrapped_token = { .slice = { .start = "<span class=\"dquo\">\xE2\x80\x9C</span>", .length = 29 }, .type = fa_token_type_inline };
const fa_token _dquo_end_token           = { .slice = { .start = "\xE2\x80\x9D",                             .length = 3  }, .type = fa_token_type_text   };
const fa_token _dquo_end_wrapped_token   = { .slice = { .start = "<span class=\"dquo\">\xE2\x80\x9D</span>", .length = 29 }, .type = fa_token_type_inline };
const fa_token _amp_wrapped_token        = { .slice = { .start = "<span class=\"amp\">&amp;</span>",         .length = 30 }, .type = fa_token_type_inline };

static inline bool _fa_should_open_quote(char in) {
	return in == '\0' || in == '(' || isspace(in);
}

void fa_text_processor_pass_on_token(fa_state *state, fa_token token) {
	fa_caps_processor_handle_token(state, token);
}

void fa_text_processor_state_init(fa_text_processor_state *state) {
	state->fsm_state          = fa_text_processor_fsm_state_start;
	state->is_at_start_of_run = true;
	state->last_char          = false;
}

void fa_text_processor_handle_token(fa_state *state, fa_token token) {
	switch (token.type) {
		case fa_token_type_text_raw:
		case fa_token_type_inline:
			fa_text_processor_pass_on_token(state, token);
			return;

		case fa_token_type_block:
			fa_text_processor_pass_on_token(state, token);
			fa_text_processor_state_init(&state->text_processor_state);
			return;

		default:
			break;
	}

	fa_token current_token;
	current_token.slice.start  = token.slice.start;
	current_token.slice.length = 0;
	current_token.type         = fa_token_type_text;

	for (; token.slice.length > 0; ++token.slice.start, --token.slice.length) {
		char c = *token.slice.start;

		switch (state->text_processor_state.fsm_state) {
			case fa_text_processor_fsm_state_start:
				switch (c) {
					case '.':
						state->text_processor_state.fsm_state = fa_text_processor_fsm_state_dot;
						break;

					case '-':
						state->text_processor_state.fsm_state = fa_text_processor_fsm_state_dash;
						break;

					case '\'':
						fa_text_processor_pass_on_token(state, current_token);
						current_token.slice.start  = token.slice.start+1;
						current_token.slice.length = 0;
						if (_fa_should_open_quote(state->text_processor_state.last_char)) {
							if (state->text_processor_state.is_at_start_of_run && state->text_processor_state.config.wrap_quotes) {
								fa_text_processor_pass_on_token(state, _squo_start_wrapped_token);
							} else {
								fa_text_processor_pass_on_token(state, _squo_start_token);
							}
						} else {
							if (state->text_processor_state.is_at_start_of_run && state->text_processor_state.config.wrap_quotes) {
								fa_text_processor_pass_on_token(state, _squo_end_wrapped_token);
							} else {
								fa_text_processor_pass_on_token(state, _squo_end_token);
							}
						}
						break;

					case '"':
						fa_text_processor_pass_on_token(state, current_token);
						current_token.slice.start  = token.slice.start+1;
						current_token.slice.length = 0;
						if (_fa_should_open_quote(state->text_processor_state.last_char)) {
							if (state->text_processor_state.is_at_start_of_run && state->text_processor_state.config.wrap_quotes) {
								fa_text_processor_pass_on_token(state, _dquo_start_wrapped_token);
							} else {
								fa_text_processor_pass_on_token(state, _dquo_start_token);
							}
						} else {
							if (state->text_processor_state.is_at_start_of_run && state->text_processor_state.config.wrap_quotes) {
								fa_text_processor_pass_on_token(state, _dquo_end_wrapped_token);
							} else {
								fa_text_processor_pass_on_token(state, _dquo_end_token);
							}
						}
						break;

					case '&':
						state->text_processor_state.fsm_state = fa_text_processor_fsm_state_amp;
						break;

					default:
						++current_token.slice.length;
				}
				state->text_processor_state.is_at_start_of_run = 0;
				break;

			case fa_text_processor_fsm_state_amp:
				if (c == 'a')
					state->text_processor_state.fsm_state = fa_text_processor_fsm_state_ampa;
				else {
					current_token.slice.length += 2;
					state->text_processor_state.fsm_state = fa_text_processor_fsm_state_start;
				}
				break;

			case fa_text_processor_fsm_state_ampa:
				if (c == 'm')
					state->text_processor_state.fsm_state = fa_text_processor_fsm_state_ampam;
				else {
					current_token.slice.length += 3;
					state->text_processor_state.fsm_state = fa_text_processor_fsm_state_start;
				}
				break;

			case fa_text_processor_fsm_state_ampam:
				if (c == 'p')
					state->text_processor_state.fsm_state = fa_text_processor_fsm_state_ampamp;
				else {
					current_token.slice.length += 4;
					state->text_processor_state.fsm_state = fa_text_processor_fsm_state_start;
				}
				break;

			case fa_text_processor_fsm_state_ampamp:
				if (c == ';') {
					if(state->text_processor_state.config.wrap_amps /* ... and not in title */) {
						fa_text_processor_pass_on_token(state, current_token);
						current_token.slice.start = token.slice.start+1;
						current_token.slice.length = 0;
						fa_text_processor_pass_on_token(state, _amp_wrapped_token);
					} else {
						current_token.slice.length += 6;
					}
				} else {
					current_token.slice.length += 5;
				}
				state->text_processor_state.fsm_state = fa_text_processor_fsm_state_start;
				break;

			case fa_text_processor_fsm_state_dot:
				if (c == '.')
					state->text_processor_state.fsm_state = fa_text_processor_fsm_state_dotdot;
				else {
					current_token.slice.length += 2;
					state->text_processor_state.fsm_state = fa_text_processor_fsm_state_start;
				}
				break;

			case fa_text_processor_fsm_state_dotdot:
				if (c == '.') {
					fa_text_processor_pass_on_token(state, current_token);
					current_token.slice.start  = token.slice.start+1;
					current_token.slice.length = 0;
					fa_text_processor_pass_on_token(state, _ellipsis_token);
					state->text_processor_state.fsm_state = fa_text_processor_fsm_state_start;
				} else {
					current_token.slice.length += 3;
					state->text_processor_state.fsm_state = fa_text_processor_fsm_state_start;
				}
				break;

			case fa_text_processor_fsm_state_dash:
				if (c == '-')
					state->text_processor_state.fsm_state = fa_text_processor_fsm_state_dashdash;
				else {
					current_token.slice.length += 2;
					state->text_processor_state.fsm_state = fa_text_processor_fsm_state_start;
				}
				break;

			case fa_text_processor_fsm_state_dashdash:
				if (c == '-') {
					fa_text_processor_pass_on_token(state, current_token);
					current_token.slice.start  = token.slice.start+1;
					current_token.slice.length = 0;
					fa_text_processor_pass_on_token(state, _mdash_token);
					state->text_processor_state.fsm_state = fa_text_processor_fsm_state_start;
				} else {
					fa_text_processor_pass_on_token(state, current_token);
					current_token.slice.start  = token.slice.start;
					current_token.slice.length = 1;
					fa_text_processor_pass_on_token(state, _mdash_token);
					state->text_processor_state.fsm_state = fa_text_processor_fsm_state_start;
				}
				break;
		}

		state->text_processor_state.last_char = c;
	}

	switch (state->text_processor_state.fsm_state) {
		case fa_text_processor_fsm_state_start:
			fa_text_processor_pass_on_token(state, current_token);
			break;

		case fa_text_processor_fsm_state_amp:
		case fa_text_processor_fsm_state_dot:
		case fa_text_processor_fsm_state_dash:
			current_token.slice.length += 1;
			fa_text_processor_pass_on_token(state, current_token);
			break;

		case fa_text_processor_fsm_state_dotdot:
		case fa_text_processor_fsm_state_ampa:
			current_token.slice.length += 2;
			fa_text_processor_pass_on_token(state, current_token);
			break;

		case fa_text_processor_fsm_state_ampam:
			current_token.slice.length += 3;
			fa_text_processor_pass_on_token(state, current_token);
			break;

		case fa_text_processor_fsm_state_ampamp:
			current_token.slice.length += 4;
			fa_text_processor_pass_on_token(state, current_token);
			break;

		case fa_text_processor_fsm_state_dashdash:
			fa_text_processor_pass_on_token(state, current_token);
			fa_text_processor_pass_on_token(state, _mdash_token);
			break;
	}
	state->text_processor_state.fsm_state = fa_text_processor_fsm_state_start;
}
