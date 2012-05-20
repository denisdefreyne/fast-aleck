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

	fast_aleck_buffer_create(&state->tag_name, 10);

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

static void _fa_handle_tag_name(fast_aleck_state *a_state, fast_aleck_buffer *out_buf);
static void _fa_feed_handle_tag_char(fast_aleck_state *a_state, char a_in, fast_aleck_buffer *out_buf);
static void _fa_feed_handle_body_text_char(fast_aleck_state *a_state, char a_in, fast_aleck_buffer *out_buf);

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

void fast_aleck_feed(fast_aleck_state *a_state, char *a_in, size_t a_in_size, fast_aleck_buffer *out_buf)
{
	for (; *a_in; a_in++)
		_fa_feed_handle_tag_char(a_state, *a_in, out_buf);
}

void fast_aleck_finish(fast_aleck_state *state, fast_aleck_buffer *buf)
{
	switch (state->fsm_state)
	{
		case _fa_fsm_text_state_dot:
			fast_aleck_buffer_unchecked_append_char(buf, '.');
			break;

		case _fa_fsm_text_state_dotdot:
			fast_aleck_buffer_unchecked_append_string(buf, "..", 2);
			break;

		case _fa_fsm_text_state_dash:
			fast_aleck_buffer_unchecked_append_char(buf, '-');
			break;

		case _fa_fsm_text_state_dashdash:
			_fa_append_mdash(buf);
			break;

		default:
			break;
	}

	fast_aleck_buffer_unchecked_append_char(buf, '\0');
}

//////////////////////////////////////////////////////////////////////////////

static void fast_aleck_reset_text_state(fast_aleck_state *a_state, fast_aleck_buffer *out_buf)
{
	// TODO implement
	a_state->is_at_start_of_run = true;
	a_state->last_char = '\0';
}

#define _FAST_ALECK_SET_FLAGS_FOR_EXCLUDED_ELEMENT(flag) \
	if (a_state->is_closing_tag) \
	{ \
		a_state->is_in_##flag = false; \
		a_state->is_in_excluded_element = \
			a_state->is_in_code || a_state->is_in_kbd || a_state->is_in_pre  || a_state->is_in_script || \
			a_state->is_in_samp || a_state->is_in_var || a_state->is_in_math || a_state->is_in_textarea; \
	} \
	else \
	{ \
		a_state->is_in_##flag = true; \
		a_state->is_in_excluded_element = true; \
	}

