#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <fast-aleck/fast-aleck.h>

enum _fa_state
{
	_fa_state_start,
	_fa_state_dot,
	_fa_state_dotdot,
	_fa_state_dash,
	_fa_state_dashdash,
	_fa_state_tag_start,
	_fa_state_tag,
	_fa_state_cdata,
	_fa_state_attr_squo,
	_fa_state_attr_dquo
};

void fast_aleck_config_init(fast_aleck_config *config)
{
	bzero(config, sizeof(fast_aleck_config));
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

static inline size_t _fa_write_single_quote_start(char *out, fa_bool a_should_wrap)
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

static inline size_t _fa_write_single_quote_end(char *out, fa_bool a_should_wrap)
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

static inline size_t _fa_write_double_quote_start(char *out, fa_bool a_should_wrap)
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

static inline size_t _fa_write_double_quote_end(char *out, fa_bool a_should_wrap)
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

static inline fa_bool _fa_should_open_quote(char in)
{
	return in == '(' || isspace(in);
}

static inline size_t _fa_finish(char *a_out, enum _fa_state a_state)
{
	char *out = a_out;
	switch (a_state)
	{
		case _fa_state_dot:
			*out++ = '.';
			break;

		case _fa_state_dotdot:
			*out++ = '.';
			*out++ = '.';
			break;

		case _fa_state_dash:
			*out++ = '-';
			break;

		case _fa_state_dashdash:
			out += _fa_write_mdash(out);
			break;

		case _fa_state_tag_start:
		case _fa_state_tag:
			*out++ = '>';
			break;

		case _fa_state_cdata:
			*out++ = ']';
			*out++ = ']';
			*out++ = '>';
			break;

		case _fa_state_attr_squo:
			*out++ = '\'';
			*out++ = '>';
			break;

		case _fa_state_attr_dquo:
			*out++ = '"';
			*out++ = '>';
			break;

		default:
			break;
	}

	*out++ = '\0';

	return out - a_out - 1;
}

#define _FA_HANDLE_BLOCK_TAG \
	printf("out start   = %s\n", out_start); \
	printf("last before = %s\n", out_last_char_before_space); \
	printf("last after  = %s\n", out_last_char_after_space); \
	if (a_config.widont && out_last_char_before_space && letter_found && out_last_space) \
	{ \
		memmove(out_first_space + 6, out_last_space + 1, out - out_last_space); \
		memcpy(out_first_space, "&nbsp;", 6); \
		out += 5 - (out_last_space - out_first_space); \
	} \
	state = _fa_state_tag; \
	is_at_start_of_run = 1; \
	letter_found = 0; \
	out_first_space = NULL; \
	out_last_space  = NULL; \
	out_last_char_after_space = NULL; \
	out_last_char = NULL;

#define _FA_WRAP_CAPS \
	if (!in_title && a_config.wrap_caps && caps_found && out_last_caps && out_last_caps - out_first_caps > 0) \
		_fa_wrap_caps(&out_first_caps, &out_last_caps, &out); \
	out_first_caps = NULL; \
	out_last_caps = NULL;

static inline void _fa_wrap_caps(char **out_first_caps, char **out_last_caps, char **out)
{
	char *s1 = "<span class=\"caps\">";
	char *s2 = "</span>";

	memmove(*out_first_caps + strlen(s1), *out_first_caps, *out_last_caps + 1 - *out_first_caps);
	memcpy(*out_first_caps, s1, strlen(s1));
	*out += strlen(s1);
	memcpy(*out, s2, strlen(s2));
	*out += strlen(s2);

	*out_first_caps = NULL;
	*out_last_caps  = NULL;
}

char *fast_aleck(fast_aleck_config a_config, char *a_in, size_t a_in_size, size_t *ao_len)
{
	enum _fa_state state = _fa_state_start;

	size_t out_size = (size_t)((float)a_in_size * 1.2);
	char *out_start = malloc(out_size);
	char *out       = out_start;

	char *in_start = a_in;
	char *in       = a_in;

	fa_bool off = 0;

	fa_bool in_code     = 0;
	fa_bool in_kbd      = 0;
	fa_bool in_pre      = 0;
	fa_bool in_script   = 0;
	fa_bool in_samp     = 0;
	fa_bool in_var      = 0;
	fa_bool in_math     = 0;
	fa_bool in_textarea = 0;
	fa_bool in_title    = 0;

	fa_bool end_tag_slash_detected  = 0;
	fa_bool is_at_start_of_run      = 1;
	fa_bool caps_found              = 0;
	fa_bool letter_found            = 0;
	fa_bool chars_found_after_space = 0;

	char *out_first_space = NULL;
	char *out_last_space  = NULL;
	char *out_first_caps  = NULL;
	char *out_last_caps   = NULL;
	char *out_last_char   = NULL;
	char *out_last_char_before_space = NULL;
	char *out_last_char_after_space  = NULL;

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
			if (out_first_space) out_first_space += diff;
			if (out_last_space)  out_last_space  += diff;
			if (out_first_caps)  out_first_caps  += diff;
			if (out_last_caps)   out_last_caps   += diff;
			if (out_last_char)   out_last_char   += diff;
		}

		switch (state)
		{
			case _fa_state_start:     goto UP_STATE_START;     break;
			case _fa_state_dot:       goto UP_STATE_DOT;       break;
			case _fa_state_dotdot:    goto UP_STATE_DOTDOT;    break;
			case _fa_state_dash:      goto UP_STATE_DASH;      break;
			case _fa_state_dashdash:  goto UP_STATE_DASHDASH;  break;
			case _fa_state_tag:       goto UP_STATE_TAG;       break;
			case _fa_state_tag_start: goto UP_STATE_TAG_START; break;
			case _fa_state_cdata:     goto UP_STATE_CDATA;     break;
			case _fa_state_attr_squo: goto UP_STATE_ATTR_SQUO; break;
			case _fa_state_attr_dquo: goto UP_STATE_ATTR_DQUO; break;
		}

	UP_STATE_START:
		if (!off)
		{
			switch(*in)
			{
				case 'a': case 'b': case 'c': case 'd': case 'e':
				case 'f': case 'g': case 'h': case 'i': case 'j':
				case 'k': case 'l': case 'm': case 'n': case 'o':
				case 'p': case 'q': case 'r': case 's': case 't':
				case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': 
					_FA_WRAP_CAPS;
					out_last_char = out;
					letter_found = 1;
					chars_found_after_space = 1;
					out_last_char_after_space = out;
					*out++ = *in;
					break;

				case 'A': case 'B': case 'C': case 'D': case 'E': 
				case 'F': case 'G': case 'H': case 'I': case 'J': 
				case 'K': case 'L': case 'M': case 'N': case 'O': 
				case 'P': case 'Q': case 'R': case 'S': case 'T': 
				case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': 
					if (!out_first_caps)
						out_first_caps = out;
					out_last_caps = out;
					letter_found = 1;
					chars_found_after_space = 1;
					caps_found = 1;
					out_last_char = out;
					out_last_char_after_space = out;
					*out++ = *in;
					break;

				case '1': case '2': case '3': case '4': case '5': 
				case '6': case '7': case '8': case '9': case '0': 
					if (out_first_caps)
						out_last_caps = out;
					out_last_char = out;
					letter_found = 1;
					chars_found_after_space = 1;
					out_last_char_after_space = out;
					*out++ = *in;
					break;

				case '\t': case ' ': case '\r': case '\n':
					_FA_WRAP_CAPS;
					out_last_char = out;
					if (chars_found_after_space)
						out_first_space = NULL;
					if (!out_first_space)
						out_first_space = out;
					out_last_space = out;
					chars_found_after_space = 0;
					if (out_last_char_after_space)
						out_last_char_before_space = out_last_char_after_space;
					out_last_char_after_space = NULL;
					*out++ = *in;
					break;

				case '.':
					_FA_WRAP_CAPS;
					chars_found_after_space = 1;
					state = _fa_state_dot;
					break;

				case '-':
					_FA_WRAP_CAPS;
					chars_found_after_space = 1;
					state = _fa_state_dash;
					break;

				case '\'':
					_FA_WRAP_CAPS;
					chars_found_after_space = 1;
					if (!out_last_char || _fa_should_open_quote(*out_last_char))
						out += _fa_write_single_quote_start(out, is_at_start_of_run && !in_title && a_config.wrap_quotes);
					else
						out += _fa_write_single_quote_end(out, is_at_start_of_run && !in_title && a_config.wrap_quotes);
					break;
				
				case '"':
					_FA_WRAP_CAPS;
					chars_found_after_space = 1;
					if (!out_last_char || _fa_should_open_quote(*out_last_char))
						out += _fa_write_double_quote_start(out, is_at_start_of_run && !in_title && a_config.wrap_quotes);
					else
						out += _fa_write_double_quote_end(out, is_at_start_of_run && !in_title && a_config.wrap_quotes);
					break;
						
				case '<':
					_FA_WRAP_CAPS;
					chars_found_after_space = 1;
					*out++ = *in;
					state = _fa_state_tag_start;
					break;

				case '&':
					_FA_WRAP_CAPS;
					chars_found_after_space = 1;
					if (a_config.wrap_amps && !in_title && (in + 4 < in_start + a_in_size - 1) && 0 == strncmp(in+1, "amp;", 4))
					{
						in += 4;
						out += _fa_write_wrapped_amp(out);
						break;
					}
					else
						*out++ = *in;
					break;

				default:
					_FA_WRAP_CAPS;
					chars_found_after_space = 1;
					out_last_char = out;
					*out++ = *in;
					break;
			}
		}
		else
		{
			if (*in == '<')
			{
				*out++ = *in;
				state = _fa_state_tag_start;
			}
			else
				*out++ = *in;
		}
		is_at_start_of_run = 0;
		continue;

	UP_STATE_DOT:
		if (*in == '.')
			state = _fa_state_dotdot;
		else
		{
			--in;
			*out++ = '.';
			state = _fa_state_start;
		}
		continue;

	UP_STATE_DOTDOT:
		if (*in == '.')
		{
			out += _fa_write_ellipsis(out);
			state = _fa_state_start;
		}
		else
		{
			--in;
			*out++ = '.';
			*out++ = '.';
			state = _fa_state_start;
		}
		continue;

	UP_STATE_DASH:
		if (*in == '-')
			state = _fa_state_dashdash;
		else
		{
			--in;
			*out++ = '-';
			state = _fa_state_start;
		}
		continue;
		
	UP_STATE_DASHDASH:
		if (*in == '-')
		{
			out += _fa_write_mdash(out);
			state = _fa_state_start;
		}
		else
		{
			--in;
			out += _fa_write_mdash(out);
			state = _fa_state_start;
		}
		continue;

	UP_STATE_TAG_START:
		if (*in == '/')
		{
			end_tag_slash_detected = 1;
			*out++ = *in;
		}
		else if (!end_tag_slash_detected && 0 == strncmp(in, "![CDATA[", 8))
		{
			in += 8;
			memcpy(out, "![CDATA[", 8);
			out += 8;
			state = _fa_state_cdata;
		}
		// special start/end tags
		else if (0 == strncmp(in, "title", 5) && (isspace(*(in+5)) || *(in+5) == '>'))
		{
			in_title = !end_tag_slash_detected;
			state = _fa_state_tag;
			in += 4;
			memcpy(out, "title", 5);
			out += 5;
		}
		// start/end tags for resetting elements
		else if (0 == strncmp(in, "blockquote", 10) && (isspace(*(in+10)) || *(in+10) == '>'))
		{
			_FA_HANDLE_BLOCK_TAG
			in += 9;
			memcpy(out, "blockquote", 10);
			out += 10;
		}
		else if (0 == strncmp(in, "br", 2) && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_FA_HANDLE_BLOCK_TAG
			in += 1;
			memcpy(out, "br", 2);
			out += 2;
		}
		else if (0 == strncmp(in, "dd", 2) && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_FA_HANDLE_BLOCK_TAG
			in += 1;
			memcpy(out, "dd", 2);
			out += 2;
		}
		else if (0 == strncmp(in, "div", 3) && (isspace(*(in+3)) || *(in+3) == '>'))
		{
			_FA_HANDLE_BLOCK_TAG
			in += 2;
			memcpy(out, "div", 3);
			out += 3;
		}
		else if (0 == strncmp(in, "dt", 2) && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_FA_HANDLE_BLOCK_TAG
			in += 1;
			memcpy(out, "dt", 2);
			out += 2;
		}
		else if ('h' == *in && *(in+1) >= '1' && *(in+1) <= '6' && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_FA_HANDLE_BLOCK_TAG
			*out++ = *in++;
			*out++ = *in;
		}
		else if (0 == strncmp(in, "li", 2) && (isspace(*(in+2)) || *(in+2) == '>'))
		{
			_FA_HANDLE_BLOCK_TAG
			in += 1;
			memcpy(out, "li", 2);
			out += 2;
		}
		else if ('p' == *in && (isspace(*(in+1)) || *(in+1) == '>'))
		{
			_FA_HANDLE_BLOCK_TAG
			*out++ = *in;
		}
		// start/end tags for excluded elements
#define _FA_MATCHES_EXCLUDED_ELEMENT(s) (0 == strncmp(in, s, strlen(s)) && (isspace(*(in+strlen(s))) || *(in+strlen(s)) == '>'))
#define _FA_HANDLE_EXCLUDED_ELEMENT(s, flag) { \
	in += strlen(s)-1; \
	memcpy(out, s, strlen(s)); \
	out += strlen(s); \
	state = _fa_state_tag; \
	if (end_tag_slash_detected) \
	{ \
		flag = 0; \
		off  = in_code || in_kbd || in_pre || in_script || in_samp || in_var || in_math || in_textarea; \
	} \
	else \
	{ \
		flag = 1; \
		off  = 1; \
	} \
}
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("code"))
			_FA_HANDLE_EXCLUDED_ELEMENT("code", in_code)
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("kbd"))
			_FA_HANDLE_EXCLUDED_ELEMENT("kbd", in_kbd)
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("pre"))
			_FA_HANDLE_EXCLUDED_ELEMENT("pre", in_pre)
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("script"))
			_FA_HANDLE_EXCLUDED_ELEMENT("script", in_script)
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("samp"))
			_FA_HANDLE_EXCLUDED_ELEMENT("samp", in_samp)
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("var"))
			_FA_HANDLE_EXCLUDED_ELEMENT("var", in_var)
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("math"))
			_FA_HANDLE_EXCLUDED_ELEMENT("math", in_math)
		else if (_FA_MATCHES_EXCLUDED_ELEMENT("textarea"))
			_FA_HANDLE_EXCLUDED_ELEMENT("textarea", in_textarea)
		else
		{
			*out++ = *in;
			state = _fa_state_tag;
		}
		continue;

	UP_STATE_TAG:
		*out++ = *in;
		if (*in == '>')
		{
			end_tag_slash_detected = 0;
			state = _fa_state_start;
		}
		else if (*in == '\'')
			state = _fa_state_attr_squo;
		else if (*in == '"')
			state = _fa_state_attr_dquo;
		continue;

	UP_STATE_CDATA:
		*out++ = *in;
		if (*in == ']' && (in + 2 < in_start + a_in_size - 1) && 0 == strncmp(in+1, "]>", 2))
		{
			in += 2;
			memcpy(out, "]>", 2);
			out += 2;
			state = _fa_state_start;
		}
		continue;

	UP_STATE_ATTR_SQUO:
		*out++ = *in;
		if (*in == '\'')
			state = _fa_state_tag;
		continue;

	UP_STATE_ATTR_DQUO:
		*out++ = *in;
		if (*in == '"')
			state = _fa_state_tag;
		continue;
	}

	out += _fa_finish(out, state);

	if (ao_len)
		*ao_len = out - out_start;
	return out_start;
}
