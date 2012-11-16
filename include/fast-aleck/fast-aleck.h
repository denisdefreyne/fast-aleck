#ifndef __FAST_ALECK_FAST_ALECK_H__
#define __FAST_ALECK_FAST_ALECK_H__

#include <stdbool.h>
#include <stddef.h>

// The configuration
typedef struct _fast_aleck_config {
	bool wrap_amps;
	bool wrap_caps;
	bool wrap_quotes;
	bool widont;
} fast_aleck_config;

// Initialize the given configuration to the default parameters (all options
// turned off).
//
// params:
//   config - the config to initialize
void fast_aleck_config_init(fast_aleck_config *config);

// params:
//   config  - the configuration
//   in      - the unenhanced input string
//   in_size - the size, in bytes, of the unenhanced input string
//   out_len - a pointer to where the size of the returned string should be written (can be NULL)
//
// return:
//   the enhanced string, allocated on the heap
char *fast_aleck(fast_aleck_config config, char *in, size_t in_size, size_t *out_len);

#endif
