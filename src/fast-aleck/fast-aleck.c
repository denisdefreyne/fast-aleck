#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <fast-aleck/fast-aleck.h>

void fast_aleck_config_init(fast_aleck_config *config)
{
	bzero(config, sizeof (fast_aleck_config));
}

void fast_aleck_init(fast_aleck_state *state, fast_aleck_config config)
{
	bzero(state, sizeof (fast_aleck_state));
	state->config = config;
	state->is_at_start_of_run = 1;
}

//////////////////////////////////////////////////////////////////////////////

static inline size_t _fa_write_ellipsis(char *out);
static inline size_t _fa_write_mdash(char *out);
static inline size_t _fa_write_single_quote_start(char *out, bool a_should_wrap);
static inline size_t _fa_write_single_quote_end(char *out, bool a_should_wrap);
static inline size_t _fa_write_double_quote_start(char *out, bool a_should_wrap);
static inline size_t _fa_write_double_quote_end(char *out, bool a_should_wrap);
static inline size_t _fa_write_wrapped_amp(char *out);

static inline bool _fa_should_open_quote(char in);

static inline void _fa_handle_block_tag(fast_aleck_state *state, char **out);
static inline void _fa_wrap_caps(fast_aleck_state *state, char **out);

//////////////////////////////////////////////////////////////////////////////

char *fast_aleck(fast_aleck_config a_config, char *a_in, size_t a_in_size, size_t *ao_out_size)
{
	fast_aleck_state state;
	fast_aleck_init(&state, a_config);

	size_t out_main_size;
	char *out_main = fast_aleck_feed(&state, a_in, a_in_size, &out_main_size);	

	size_t out_finish_size;
	char *out_finish = fast_aleck_finish(&state, &out_finish_size);

	size_t out_size = out_main_size + out_finish_size;
	char *out = realloc(out_main, out_main_size + out_finish_size + 1);
	memcpy(out + out_main_size, out_finish, out_finish_size + 1);
	if (ao_out_size)
		*ao_out_size = out_main_size + out_finish_size;

	return out;
}

char *fast_aleck_finish(fast_aleck_state *state, size_t *ao_out_size)
{
	char *out_start = malloc(4);
	char *out = out_start;

	switch (state->fsm_state)
	{
		case _fa_fsm_state_dot:
			*out++ = '.';
			break;

		case _fa_fsm_state_dotdot:
			*out++ = '.';
			*out++ = '.';
			break;

		case _fa_fsm_state_dash:
			*out++ = '-';
			break;

		case _fa_fsm_state_dashdash:
			out += _fa_write_mdash(out);
			break;

		case _fa_fsm_state_tag_start:
		case _fa_fsm_state_tag:
			*out++ = '>';
			break;

		case _fa_fsm_state_cdata:
			*out++ = ']';
			*out++ = ']';
			*out++ = '>';
			break;

		case _fa_fsm_state_attr_squo:
			*out++ = '\'';
			*out++ = '>';
			break;

		case _fa_fsm_state_attr_dquo:
			*out++ = '"';
			*out++ = '>';
			break;

		default:
			break;
	}

	*out = '\0';

	if (ao_out_size)
		*ao_out_size = out - out_start;

	return out_start;
}

