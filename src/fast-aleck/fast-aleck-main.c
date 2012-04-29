#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <fast-aleck/fast-aleck.h>

int main(int argc, char **argv)
{
	// Parse opts
	int wrap_amps = 0, wrap_quotes = 0;
	static struct option longopts[] = {
		{ "wrap-amps",   no_argument, NULL, 'a' },
		{ "wrap-quotes", no_argument, NULL, 'q' },
		{ NULL,          0,           NULL, 0   }
	};
	int ch;
	while ((ch = getopt_long(argc, argv, "aq", longopts, NULL)) != -1)
		switch (ch) {
		case 'a':
			wrap_amps = 1;
			break;
		case 'q':
			wrap_quotes = 1;
			break;
		default:
			fprintf(stderr, "usage: fast-aleck [options] [filename]\n");
			exit(1);
	}
	argc -= optind;
	argv += optind;

	// Get filename
	if (argc != 2)
	{
		fprintf(stderr, "usage: fast-aleck [filename]\n");
		exit(1);
	}
	char *filename = argv[1];

	// Open
	int fd = open(filename, O_RDONLY);
	if (-1 == fd)
	{
		perror("open");
		exit(1);
	}

	// Get size
	struct stat stat;
	int ret = fstat(fd, &stat);
	if (-1 == ret)
	{
		perror("fstat");
		exit(1);
	}

	// Map
	char *in = mmap(0, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (MAP_FAILED == in)
	{
		perror("mmap");
		exit(1);
	}

	// Fast Aleckize
	fast_aleck_config config;
	config.wrap_amps   = wrap_amps;
	config.wrap_quotes = wrap_quotes;
	struct timeval tp1, tp2;
	gettimeofday(&tp1, NULL);
	char *out = fast_aleck(config, in, stat.st_size);
	gettimeofday(&tp2, NULL);

	// Write
	fprintf(stderr, "done in %li.%06is\n", tp2.tv_sec - tp1.tv_sec, tp2.tv_usec - tp1.tv_usec);
	fputs(out, stdout);

	return 0;
}