static void _fa_handle_tag_name(fast_aleck_state *a_state, fast_aleck_buffer *out_buf)
{
	char *tag_name = a_state->tag_name.start;
	size_t tag_name_length = a_state->tag_name.cur - a_state->tag_name.start;

	bool is_block = false;

	if (tag_name_length > 10)
		return;
	switch (tag_name_length)
	{
		case 1: // p
			if ('p' == tag_name[0])
				is_block = true;
			break;

		case 2: // br dd dt h1 h2 h3 h4 h5 h6 li
			switch (tag_name[0])
			{
				case 'b':
					if ('r' == tag_name[1])
						is_block = true;
					break;

				case 'd':
					if ('d' == tag_name[1] || 't' == tag_name[1])
						is_block = true;
					break;

				case 'h':
					if ('1' <= tag_name[1] && '6' >= tag_name[1])
						is_block = true;
					break;

				case 'l':
					if ('i' == tag_name[1])
						is_block = true;
					break;
			}
			break;

		case 3: // div kbd pre var
			switch (tag_name[0])
			{
				case 'd':
					if (0 == strncmp("iv", tag_name+1, 2))
						is_block = true;
					break;

				case 'k':
					if (0 == strncmp("bd", tag_name+1, 2))
						_FAST_ALECK_SET_FLAGS_FOR_EXCLUDED_ELEMENT(kbd);
					break;

				case 'p':
					if (0 == strncmp("re", tag_name+1, 2))
					{
						_FAST_ALECK_SET_FLAGS_FOR_EXCLUDED_ELEMENT(pre);
						is_block = true;
					}
					break;

				case 'v':
					if (0 == strncmp("ar", tag_name+1, 2))
						_FAST_ALECK_SET_FLAGS_FOR_EXCLUDED_ELEMENT(var);
					break;
			}
			break;

		case 4: // code math samp
			switch (tag_name[0])
			{
				case 'c':
					if (0 == strncmp("ode", tag_name+1, 3))
						_FAST_ALECK_SET_FLAGS_FOR_EXCLUDED_ELEMENT(code);
					break;

				case 'm':
					if (0 == strncmp("ath", tag_name+1, 3))
						_FAST_ALECK_SET_FLAGS_FOR_EXCLUDED_ELEMENT(math);
					break;

				case 's':
					if (0 == strncmp("amp", tag_name+1, 3))
						_FAST_ALECK_SET_FLAGS_FOR_EXCLUDED_ELEMENT(samp);
					break;
			}
			break;

		case 5: // title
			if (0 == strncmp("title", tag_name, 5))
				a_state->is_in_title = !a_state->is_closing_tag;
			break;

		case 6: // script
			if (0 == strncmp("script", tag_name, 6))
				_FAST_ALECK_SET_FLAGS_FOR_EXCLUDED_ELEMENT(script);
			break;

		case 8: // textarea
			if (0 == strncmp("textarea", tag_name, 8))
				_FAST_ALECK_SET_FLAGS_FOR_EXCLUDED_ELEMENT(textarea);
			break;

		case 10: // blockquote
			if (0 == strncmp("blockquote", tag_name, 10))
				is_block = true;
			break;

		default:
			break;
	}

	if (is_block)
		fast_aleck_reset_text_state(a_state, out_buf);
}

static void _fa_feed_handle_tag_char(fast_aleck_state *a_state, char a_in, fast_aleck_buffer *out_buf)
{
	bool is_tag_name_complete;

redo:
	switch (a_state->fsm_tag_state)
	{
		case _fa_fsm_tag_state_entry:
			if ('<' == a_in)
			{
				a_state->fsm_tag_state = _fa_fsm_tag_state_tag_start;
				fast_aleck_buffer_append_char(out_buf, a_in);
			}
			else if (a_state->is_in_excluded_element)
			{
				fast_aleck_buffer_append_char(out_buf, a_in);
			}
			else
			{
				_fa_feed_handle_body_text_char(a_state, a_in, out_buf);
			}
			break;

		case _fa_fsm_tag_state_tag_start:
			a_state->is_closing_tag = false;
			fast_aleck_buffer_clear(&a_state->tag_name);
			switch (a_in)
			{
				case '>':
					fast_aleck_buffer_append_char(out_buf, a_in);
					a_state->fsm_tag_state = _fa_fsm_tag_state_entry;
					break;

				case '/':
					fast_aleck_buffer_append_char(out_buf, a_in);
					a_state->fsm_tag_state = _fa_fsm_tag_state_tag_name;
					a_state->is_closing_tag = true;
					break;
			
				default:
					a_state->fsm_tag_state = _fa_fsm_tag_state_tag_name;
					goto redo;
					break;
			}
			break;

		case _fa_fsm_tag_state_tag_name:
			fast_aleck_buffer_append_char(out_buf, a_in);
			is_tag_name_complete = false;
			switch (a_in)
			{
				case ' ':
				case '\t':
					a_state->fsm_tag_state = _fa_fsm_tag_state_attr;
					is_tag_name_complete = true;
					break;

				case '>':
					a_state->fsm_tag_state = _fa_fsm_tag_state_entry;
					is_tag_name_complete = true;
					break;

				default:
					fast_aleck_buffer_append_char(&a_state->tag_name, a_in);
					break;
			}
			if (is_tag_name_complete)
				_fa_handle_tag_name(a_state, out_buf);
			break;

		case _fa_fsm_tag_state_attr:
			fast_aleck_buffer_append_char(out_buf, a_in);
			switch (a_in)
			{
				case '"':
					a_state->fsm_tag_state = _fa_fsm_tag_state_attr_dquo;
					break;

				case '\'':
					a_state->fsm_tag_state = _fa_fsm_tag_state_attr_squo;
					break;

				case '>':
					a_state->fsm_tag_state = _fa_fsm_tag_state_entry;
					break;
			}
			break;

		case _fa_fsm_tag_state_attr_squo:
			fast_aleck_buffer_append_char(out_buf, a_in);
			if ('\'' == a_in)
				a_state->fsm_tag_state = _fa_fsm_tag_state_attr;
			break;

		case _fa_fsm_tag_state_attr_dquo:
			fast_aleck_buffer_append_char(out_buf, a_in);
			if ('"' == a_in)
				a_state->fsm_tag_state = _fa_fsm_tag_state_attr;
			break;
	}
}

