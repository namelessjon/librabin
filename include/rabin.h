/*
 *
 */

#ifndef _RABIN_H
#define _RABIN_H

// taken from pcompress implementation
#define RAB_POLYNOMIAL_CONST 153191
#define RAB_WINDOW_SIZE      48
#define RAB_AVG_BLOCKSIZE    65535


struct rabin_node_t {
    unsigned long long value;
    struct rabin_node_t *next;
};

typedef struct {
    unsigned long long prime;
    unsigned long      window_size;
    unsigned long long blk_sz;
    unsigned long long fingerprint;
    unsigned long      min_blk_sz;
    unsigned long      avg_blk_sz;
    unsigned long      max_blk_sz;
    unsigned long long polynomial_map[256];
    struct rabin_node_t cycle[RAB_WINDOW_SIZE];
    struct rabin_node_t* curr;
    unsigned char      *buffer;

} rabin_fingerprinter_t;

// define FP callback
typedef void (*rfp_chunk_callback)(unsigned long long blk_sz,
                                   unsigned char *chunk,
                                   void *userdata);

// initialize a new rabin fingerprinter, ready for use.
rabin_fingerprinter_t* rabin_fingerprinter_init(unsigned long min_block_size,
                                                unsigned long max_block_size);

// reset a fingerprinter to allow it to be used on a new file
void rabin_fingerprinter_reset(rabin_fingerprinter_t *fp);

// free all memoery associated with a fingerprinter
void rabin_fingerprinter_free(rabin_fingerprinter_t * fingerprinter);

// fingerprint a given file
int fingerprint_file(rabin_fingerprinter_t * fingerprinter,
                     int fileno,
                     rfp_chunk_callback cb,
                     void *userdata);



#endif /* RABIN_H */