char *fast_aleck_feed(fast_aleck_state *a_state, char *a_in, size_t a_in_size, size_t *ao_out_size)
{
	size_t out_size = (size_t)((float)a_in_size * 1.2);
	char *out_start = malloc(out_size);
	char *out       = out_start;

	char *in_start = a_in;
	char *in       = a_in;

	for (; *in; ++in)
	{
		if (out + 30 >= out_start + out_size)
		{
			char *out_start_old = out_start;
			
			out_size = (out_size + 30) * 2;
			out_start = realloc(out_start, out_size);
			if (!out_start)
			{
				free(out_start_old);
				return NULL;
			}

			ptrdiff_t diff = out_start - out_start_old;
			out += diff;
			if (a_state->out_first_space)      a_state->out_first_space      += diff;
			if (a_state->out_last_space)       a_state->out_last_space       += diff;
			if (a_state->out_first_real_space) a_state->out_first_real_space += diff;
			if (a_state->out_last_real_space)  a_state->out_last_real_space  += diff;
			if (a_state->out_first_caps)       a_state->out_first_caps       += diff;
			if (a_state->out_last_caps)        a_state->out_last_caps        += diff;
			if (a_state->out_last_char)        a_state->out_last_char        += diff;
		}

		switch (a_state->fsm_state)
		{
			case _fa_fsm_state_start:     goto FA_STATE_START;     break;
			case _fa_fsm_state_dot:       goto FA_STATE_DOT;       break;
			case _fa_fsm_state_dotdot:    goto FA_STATE_DOTDOT;    break;
			case _fa_fsm_state_dash:      goto FA_STATE_DASH;      break;
			case _fa_fsm_state_dashdash:  goto FA_STATE_DASHDASH;  break;
			case _fa_fsm_state_tag:       goto FA_STATE_TAG;       break;
			case _fa_fsm_state_tag_start: goto FA_STATE_TAG_START; break;
			case _fa_fsm_state_cdata:     goto FA_STATE_CDATA;     break;
			case _fa_fsm_state_attr_squo: goto FA_STATE_ATTR_SQUO; break;
			case _fa_fsm_state_attr_dquo: goto FA_STATE_ATTR_DQUO; break;
		}

	FA_STATE_START:
		if (!a_state->off)
		{
			switch(*in)
			{
				case 'a': case 'b': case 'c': case 'd': case 'e':
				case 'f': case 'g': case 'h': case 'i': case 'j':
				case 'k': case 'l': case 'm': case 'n': case 'o':
				case 'p': case 'q': case 'r': case 's': case 't':
				case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': 
					_fa_wrap_caps(a_state, &out);
					a_state->out_last_char = out;
					a_state->letter_found = 1;
					a_state->chars_found_after_space = 1;
					a_state->out_last_char_after_space = out;
					a_state->out_first_real_space = a_state->out_first_space;
					a_state->out_last_real_space = a_state->out_last_space;
					*out++ = *in;
					break;

				case 'A': case 'B': case 'C': case 'D': case 'E': 
				case 'F': case 'G': case 'H': case 'I': case 'J': 
				case 'K': case 'L': case 'M': case 'N': case 'O': 
				case 'P': case 'Q': case 'R': case 'S': case 'T': 
				case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': 
					if (!a_state->out_first_caps)
						a_state->out_first_caps = out;
					a_state->out_last_caps = out;
					a_state->letter_found = 1;
					a_state->chars_found_after_space = 1;
					a_state->caps_found = 1;
					a_state->out_last_char = out;
					a_state->out_last_char_after_space = out;
					a_state->out_first_real_space = a_state->out_first_space;
					a_state->out_last_real_space = a_state->out_last_space;
					*out++ = *in;
					break;

				case '1': case '2': case '3': case '4': case '5': 
				case '6': case '7': case '8': case '9': case '0': 
					if (a_state->out_first_caps)
						a_state->out_last_caps = out;
					a_state->out_last_char = out;
					a_state->letter_found = 1;
					a_state->chars_found_after_space = 1;
					a_state->out_last_char_after_space = out;
					a_state->out_first_real_space = a_state->out_first_space;
					a_state->out_last_real_space = a_state->out_last_space;
					*out++ = *in;
					break;

				case '\t': case ' ': case '\r': case '\n':
					_fa_wrap_caps(a_state, &out);
					a_state->out_last_char = out;
					if (a_state->chars_found_after_space)
						a_state->out_first_space = NULL;
					if (!a_state->out_first_space)
						a_state->out_first_space = out;
					a_state->out_last_space = out;
					a_state->chars_found_after_space = 0;
					if (a_state->out_last_char_after_space)
						a_state->out_last_char_before_space = a_state->out_last_char_after_space;
					a_state->out_last_char_after_space = NULL;
					*out++ = *in;
					break;

				case '.':
					_fa_wrap_caps(a_state, &out);
					a_state->chars_found_after_space = 1;
					a_state->fsm_state = _fa_fsm_state_dot;
					break;

				case '-':
					_fa_wrap_caps(a_state, &out);
					a_state->chars_found_after_space = 1;
					a_state->fsm_state = _fa_fsm_state_dash;
					break;

				case '\'':
					_fa_wrap_caps(a_state, &out);
					a_state->chars_found_after_space = 1;
					if (!a_state->out_last_char || _fa_should_open_quote(*a_state->out_last_char))
						out += _fa_write_single_quote_start(out, a_state->is_at_start_of_run && !a_state->in_title && a_state->config.wrap_quotes);
					else
						out += _fa_write_single_quote_end(out, a_state->is_at_start_of_run && !a_state->in_title && a_state->config.wrap_quotes);
					break;
				
				case '"':
					_fa_wrap_caps(a_state, &out);
					a_state->chars_found_after_space = 1;
					if (!a_state->out_last_char || _fa_should_open_quote(*a_state->out_last_char))
						out += _fa_write_double_quote_start(out, a_state->is_at_start_of_run && !a_state->in_title && a_state->config.wrap_quotes);
					else
						out += _fa_write_double_quote_end(out, a_state->is_at_start_of_run && !a_state->in_title && a_state->config.wrap_quotes);
					break;
						
				case '<':
					_fa_wrap_caps(a_state, &out);
					a_state->chars_found_after_space = 1;
					*out++ = *in;
					a_state->fsm_state = _fa_fsm_state_tag_start;
					break;

				case '&':
					_fa_wrap_caps(a_state, &out);
					a_state->chars_found_after_space = 1;
					if (a_state->config.wrap_amps && !a_state->in_title && (in + 4 < in_start + a_in_size - 1) && 0 == strncmp(in+1, "amp;", 4))
					{
						in += 4;
						out += _fa_write_wrapped_amp(out);
						break;
					}
					else
						*out++ = *in;
					break;

				default:
					_fa_wrap_caps(a_state, &out);
					a_state->chars_found_after_space = 1;
					a_state->out_last_char = out;
					*out++ = *in;
					break;
			}
		}
		else
		{
			if (*in == '<')
			{
				*out++ = *in;
				a_state->fsm_state = _fa_fsm_state_tag_start;
			}
			else
				*out++ = *in;
		}
		a_state->is_at_start_of_run = 0;
		continue;

	FA_STATE_DOT:
		if (*in == '.')
			a_state->fsm_state = _fa_fsm_state_dotdot;
		else
		{
			--in;
			*out++ = '.';
			a_state->fsm_state = _fa_fsm_state_start;
		}
		continue;

	FA_STATE_DOTDOT:
		if (*in == '.')
		{
			out += _fa_write_ellipsis(out);
			a_state->fsm_state = _fa_fsm_state_start;
		}
		else
		{
			--in;
			*out++ = '.';
			*out++ = '.';
			a_state->fsm_state = _fa_fsm_state_start;
		}
		continue;

	FA_STATE_DASH:
		if (*in == '-')
			a_state->fsm_state = _fa_fsm_state_dashdash;
		else
		{
			--in;
			*out++ = '-';
			a_state->fsm_state = _fa_fsm_state_start;
		}
		continue;
		
	FA_STATE_DASHDASH:
		if (*in == '-')
		{
			out += _fa_write_mdash(out);
			a_state->fsm_state = _fa_fsm_state_start;
		}
		else
		{
			--in;
			out += _fa_write_mdash(out);
			a_state->fsm_state = _fa_fsm_state_start;
		}
		continue;

	FA_STATE_TAG_START:
		if (*in == '/')
		{
			a_state->end_tag_slash_detected = 1;
			*out++ = *in;
		}
		else if (!a_state->end_tag_slash_detected && 0 == strncmp(in, "![CDATA[", 8))
		{
			in += 8;
			memcpy(out, "![CDATA[", 8);
			out += 8;
			a_state->fsm_state = _fa_fsm_state_cdata;
		}
		// special start/end tags
		else if (0 == strncmp(in, "title", 5) && (isspace(*(in+5)) || *(in+5) == '>'))
		{
			a_state->in_title = !a_state->end_tag_slash_detected;
			a_state->fsm_state = _fa_fsm_state_tag;
			in += 4;
			memcpy(out, "title", 5);
			out += 5;
		}
		// start/end tags for resetting elements
		else if (0 == strncmp(in, "blockquote", 10) && (isspace(*(in+10)) || *(in+10) == '>'))
		{
			_fa_handle_block_tag(a_state, &out);
			in += 9;
			memcpy(out, "blockquote", 10);
			out += 10;
		}
		else if (0 == strncmp(in, "br", 2) && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_fa_handle_block_tag(a_state, &out);
			in += 1;
			memcpy(out, "br", 2);
			out += 2;
		}
		else if (0 == strncmp(in, "dd", 2) && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_fa_handle_block_tag(a_state, &out);
			in += 1;
			memcpy(out, "dd", 2);
			out += 2;
		}
		else if (0 == strncmp(in, "div", 3) && (isspace(*(in+3)) || *(in+3) == '>'))
		{
			_fa_handle_block_tag(a_state, &out);
			in += 2;
			memcpy(out, "div", 3);
			out += 3;
		}
		else if (0 == strncmp(in, "dt", 2) && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_fa_handle_block_tag(a_state, &out);
			in += 1;
			memcpy(out, "dt", 2);
			out += 2;
		}
		else if ('h' == *in && *(in+1) >= '1' && *(in+1) <= '6' && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_fa_handle_block_tag(a_state, &out);
			*out++ = *in++;
			*out++ = *in;
		}
		else if (0 == strncmp(in, "li", 2) && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_fa_handle_block_tag(a_state, &out);
			in += 1;
			memcpy(out, "li", 2);
			out += 2;
		}
		else if ('p' == *in && (isspace(*(in+1)) || *(in+1) == '>'))
		{
			_fa_handle_block_tag(a_state, &out);
			*out++ = *in;
		}
		// start/end tags for excluded elements
#define _FA_MATCHES_EXCLUDED_ELEMENT(s) (0 == strncmp(in, s, strlen(s)) && (isspace(*(in+strlen(s))) || *(in+strlen(s)) == '>'))
#define _FA_HANDLE_EXCLUDED_ELEMENT(s, flag) { \
	in += strlen(s)-1; \
	memcpy(out, s, strlen(s)); \
	out += strlen(s); \
	a_state->fsm_state = _fa_fsm_state_tag; \
	if (a_state->end_tag_slash_detected) \
	{ \
		flag = 0; \
		a_state->off  = a_state->in_code || a_state->in_kbd || a_state->in_pre || a_state->in_script || a_state->in_samp || a_state->in_var || a_state->in_math || a_state->in_textarea; \
	} \
	else \
	{ \
		flag = 1; \
		a_state->off  = 1; \
	} \
}
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("code"))
			_FA_HANDLE_EXCLUDED_ELEMENT("code", a_state->in_code)
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("kbd"))
			_FA_HANDLE_EXCLUDED_ELEMENT("kbd", a_state->in_kbd)
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("pre"))
			_FA_HANDLE_EXCLUDED_ELEMENT("pre", a_state->in_pre)
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("script"))
			_FA_HANDLE_EXCLUDED_ELEMENT("script", a_state->in_script)
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("samp"))
			_FA_HANDLE_EXCLUDED_ELEMENT("samp", a_state->in_samp)
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("var"))
			_FA_HANDLE_EXCLUDED_ELEMENT("var", a_state->in_var)
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("math"))
			_FA_HANDLE_EXCLUDED_ELEMENT("math", a_state->in_math)
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("textarea"))
			_FA_HANDLE_EXCLUDED_ELEMENT("textarea", a_state->in_textarea)
		else
		{
			*out++ = *in;
			a_state->fsm_state = _fa_fsm_state_tag;
		}
		continue;

	FA_STATE_TAG:
		*out++ = *in;
		if (*in == '>')
		{
			a_state->end_tag_slash_detected = 0;
			a_state->fsm_state = _fa_fsm_state_start;
		}
		else if (*in == '\'')
			a_state->fsm_state = _fa_fsm_state_attr_squo;
		else if (*in == '"')
			a_state->fsm_state = _fa_fsm_state_attr_dquo;
		continue;

	FA_STATE_CDATA:
		*out++ = *in;
		if (*in == ']' && (in + 2 < in_start + a_in_size - 1) && 0 == strncmp(in+1, "]>", 2))
		{
			in += 2;
			memcpy(out, "]>", 2);
			out += 2;
			a_state->fsm_state = _fa_fsm_state_start;
		}
		continue;

	FA_STATE_ATTR_SQUO:
		*out++ = *in;
		if (*in == '\'')
			a_state->fsm_state = _fa_fsm_state_tag;
		continue;

	FA_STATE_ATTR_DQUO:
		*out++ = *in;
		if (*in == '"')
			a_state->fsm_state = _fa_fsm_state_tag;
		continue;
	}

	if (ao_out_size)
		*ao_out_size = out - out_start;
	return out_start;
}

