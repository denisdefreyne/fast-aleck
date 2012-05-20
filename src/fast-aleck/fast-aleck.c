#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fast-aleck/fast-aleck.h>
#include <fast-aleck/buffer.h>

void fast_aleck_config_init(fast_aleck_config *config)
{
	bzero(config, sizeof (fast_aleck_config));
}

void fast_aleck_init(fast_aleck_state *state, fast_aleck_config config)
{
	bzero(state, sizeof (fast_aleck_state));
	state->config = config;
	state->is_at_start_of_run = 1;
	state->out_diff_first_space = -1;
	state->out_diff_first_real_space = -1;
	state->out_diff_last_space = -1;
	state->out_diff_last_real_space = -1;
	state->out_diff_first_caps = -1;
	state->out_diff_last_caps = -1;
	state->out_diff_last_char = -1;
	state->out_diff_last_char_before_space = -1;
	state->out_diff_last_char_after_space = -1;
}

//////////////////////////////////////////////////////////////////////////////

static inline void _fa_append_ellipsis(fast_aleck_buffer *buf);
static inline void _fa_append_mdash(fast_aleck_buffer *buf);
static inline void _fa_append_single_quote_start(fast_aleck_buffer *buf, bool a_should_wrap);
static inline void _fa_append_single_quote_end(fast_aleck_buffer *buf, bool a_should_wrap);
static inline void _fa_append_double_quote_start(fast_aleck_buffer *buf, bool a_should_wrap);
static inline void _fa_append_double_quote_end(fast_aleck_buffer *buf, bool a_should_wrap);
static inline void _fa_append_wrapped_amp(fast_aleck_buffer *buf);

static inline bool _fa_should_open_quote(char in);

static inline void _fa_handle_block_tag(fast_aleck_state *state, fast_aleck_buffer *buf);
static inline void _fa_wrap_caps(fast_aleck_state *state, fast_aleck_buffer *buf);

//////////////////////////////////////////////////////////////////////////////

char *fast_aleck(fast_aleck_config a_config, char *a_in, size_t a_in_size, size_t *ao_out_size)
{
	fast_aleck_state state;
	fast_aleck_init(&state, a_config);

	fast_aleck_buffer out_buf;
	fast_aleck_buffer_create(&out_buf, (size_t)((float)a_in_size * 1.2));

	fast_aleck_feed(&state, a_in, a_in_size, &out_buf);	
	fast_aleck_finish(&state, &out_buf);

	if (ao_out_size)
		*ao_out_size = out_buf.cur - out_buf.start - 1;
	return out_buf.start;
}

// TODO allow buf to be nil
void fast_aleck_finish(fast_aleck_state *state, fast_aleck_buffer *buf)
{
	switch (state->fsm_state)
	{
		case _fa_fsm_state_dot:
			fast_aleck_buffer_unchecked_append_char(buf, '.');
			break;

		case _fa_fsm_state_dotdot:
			fast_aleck_buffer_unchecked_append_string(buf, "..", 2);
			break;

		case _fa_fsm_state_dash:
			fast_aleck_buffer_unchecked_append_char(buf, '-');
			break;

		case _fa_fsm_state_dashdash:
			_fa_append_mdash(buf);
			break;

		case _fa_fsm_state_tag_start:
		case _fa_fsm_state_tag:
			fast_aleck_buffer_unchecked_append_char(buf, '>');
			break;

		case _fa_fsm_state_cdata:
			fast_aleck_buffer_unchecked_append_string(buf, "]]>", 3);
			break;

		case _fa_fsm_state_attr_squo:
			fast_aleck_buffer_unchecked_append_string(buf, "'>", 2);
			break;

		case _fa_fsm_state_attr_dquo:
			fast_aleck_buffer_unchecked_append_string(buf, "\">", 2);
			break;

		default:
			break;
	}

	fast_aleck_buffer_unchecked_append_char(buf, '\0');
}

/*
 * Architecture:
 *
 * Three translators:
 * 1. general
 * 2. caps
 * 3. widon't
 *
 * 2 and 3 need an internal buffer because they potentially modify the output stream after it is written.
 *
 * Each translator has 1 char input and has 1 or more chars output. A translation function takes three args:
 * 1. input character
 * 2. state (as reference)
 * 3. output buffer
 */

