#ifndef __FAST_ALECK_FAST_ALECK_H__
#define __FAST_ALECK_FAST_ALECK_H__

typedef char fa_bool;

typedef struct _fast_aleck_config {
	fa_bool wrap_amps;
	fa_bool wrap_quotes;
} fast_aleck_config;

char *fast_aleck(fast_aleck_config config, char *in, size_t in_size, size_t *ao_len);

#endif
