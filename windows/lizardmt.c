
/**
 * Copyright (c) 2017 Tino Reichardt
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 * You can contact the author at:
 * - lizardmt source repository: https://github.com/mcmilk/zstdmt
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "lizardmt.h"

/**
 * program for testing threaded stuff
 */

static void perror_exit(const char *msg)
{
	printf("%s\n", msg);
	fflush(stdout);
	exit(1);
}

static void version(void)
{
	printf("lizardmt version " VERSION "\n");

	exit(0);
}

static void usage(void)
{
	printf("Usage: lizardmt [options] infile outfile\n\n");
	printf("Otions:\n");
	printf(" -#      set compression level to # (1-10, default:1)\n");
	printf(" -T N    set number of (de)compression threads (default: 2)\n");
	printf(" -i N    set number of iterations for testing (default: 1)\n");
	printf(" -b N    set input chunksize to N KiB (default: auto)\n");
	printf(" -c      compress (default mode)\n");
	printf(" -d      use decompress mode\n");
	printf(" -h      show usage\n");
	printf(" -v      show version\n\n");

	printf("Method options:\n");
	printf(" -M N    use method M of lizard (default:1)\n");
	printf("    1:   fastLZ4: give better decompression speed than LZ4\n");
	printf("    2:   LIZv1: give better ratio than LZ4 keeping 75%% decompression speed\n");
	printf("    3:   fastLZ4 + Huffman: add Huffman coding to fastLZ4\n");
	printf("    4:   LIZv1 + Huffman: add Huffman coding to LIZv1\n\n");

	exit(0);
}

#define MODE_COMPRESS    1
#define MODE_DECOMPRESS  2

/* for the -i option */
#define MAX_ITERATIONS   1000

int my_read_loop(void *arg, LIZARDMT_Buffer * in)
{
	FILE *fd = (FILE *) arg;
	ssize_t done = fread(in->buf, 1, in->size, fd);

#if 0
	printf("fread(), todo=%u done=%u\n", in->size, done);
	fflush(stdout);
#endif

	in->size = done;
	return 0;
}

int my_write_loop(void *arg, LIZARDMT_Buffer * out)
{
	FILE *fd = (FILE *) arg;
	ssize_t done = fwrite(out->buf, 1, out->size, fd);

#if 0
	printf("fwrite(), todo=%u done=%u\n", out->size, done);
	fflush(stdout);
#endif

	out->size = done;
	return 0;
}

static void
do_compress(int threads, int level, int bufsize, FILE * fin, FILE * fout)
{
	LIZARDMT_RdWr_t rdwr;
	size_t ret;

	/* 1) setup read/write functions */
	rdwr.fn_read = my_read_loop;
	rdwr.fn_write = my_write_loop;
	rdwr.arg_read = (void *)fin;
	rdwr.arg_write = (void *)fout;

	/* 2) create compression context */
	LIZARDMT_CCtx *ctx = LIZARDMT_createCCtx(threads, level, bufsize);
	if (!ctx)
		perror_exit("Allocating ctx failed!");

	/* 3) compress */
	ret = LIZARDMT_compressCCtx(ctx, &rdwr);
	if (LIZARDMT_isError(ret))
		perror_exit(LIZARDMT_getErrorString(ret));

	/* 5) free resources */
	LIZARDMT_freeCCtx(ctx);
}

static void do_decompress(int threads, int bufsize, FILE * fin, FILE * fout)
{
	LIZARDMT_RdWr_t rdwr;
	size_t ret;

	/* 1) setup read/write functions */
	rdwr.fn_read = my_read_loop;
	rdwr.fn_write = my_write_loop;
	rdwr.arg_read = (void *)fin;
	rdwr.arg_write = (void *)fout;

	/* 2) create compression context */
	LIZARDMT_DCtx *ctx = LIZARDMT_createDCtx(threads, bufsize);
	if (!ctx)
		perror_exit("Allocating ctx failed!");

	/* 3) compress */
	ret = LIZARDMT_decompressDCtx(ctx, &rdwr);
	if (LIZARDMT_isError(ret))
		perror_exit(LIZARDMT_getErrorString(ret));

	/* 4) free resources */
	LIZARDMT_freeDCtx(ctx);
}