//////////////////////////////////////////////////////////////////////////////

static inline bool _fa_should_open_quote(char in)
{
	return in == '(' || isspace(in);
}

static inline void _fa_handle_block_tag(fast_aleck_state *state, char **out)
{
	if (state->config.widont && state->out_last_char_before_space && state->letter_found && state->out_last_real_space)
	{
		memmove(state->out_first_real_space + 6, state->out_last_real_space + 1, *out - state->out_last_real_space);
		memcpy(state->out_first_real_space, "&nbsp;", 6);
		*out += 5 - (state->out_last_real_space - state->out_first_real_space);
	}

	state->fsm_state = _fa_fsm_state_tag;
	state->is_at_start_of_run = 1;
	state->letter_found = 0;
	state->out_first_space = NULL;
	state->out_last_space  = NULL;
	state->out_first_real_space = NULL;
	state->out_last_real_space = NULL;
	state->out_last_char_before_space = NULL;
	state->out_last_char_after_space = NULL;
	state->out_last_char = NULL;
}

static inline void _fa_wrap_caps(fast_aleck_state *state, char **out)
{
	if (!state->in_title && state->config.wrap_caps && state->caps_found && state->out_last_caps && state->out_last_caps - state->out_first_caps > 0)
	{
		char *s1 = "<span class=\"caps\">";
		char *s2 = "</span>";

		memmove(state->out_first_caps + strlen(s1), state->out_first_caps, state->out_last_caps + 1 - state->out_first_caps);
		memcpy(state->out_first_caps, s1, strlen(s1));
		*out += strlen(s1);
		memcpy(*out, s2, strlen(s2));
		*out += strlen(s2);
	}

	state->out_first_caps = NULL;
	state->out_last_caps = NULL;
}