void fast_aleck_feed(fast_aleck_state *a_state, char *a_in, size_t a_in_size, fast_aleck_buffer *out_buf)
{
	char *in_start = a_in;
	char *in       = a_in;

	for (; *in; ++in)
	{
		fast_aleck_buffer_ensure_remaining(out_buf, 30);

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
					_fa_wrap_caps(a_state, out_buf);
					a_state->out_diff_last_char             = out_buf->cur - out_buf->start;
					a_state->letter_found                   = 1;
					a_state->chars_found_after_space        = 1;
					a_state->out_diff_last_char_after_space = out_buf->cur - out_buf->start;
					a_state->out_diff_first_real_space      = a_state->out_diff_first_space;
					a_state->out_diff_last_real_space       = a_state->out_diff_last_space;
					fast_aleck_buffer_unchecked_append_char(out_buf, *in);
					break;

				case 'A': case 'B': case 'C': case 'D': case 'E': 
				case 'F': case 'G': case 'H': case 'I': case 'J': 
				case 'K': case 'L': case 'M': case 'N': case 'O': 
				case 'P': case 'Q': case 'R': case 'S': case 'T': 
				case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': 
					if (a_state->out_diff_first_caps < 0)
						a_state->out_diff_first_caps = out_buf->cur - out_buf->start;
					a_state->out_diff_last_caps             = out_buf->cur - out_buf->start;
					a_state->letter_found                   = 1;
					a_state->chars_found_after_space        = 1;
					a_state->caps_found                     = 1;
					a_state->out_diff_last_char             = out_buf->cur - out_buf->start;
					a_state->out_diff_last_char_after_space = out_buf->cur - out_buf->start;
					a_state->out_diff_first_real_space      = a_state->out_diff_first_space;
					a_state->out_diff_last_real_space       = a_state->out_diff_last_space;
					fast_aleck_buffer_unchecked_append_char(out_buf, *in);
					break;

				case '1': case '2': case '3': case '4': case '5': 
				case '6': case '7': case '8': case '9': case '0': 
					if (a_state->out_diff_first_caps >= 0)
						a_state->out_diff_last_caps = out_buf->cur - out_buf->start;
					a_state->out_diff_last_char             = out_buf->cur - out_buf->start;
					a_state->letter_found                   = 1;
					a_state->chars_found_after_space        = 1;
					a_state->out_diff_last_char_after_space = out_buf->cur - out_buf->start;
					a_state->out_diff_first_real_space      = a_state->out_diff_first_space;
					a_state->out_diff_last_real_space       = a_state->out_diff_last_space;
					fast_aleck_buffer_unchecked_append_char(out_buf, *in);
					break;

				case '\t': case ' ': case '\r': case '\n':
					_fa_wrap_caps(a_state, out_buf);
					a_state->out_diff_last_char = out_buf->cur - out_buf->start;
					if (a_state->chars_found_after_space)
						a_state->out_diff_first_space = -1;
					if (a_state->out_diff_first_space < 0)
						a_state->out_diff_first_space = out_buf->cur - out_buf->start;
					a_state->out_diff_last_space = out_buf->cur - out_buf->start;
					a_state->chars_found_after_space = 0;
					if (a_state->out_diff_last_char_after_space >= 0)
						a_state->out_diff_last_char_before_space = a_state->out_diff_last_char_after_space;
					a_state->out_diff_last_char_after_space = -1;
					fast_aleck_buffer_unchecked_append_char(out_buf, *in);
					break;

				case '.':
					_fa_wrap_caps(a_state, out_buf);
					a_state->chars_found_after_space = 1;
					a_state->fsm_state = _fa_fsm_state_dot;
					break;

				case '-':
					_fa_wrap_caps(a_state, out_buf);
					a_state->chars_found_after_space = 1;
					a_state->fsm_state = _fa_fsm_state_dash;
					break;

				case '\'':
					_fa_wrap_caps(a_state, out_buf);
					a_state->chars_found_after_space = 1;
					if (a_state->out_diff_last_char < 0 || _fa_should_open_quote(*(out_buf->start + a_state->out_diff_last_char)))
						_fa_append_single_quote_start(out_buf, a_state->is_at_start_of_run && !a_state->in_title && a_state->config.wrap_quotes);
					else
						_fa_append_single_quote_end(out_buf, a_state->is_at_start_of_run && !a_state->in_title && a_state->config.wrap_quotes);
					break;
				
				case '"':
					_fa_wrap_caps(a_state, out_buf);
					a_state->chars_found_after_space = 1;
					if (a_state->out_diff_last_char < 0 || _fa_should_open_quote(*(out_buf->start + a_state->out_diff_last_char)))
						_fa_append_double_quote_start(out_buf, a_state->is_at_start_of_run && !a_state->in_title && a_state->config.wrap_quotes);
					else
						_fa_append_double_quote_end(out_buf, a_state->is_at_start_of_run && !a_state->in_title && a_state->config.wrap_quotes);
					break;
						
				case '<':
					_fa_wrap_caps(a_state, out_buf);
					a_state->chars_found_after_space = 1;
					fast_aleck_buffer_unchecked_append_char(out_buf, *in);
					a_state->fsm_state = _fa_fsm_state_tag_start;
					break;

				case '&':
					_fa_wrap_caps(a_state, out_buf);
					a_state->chars_found_after_space = 1;
					if (a_state->config.wrap_amps && !a_state->in_title && (in + 4 < in_start + a_in_size - 1) && 0 == strncmp(in+1, "amp;", 4))
					{
						in += 4;
						_fa_append_wrapped_amp(out_buf);
						break;
					}
					else
						fast_aleck_buffer_unchecked_append_char(out_buf, *in);
					break;

				default:
					_fa_wrap_caps(a_state, out_buf);
					a_state->chars_found_after_space = 1;
					a_state->out_diff_last_char = out_buf->cur - out_buf->start;
					fast_aleck_buffer_unchecked_append_char(out_buf, *in);
					break;
			}
		}
		else
		{
			fast_aleck_buffer_unchecked_append_char(out_buf, *in);
			if (*in == '<')
				a_state->fsm_state = _fa_fsm_state_tag_start;
		}
		a_state->is_at_start_of_run = 0;
		continue;

	FA_STATE_DOT:
		if (*in == '.')
			a_state->fsm_state = _fa_fsm_state_dotdot;
		else
		{
			--in;
			fast_aleck_buffer_unchecked_append_char(out_buf, '.');
			a_state->fsm_state = _fa_fsm_state_start;
		}
		continue;

	FA_STATE_DOTDOT:
		if (*in == '.')
		{
			_fa_append_ellipsis(out_buf);
			a_state->fsm_state = _fa_fsm_state_start;
		}
		else
		{
			--in;
			fast_aleck_buffer_unchecked_append_string(out_buf, "..", 2);
			a_state->fsm_state = _fa_fsm_state_start;
		}
		continue;

	FA_STATE_DASH:
		if (*in == '-')
			a_state->fsm_state = _fa_fsm_state_dashdash;
		else
		{
			--in;
			fast_aleck_buffer_unchecked_append_char(out_buf, '-');
			a_state->fsm_state = _fa_fsm_state_start;
		}
		continue;
		
	FA_STATE_DASHDASH:
		if (*in == '-')
		{
			_fa_append_mdash(out_buf);
			a_state->fsm_state = _fa_fsm_state_start;
		}
		else
		{
			--in;
			_fa_append_mdash(out_buf);
			a_state->fsm_state = _fa_fsm_state_start;
		}
		continue;

	FA_STATE_TAG_START:
		if (*in == '/')
		{
			a_state->end_tag_slash_detected = 1;
			fast_aleck_buffer_unchecked_append_char(out_buf, *in);
		}
		else if (!a_state->end_tag_slash_detected && 0 == strncmp(in, "![CDATA[", 8))
		{
			in += 8;
			fast_aleck_buffer_unchecked_append_string(out_buf, "![CDATA[", 8);
			a_state->fsm_state = _fa_fsm_state_cdata;
		}
		// special start/end tags
		else if (0 == strncmp(in, "title", 5) && (isspace(*(in+5)) || *(in+5) == '>'))
		{
			a_state->in_title = !a_state->end_tag_slash_detected;
			a_state->fsm_state = _fa_fsm_state_tag;
			in += 4;
			fast_aleck_buffer_unchecked_append_string(out_buf, "title", 5);
		}
		// start/end tags for resetting elements
		else if (0 == strncmp(in, "blockquote", 10) && (isspace(*(in+10)) || *(in+10) == '>'))
		{
			_fa_handle_block_tag(a_state, out_buf);
			in += 9;
			fast_aleck_buffer_unchecked_append_string(out_buf, "blockquote", 10);
		}
		else if (0 == strncmp(in, "br", 2) && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_fa_handle_block_tag(a_state, out_buf);
			in += 1;
			fast_aleck_buffer_unchecked_append_string(out_buf, "br", 2);
		}
		else if (0 == strncmp(in, "dd", 2) && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_fa_handle_block_tag(a_state, out_buf);
			in += 1;
			fast_aleck_buffer_unchecked_append_string(out_buf, "dd", 2);
		}
		else if (0 == strncmp(in, "div", 3) && (isspace(*(in+3)) || *(in+3) == '>'))
		{
			_fa_handle_block_tag(a_state, out_buf);
			in += 2;
			fast_aleck_buffer_unchecked_append_string(out_buf, "div", 3);
		}
		else if (0 == strncmp(in, "dt", 2) && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_fa_handle_block_tag(a_state, out_buf);
			in += 1;
			fast_aleck_buffer_unchecked_append_string(out_buf, "dt", 2);
		}
		else if ('h' == *in && *(in+1) >= '1' && *(in+1) <= '6' && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_fa_handle_block_tag(a_state, out_buf);
			fast_aleck_buffer_unchecked_append_string(out_buf, in, 2);
			in += 1;
		}
		else if (0 == strncmp(in, "li", 2) && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_fa_handle_block_tag(a_state, out_buf);
			in += 1;
			fast_aleck_buffer_unchecked_append_string(out_buf, "li", 2);
		}
		else if ('p' == *in && (isspace(*(in+1)) || *(in+1) == '>'))
		{
			_fa_handle_block_tag(a_state, out_buf);
			fast_aleck_buffer_unchecked_append_char(out_buf, *in);
		}
		// start/end tags for excluded elements