int main(int argc, char **argv)
{
	/* default options: */
	int opt, opt_threads = 2, opt_level = 1;
	int opt_mode = MODE_COMPRESS, opt_method = 1;
	int opt_iterations = 1, opt_bufsize = 0;
	int opt_numbers = 0;
	FILE *fin, *fout;

	while ((opt = getopt(argc, argv, "vhM:T:i:dcb:0123456789")) != -1) {
		switch (opt) {
		case 'v':	/* version */
			version();
		case 'h':	/* help */
			usage();
		case 'M':	/* method */
			opt_method = atoi(optarg);
			break;
		case 'T':	/* threads */
			opt_threads = atoi(optarg);
			break;
		case 'i':	/* iterations */
			opt_iterations = atoi(optarg);
			break;
		case 'd':	/* mode = decompress */
			opt_mode = MODE_DECOMPRESS;
			break;
		case 'c':	/* mode = compress */
			opt_mode = MODE_COMPRESS;
			break;
		case 'b':	/* input buffer in MB */
			opt_bufsize = atoi(optarg);
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			if (opt_numbers == 0)
				opt_level = 0;
			else
				opt_level *= 10;
			opt_level += ((int)opt - 48);
			opt_numbers++;
			break;
		default:
			usage();
		}
	}

	/* prog [options] infile outfile */
	if (argc != optind + 2)
		usage();

	/**
	 * check parameters
	 */

	/* opt_level = 1..LIZARDMT_LEVEL_MAX */
	if (opt_level < 1)
		opt_level = 1;
	else if (opt_level > LIZARDMT_LEVEL_MAX)
		opt_level = LIZARDMT_LEVEL_MAX;

	/**
	 * opt_mthod = 1 (default)
	 * 1) fastLZ4 : compression levels -10...-19
	 * 2) LIZv1 : compression levels -20...-29
	 * 3) fastLZ4 + Huffman : compression levels -30...-39
	 * 4) LIZv1 + Huffman : compression levels -40...-49
	 */
	if (opt_method < 1)
		opt_level = 1;
	else if (opt_level > 4)
		opt_level = 4;

	/* remap to real level */
	opt_level += opt_method * 10;

	/* opt_threads = 1..LIZARDMT_THREAD_MAX */
	if (opt_threads < 1)
		opt_threads = 1;
	else if (opt_threads > LIZARDMT_THREAD_MAX)
		opt_threads = LIZARDMT_THREAD_MAX;

	/* opt_iterations = 1..MAX_ITERATIONS */
	if (opt_iterations < 1)
		opt_iterations = 1;
	else if (opt_iterations > MAX_ITERATIONS)
		opt_iterations = MAX_ITERATIONS;

	/* opt_bufsize is in KiB */
	if (opt_bufsize > 0)
		opt_bufsize *= 1024;

	/* file names */
	fin = fopen(argv[optind], "rb");
	if (fin == NULL)
		perror_exit("Opening infile failed");

	fout = fopen(argv[optind + 1], "wb");
	if (fout == NULL)
		perror_exit("Opening outfile failed");

	for (;;) {
		if (opt_mode == MODE_COMPRESS) {
			do_compress(opt_threads, opt_level, opt_bufsize, fin,
				    fout);
		} else {
			do_decompress(opt_threads, opt_bufsize, fin, fout);
		}

		opt_iterations--;
		if (opt_iterations == 0)
			break;

		fseek(fin, 0, SEEK_SET);
		fseek(fout, 0, SEEK_SET);
	}

	/* exit should flush stdout */
	exit(0);
}