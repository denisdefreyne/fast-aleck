#include <stdlib.h>

enum _up_state
{
	_up_state_start,
	_up_state_dot,
	_up_state_dotdot
};

void ultrapants(char *in, char *out)
{
	enum _up_state state = _up_state_start;

	for (char *ip = in; *ip; ++ip)
	{
		switch (state)
		{
			case _up_state_start:  goto UP_STATE_START;  break;
			case _up_state_dot:    goto UP_STATE_DOT;    break;
			case _up_state_dotdot: goto UP_STATE_DOTDOT; break;
		}

UP_STATE_START:
		if (*ip == '.')
			state = _up_state_dot;
		else
			*out++ = *ip;
		continue;

UP_STATE_DOT:
		if (*ip == '.')
			state = _up_state_dotdot;
		else
		{
			*out++ = '.';
			*out++ = *ip;
			state = _up_state_start;
		}
		continue;

UP_STATE_DOTDOT:
		if (*ip == '.')
		{
			*out++ = 0xE2;
			*out++ = 0x80;
			*out++ = 0xA6;
			state = _up_state_start;
		}
		else
		{
			*out++ = '.';
			*out++ = '.';
			*out++ = *ip;
			state = _up_state_start;
		}
		continue;
	}
}
