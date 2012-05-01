#ifndef __FAST_ALECK_FAST_ALECK_H__
#define __FAST_ALECK_FAST_ALECK_H__

#include <string.h>

typedef char fa_bool;

typedef struct _fast_aleck_config {
	fa_bool wrap_amps;
	fa_bool wrap_caps;
	fa_bool wrap_quotes;
	fa_bool widont;
} fast_aleck_config;

// Initialize a config to the default parameters
//
// params:
//   fast_aleck_config *config - the config to initialize
void fast_aleck_config_init(fast_aleck_config *config);

// params:
//   char *in - the unenhanced input string
//   size_t in_size - the size, in bytes, of the unenhanced input string
//   size_t *ao_len - a pointer to where the size of the returned string should be written (can be NULL)
//
// return:
//   the enhanced string, allocated on the heap
char *fast_aleck(fast_aleck_config config, char *in, size_t in_size, size_t *ao_len);

#endif