#define _FA_MATCHES_EXCLUDED_ELEMENT(s) (0 == strncmp(in, s, strlen(s)) && (isspace(*(in+strlen(s))) || *(in+strlen(s)) == '>'))
#define _FA_HANDLE_EXCLUDED_ELEMENT(s, flag) { \
	in += strlen(s)-1; \
	fast_aleck_buffer_unchecked_append_string(out_buf, s, strlen(s)); \
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
			fast_aleck_buffer_unchecked_append_char(out_buf, *in);
			a_state->fsm_state = _fa_fsm_state_tag;
		}
		continue;

	FA_STATE_TAG:
		fast_aleck_buffer_unchecked_append_char(out_buf, *in);
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
		fast_aleck_buffer_unchecked_append_char(out_buf, *in);
		if (*in == ']' && (in + 2 < in_start + a_in_size - 1) && 0 == strncmp(in+1, "]>", 2))
		{
			in += 2;
			fast_aleck_buffer_unchecked_append_string(out_buf, "]>", 2);
			a_state->fsm_state = _fa_fsm_state_start;
		}
		continue;

	FA_STATE_ATTR_SQUO:
		fast_aleck_buffer_unchecked_append_char(out_buf, *in);
		if (*in == '\'')
			a_state->fsm_state = _fa_fsm_state_tag;
		continue;

	FA_STATE_ATTR_DQUO:
		fast_aleck_buffer_unchecked_append_char(out_buf, *in);
		if (*in == '"')
			a_state->fsm_state = _fa_fsm_state_tag;
		continue;
	}
}

