#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <ultrapants/ultrapants.h>

enum _up_state
{
	_up_state_start,
	_up_state_dot,
	_up_state_dotdot,
	_up_state_dash,
	_up_state_dashdash,
	_up_state_tag,
	_up_state_cdata,
	_up_state_attr_squo,
	_up_state_attr_dquo
};

static size_t _up_write_ellipsis(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0xA6;

	return 3;
}

static size_t _up_write_mdash(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0x94;

	return 3;
}

static size_t _up_write_single_quote_start(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0x98;

	return 3;
}

static size_t _up_write_single_quote_end(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0x99;

	return 3;
}

static size_t _up_write_double_quote_start(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0x9C;

	return 3;
}

static size_t _up_write_double_quote_end(char *out)
{
	*out++ = 0xE2;
	*out++ = 0x80;
	*out++ = 0x9D;

	return 3;
}

static size_t _up_write_wrapped_amp(char *out)
{
	char *s = "<span class=\"amp\">&amp;</span>";
	memcpy(out, s, 30);
	return 30;
}

static up_bool _up_should_open_quote(char in)
{
	return in == '(' || isspace(in);
}

char *ultrapants(ultrapants_config a_config, char *a_in, size_t a_in_size)
{
	enum _up_state state = _up_state_start;

	size_t out_size = (size_t)((float)a_in_size * 1.2);
	char *out_start = malloc(out_size);
	char *out       = out_start;

	char *in_start = a_in;
	char *in       = a_in;

	for (; *in; ++in)
	{
		if (out - out_start >= out_size - 7)
		{
			char *out_start_old = out_start;
			out_size = (out_size + 7) * 2;
			out_start = realloc(out_start, out_size);
			if (!out_start)
			{
				free(out_start_old);
				return NULL;
			}
			out = out_start + (out - out_start_old);
		}

		switch (state)
		{
			case _up_state_start:     goto UP_STATE_START;     break;
			case _up_state_dot:       goto UP_STATE_DOT;       break;
			case _up_state_dotdot:    goto UP_STATE_DOTDOT;    break;
			case _up_state_dash:      goto UP_STATE_DASH;      break;
			case _up_state_dashdash:  goto UP_STATE_DASHDASH;  break;
			case _up_state_tag:       goto UP_STATE_TAG;       break;
			case _up_state_cdata:     goto UP_STATE_CDATA;     break;
			case _up_state_attr_squo: goto UP_STATE_ATTR_SQUO; break;
			case _up_state_attr_dquo: goto UP_STATE_ATTR_DQUO; break;
		}

	UP_STATE_START:
		if (*in == '.')
			state = _up_state_dot;
		else if (*in == '-')
			state = _up_state_dash;
		else if (*in == '\'')
		{
			if (in > a_in && _up_should_open_quote(*(in-1)))
				out += _up_write_single_quote_start(out);
			else
				out += _up_write_single_quote_end(out);
		}
		else if (*in == '"')
		{
			if (in > a_in && _up_should_open_quote(*(in-1)))
				out += _up_write_double_quote_start(out);
			else
				out += _up_write_double_quote_end(out);
		}
		else if (*in == '<')
		{
			*out++ = *in;
			state = _up_state_tag;
		}
		else if (*in == '&' && a_config.wrap_amps)
		{
			if ((in + 4 < in_start + a_in_size - 1) && 0 == strncmp(in+1, "amp;", 4))
			{
				in += 4;
				out += _up_write_wrapped_amp(out);
			}
			else
				*out++ = *in;
		}
		else
			*out++ = *in;
		continue;

	UP_STATE_DOT:
		if (*in == '.')
			state = _up_state_dotdot;
		else
		{
			--in;
			*out++ = '.';
			state = _up_state_start;
		}
		continue;

	UP_STATE_DOTDOT:
		if (*in == '.')
		{
			out += _up_write_ellipsis(out);
			state = _up_state_start;
		}
		else
		{
			--in;
			*out++ = '.';
			*out++ = '.';
			state = _up_state_start;
		}
		continue;

	UP_STATE_DASH:
		if (*in == '-')
			state = _up_state_dashdash;
		else
		{
			--in;
			*out++ = '-';
			state = _up_state_start;
		}
		continue;
		
	UP_STATE_DASHDASH:
		if (*in == '-')
		{
			out += _up_write_mdash(out);
			state = _up_state_start;
		}
		else
		{
			--in;
			out += _up_write_mdash(out);
			state = _up_state_start;
		}
		continue;

	UP_STATE_TAG:
		*out++ = *in;
		if (*in == '>')
			state = _up_state_start;
		else if (*in == '\'')
			state = _up_state_attr_squo;
		else if (*in == '"')
			state = _up_state_attr_dquo;
		else if (*in == '!' && (in + 7 < in_start + a_in_size - 1) && 0 == strncmp(in+1, "[CDATA[", 7))
		{
			in += 7;
			memcpy(out, "[CDATA[", 7);
			out += 7;
			state = _up_state_cdata;
		}
		continue;

	UP_STATE_CDATA:
		*out++ = *in;
		if (*in == ']' && (in + 2 < in_start + a_in_size - 1) && 0 == strncmp(in+1, "]>", 2))
		{
			in += 2;
			memcpy(out, "]>", 2);
			out += 2;
			state = _up_state_start;
		}
		continue;

	UP_STATE_ATTR_SQUO:
		*out++ = *in;
		if (*in == '\'')
			state = _up_state_tag;
		continue;

	UP_STATE_ATTR_DQUO:
		*out++ = *in;
		if (*in == '"')
			state = _up_state_tag;
		continue;
	}

	return out_start;
}
