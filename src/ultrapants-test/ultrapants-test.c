#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <ultrapants/ultrapants.h>

int main(int argc, char **argv)
{
	int ret;

	// Get filename
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s [filename]\n", argv[0]);
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
	ret = fstat(fd, &stat);
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

	// DEBUG
	puts ("*** INPUT:");
	fputs(in, stdout);

	// Ultrapantsize
	// FIXME *2 is not enough
	char *out = malloc(stat.st_size*2);
	out[0] = '\0';
	ultrapants(in, out);

	// DEBUG
	puts ("\n*** OUTPUT:");
	fputs(out, stdout);

	return 0;
}