//////////////////////////////////////////////////////////////////////////////

static inline bool _fa_should_open_quote(char in)
{
	return in == '(' || isspace(in);
}

static inline void _fa_handle_block_tag(fast_aleck_state *state, fast_aleck_buffer *buf)
{
	if (state->config.widont && state->out_diff_last_char_before_space >= 0 && state->letter_found && state->out_diff_last_real_space >= 0)
	{
		memmove(
			buf->start + state->out_diff_first_real_space + 6,
			buf->start + state->out_diff_last_real_space + 1,
			buf->cur - (buf->start + state->out_diff_last_real_space));
		memcpy(
			buf->start + state->out_diff_first_real_space,
			"&nbsp;",
			6);
		buf->cur += 5 - (state->out_diff_last_real_space - state->out_diff_first_real_space);
	}

	state->fsm_state = _fa_fsm_state_tag;
	state->is_at_start_of_run = 1;
	state->letter_found = 0;
	state->out_diff_first_space            = -1;
	state->out_diff_last_space             = -1;
	state->out_diff_first_real_space       = -1;
	state->out_diff_last_real_space        = -1;
	state->out_diff_last_char_before_space = -1;
	state->out_diff_last_char_after_space  = -1;
	state->out_diff_last_char              = -1;
}

static inline void _fa_wrap_caps(fast_aleck_state *state, fast_aleck_buffer *buf)
{
	if (!state->in_title && state->config.wrap_caps && state->caps_found && state->out_diff_last_caps >= 0 && state->out_diff_last_caps - state->out_diff_first_caps > 0)
	{
		char *s1 = "<span class=\"caps\">";
		char *s2 = "</span>";

		memmove(
			buf->start + state->out_diff_first_caps + strlen(s1),
			buf->start + state->out_diff_first_caps,
			state->out_diff_last_caps + 1 - state->out_diff_first_caps);
		memcpy(
			buf->start + state->out_diff_first_caps,
			s1,
			strlen(s1));
		buf->cur += strlen(s1);
		memcpy(buf->cur, s2, strlen(s2));
		buf->cur += strlen(s2);
	}

	state->out_diff_first_caps = -1;
	state->out_diff_last_caps  = -1;
}