static inline size_t _fa_write_ellipsis(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0xA6;

	return 3;
}

static inline size_t _fa_write_mdash(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0x94;

	return 3;
}

static inline size_t _fa_write_single_quote_start(char *out, bool a_should_wrap)
{
	char *s1 = "<span class=\"quo\">";
	char *s2 = "</span>";

	if (a_should_wrap)
	{
		memcpy(out, s1, 18);
		out += 18;
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x98;
		memcpy(out, s2, 7);
		return 28;
	}
	else
	{
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x98;
		return 3;
	}
}

static inline size_t _fa_write_single_quote_end(char *out, bool a_should_wrap)
{
	char *s1 = "<span class=\"quo\">";
	char *s2 = "</span>";

	if (a_should_wrap)
	{
		memcpy(out, s1, 18);
		out += 18;
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x99;
		memcpy(out, s2, 7);
		return 28;
	}
	else
	{
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x99;
		return 3;
	}
}

static inline size_t _fa_write_double_quote_start(char *out, bool a_should_wrap)
{
	char *s1 = "<span class=\"dquo\">";
	char *s2 = "</span>";

	if (a_should_wrap)
	{
		memcpy(out, s1, 19);
		out += 19;
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x9C;
		memcpy(out, s2, 7);
		return 29;
	}
	else
	{
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x9C;
		return 3;
	}
}

static inline size_t _fa_write_double_quote_end(char *out, bool a_should_wrap)
{
	char *s1 = "<span class=\"dquo\">";
	char *s2 = "</span>";

	if (a_should_wrap)
	{
		memcpy(out, s1, 19);
		out += 19;
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x9D;
		memcpy(out, s2, 7);
		return 29;
	}
	else
	{
		*out++ = 0xE2;
		*out++ = 0x80;
		*out++ = 0x9D;
		return 3;
	}
}

static inline size_t _fa_write_wrapped_amp(char *out)
{
	char *s = "<span class=\"amp\">&amp;</span>";
	memcpy(out, s, 30);
	return 30;
}
