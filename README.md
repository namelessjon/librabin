# librabin

## What is it?

An implementation of the Rabin-Karp hash in C, useful for content based
splitting of files.  It provides a callback based mechanism for handling the
chucks and supports a configurable maximum and minimum block size.

## What does it offer?

* User definable callbacks for handling chunks.
* Threadsafe (if used with one chunker per thread)
* Very simple (one c file, one header)


## How do I use it?

It's pretty simple.  This is a working program which will print the chunk index
and the size of the chunk to stdout.

    #include <stdio.h>
    #include <rabin.h>


    static void handle_chunk(unsigned long long blk_sz, unsigned char *chunk, void *userdata) {

        unsigned long *i = (unsigned long*) userdata;
        printf("Got chunk %ld (%lld bytes)\n", *i, blk_sz);
        (*i)++;
    }

    int main (int argc, char ** argv) {
        rabin_splitter_t *rs = rabin_splitter_init(32*1024, 128*1024);
        FILE *file;
        unsigned long i = 0;

        if (argc < 2) {
            fprintf(stderr, "Usage: %s file\n", argv[0]);
            return 1;
        }

        // open the file
        file = fopen(argv[1], "rb");
        if (!file) {
            perror(argv[1]);
            return 1;
        }

        // split the file into chunks
        rabin_split_file(rs, fileno(file), &handle_chunk, &i);

        // free the splitter
        rabin_splitter_free(rs);

        return 0;
    }

## Any problems?

Let me know on github at https://github.com/namelessjon/librabin