static inline void _fa_append_ellipsis(fast_aleck_buffer *buf)
{
	fast_aleck_buffer_unchecked_append_string(buf, "\xE2\x80\xA6", 3);
}

static inline void _fa_append_mdash(fast_aleck_buffer *buf)
{
	fast_aleck_buffer_unchecked_append_string(buf, "\xE2\x80\x94", 3);
}

static inline void _fa_append_single_quote_start(fast_aleck_buffer *buf, bool a_should_wrap)
{
	if (a_should_wrap)
	{
		fast_aleck_buffer_unchecked_append_string(buf, "<span class=\"quo\">", 18);
		fast_aleck_buffer_unchecked_append_string(buf, "\xE2\x80\x98", 3);
		fast_aleck_buffer_unchecked_append_string(buf, "</span>", 7);
	}
	else
	{
		fast_aleck_buffer_unchecked_append_string(buf, "\xE2\x80\x98", 3);
	}
}

static inline void _fa_append_single_quote_end(fast_aleck_buffer *buf, bool a_should_wrap)
{
	if (a_should_wrap)
	{
		fast_aleck_buffer_unchecked_append_string(buf, "<span class=\"quo\">", 18);
		fast_aleck_buffer_unchecked_append_string(buf, "\xE2\x80\x99", 3);
		fast_aleck_buffer_unchecked_append_string(buf, "</span>", 7);
	}
	else
	{
		fast_aleck_buffer_unchecked_append_string(buf, "\xE2\x80\x99", 3);
	}
}

static inline void _fa_append_double_quote_start(fast_aleck_buffer *buf, bool a_should_wrap)
{
	if (a_should_wrap)
	{
		fast_aleck_buffer_unchecked_append_string(buf, "<span class=\"dquo\">", 19);
		fast_aleck_buffer_unchecked_append_string(buf, "\xE2\x80\x9C", 3);
		fast_aleck_buffer_unchecked_append_string(buf, "</span>", 7);
	}
	else
	{
		fast_aleck_buffer_unchecked_append_string(buf, "\xE2\x80\x9C", 3);
	}
}

static inline void _fa_append_double_quote_end(fast_aleck_buffer *buf, bool a_should_wrap)
{
	if (a_should_wrap)
	{
		fast_aleck_buffer_unchecked_append_string(buf, "<span class=\"dquo\">", 19);
		fast_aleck_buffer_unchecked_append_string(buf, "\xE2\x80\x9D", 3);
		fast_aleck_buffer_unchecked_append_string(buf, "</span>", 7);
	}
	else
	{
		fast_aleck_buffer_unchecked_append_string(buf, "\xE2\x80\x9D", 3);
	}
}

static inline void _fa_append_wrapped_amp(fast_aleck_buffer *buf)
{
	fast_aleck_buffer_unchecked_append_string(buf, "<span class=\"amp\">&amp;</span>", 30);
}