void _fa_feed_handle_body_text_char(fast_aleck_state *a_state, char a_in, fast_aleck_buffer *out_buf)
{
	fast_aleck_buffer_ensure_remaining(out_buf, 30);

	switch (a_state->fsm_state)
	{
		case _fa_fsm_text_state_start:
			if (!a_state->off)
			{
				switch(a_in)
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
						fast_aleck_buffer_unchecked_append_char(out_buf, a_in);
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
						fast_aleck_buffer_unchecked_append_char(out_buf, a_in);
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
						fast_aleck_buffer_unchecked_append_char(out_buf, a_in);
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
						fast_aleck_buffer_unchecked_append_char(out_buf, a_in);
						break;
		
					case '.':
						_fa_wrap_caps(a_state, out_buf);
						a_state->chars_found_after_space = 1;
						a_state->fsm_state = _fa_fsm_text_state_dot;
						break;
		
					case '-':
						_fa_wrap_caps(a_state, out_buf);
						a_state->chars_found_after_space = 1;
						a_state->fsm_state = _fa_fsm_text_state_dash;
						break;
		
					case '\'':
						_fa_wrap_caps(a_state, out_buf);
						a_state->chars_found_after_space = 1;
						if (_fa_should_open_quote(a_state->last_char))
							_fa_append_single_quote_start(out_buf, a_state->is_at_start_of_run && !a_state->is_in_title && a_state->config.wrap_quotes);
						else
							_fa_append_single_quote_end(out_buf, a_state->is_at_start_of_run && !a_state->is_in_title && a_state->config.wrap_quotes);
						break;
					
					case '"':
						_fa_wrap_caps(a_state, out_buf);
						a_state->chars_found_after_space = 1;
						if (_fa_should_open_quote(a_state->last_char))
							_fa_append_double_quote_start(out_buf, a_state->is_at_start_of_run && !a_state->is_in_title && a_state->config.wrap_quotes);
						else
							_fa_append_double_quote_end(out_buf, a_state->is_at_start_of_run && !a_state->is_in_title && a_state->config.wrap_quotes);
						break;
							
					case '&':
						_fa_wrap_caps(a_state, out_buf);
						a_state->chars_found_after_space = 1;
						/*
						if (a_state->config.wrap_amps && !a_state->is_in_title && (in + 4 < in_start + a_in_size - 1) && 0 == strncmp(in+1, "amp;", 4))
						{
							in += 4;
							_fa_append_wrapped_amp(out_buf);
							break;
						}
						*/
						// FIXME this is wrong (&amp; vs &)
						if (a_state->config.wrap_amps && !a_state->is_in_title)
						{
							_fa_append_wrapped_amp(out_buf);
						}
						else
							fast_aleck_buffer_unchecked_append_char(out_buf, a_in);
						break;
		
					default:
						_fa_wrap_caps(a_state, out_buf);
						a_state->chars_found_after_space = 1;
						a_state->out_diff_last_char = out_buf->cur - out_buf->start;
						fast_aleck_buffer_unchecked_append_char(out_buf, a_in);
						break;
				}
			}
			else
			{
				fast_aleck_buffer_unchecked_append_char(out_buf, a_in);
			}
			a_state->is_at_start_of_run = 0;
			break;

		case _fa_fsm_text_state_dot:
			if (a_in == '.')
				a_state->fsm_state = _fa_fsm_text_state_dotdot;
			else
			{
				fast_aleck_buffer_unchecked_append_char(out_buf, '.');
				a_state->fsm_state = _fa_fsm_text_state_start;
				_fa_feed_handle_body_text_char(a_state, a_in, out_buf);
			}
			break;
		
		case _fa_fsm_text_state_dotdot:
			if (a_in == '.')
			{
				_fa_append_ellipsis(out_buf);
				a_state->fsm_state = _fa_fsm_text_state_start;
			}
			else
			{
				fast_aleck_buffer_unchecked_append_string(out_buf, "..", 2);
				a_state->fsm_state = _fa_fsm_text_state_start;
				_fa_feed_handle_body_text_char(a_state, a_in, out_buf);
			}
			break;
		
		case _fa_fsm_text_state_dash:
			if (a_in == '-')
				a_state->fsm_state = _fa_fsm_text_state_dashdash;
			else
			{
				fast_aleck_buffer_unchecked_append_char(out_buf, '-');
				a_state->fsm_state = _fa_fsm_text_state_start;
				_fa_feed_handle_body_text_char(a_state, a_in, out_buf);
			}
			break;
			
		case _fa_fsm_text_state_dashdash:
			if (a_in == '-')
			{
				_fa_append_mdash(out_buf);
				a_state->fsm_state = _fa_fsm_text_state_start;
			}
			else
			{
				_fa_append_mdash(out_buf);
				a_state->fsm_state = _fa_fsm_text_state_start;
				_fa_feed_handle_body_text_char(a_state, a_in, out_buf);
			}
			break;
	}

	a_state->last_char = *(out_buf->cur-1);
}

//////////////////////////////////////////////////////////////////////////////

static inline bool _fa_should_open_quote(char in)
{
	return in == '\0' || in == '(' || isspace(in);
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

	// FIXME reenable
	//state->fsm_state = _fa_fsm_text_state_tag;
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
	if (!state->is_in_title && state->config.wrap_caps && state->caps_found && state->out_diff_last_caps >= 0 && state->out_diff_last_caps - state->out_diff_first_caps > 0)
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
		fast_aleck_buffer_unchecked_append_string(buf, "<span class=\"quo\">\xE2\x80\x98</span>", 28);
	else
		fast_aleck_buffer_unchecked_append_string(buf, "\xE2\x80\x98", 3);
}

static inline void _fa_append_single_quote_end(fast_aleck_buffer *buf, bool a_should_wrap)
{
	if (a_should_wrap)
		fast_aleck_buffer_unchecked_append_string(buf, "<span class=\"quo\">\xE2\x80\x99", 28);
	else
		fast_aleck_buffer_unchecked_append_string(buf, "\xE2\x80\x99", 3);
}

static inline void _fa_append_double_quote_start(fast_aleck_buffer *buf, bool a_should_wrap)
{
	if (a_should_wrap)
		fast_aleck_buffer_unchecked_append_string(buf, "<span class=\"dquo\">\xE2\x80\x9C</span>", 29);
	else
		fast_aleck_buffer_unchecked_append_string(buf, "\xE2\x80\x9C", 3);
}

static inline void _fa_append_double_quote_end(fast_aleck_buffer *buf, bool a_should_wrap)
{
	if (a_should_wrap)
		fast_aleck_buffer_unchecked_append_string(buf, "<span class=\"dquo\">\xE2\x80\x9D</span>", 29);
	else
		fast_aleck_buffer_unchecked_append_string(buf, "\xE2\x80\x9D", 3);
}

static inline void _fa_append_wrapped_amp(fast_aleck_buffer *buf)
{
	fast_aleck_buffer_unchecked_append_string(buf, "<span class=\"amp\">&amp;</span>", 30);
}
