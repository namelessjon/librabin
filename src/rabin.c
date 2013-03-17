/*
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include "rabin.h"

static void pregenerate_polys(rabin_fingerprinter_t *fp) {
    // Calculates result = prime ^ windowsize
    // After that multiplies all 256 bytes with result
    unsigned long long result = 1;
    unsigned int i;
    for (i=0; i<fp->window_size; i++)
        result *= fp->prime;

    for (i=0; i<=256; i++)
        fp->polynomial_map[i] = i * result;

    return;
}

static void create_rabin_ring(rabin_fingerprinter_t *fp) {
    // initializes all the values to 0 and sets things up for cycling of the
    // hash window
    unsigned int i = 0;
    for (i = 0; i < (fp->window_size-1); i++) {
        (fp->cycle+i)->next = (fp->cycle+i+1);
        (fp->cycle+i)->value = 0;
    }
    // close ring
    (fp->cycle+fp->window_size-1)->next  = fp->cycle;
    (fp->cycle+fp->window_size-1)->value = 0;
}

void rabin_fingerprinter_reset(rabin_fingerprinter_t *fp) {
    // reset the ring
    create_rabin_ring(fp);
    fp->curr         = fp->cycle;

    // set blocksize and fingerprint to 0
    fp->blk_sz       = 0;
    fp->fingerprint  = 0;

}

rabin_fingerprinter_t* rabin_fingerprinter_init(unsigned long min_block_size,
                                                unsigned long max_block_size) {

    // basic sanity check.
    if (min_block_size > max_block_size) {
        return 0;
    }

    rabin_fingerprinter_t * fp = malloc(sizeof(rabin_fingerprinter_t));
    if (!fp) {
        return 0;
    }

    fp->prime       = RAB_POLYNOMIAL_CONST;
    fp->min_blk_sz  = min_block_size;
    fp->avg_blk_sz  = RAB_AVG_BLOCKSIZE;
    fp->max_blk_sz  = max_block_size;
    fp->window_size = RAB_WINDOW_SIZE;

    // pre-generate the rabin polynomials
    pregenerate_polys(fp);

    // set up the ring buffer
    create_rabin_ring(fp);

    // set up the buffers
    fp->buffer       = malloc(fp->max_blk_sz);
    if (!fp->buffer) {
        rabin_fingerprinter_free(fp);
        return 0;
    }
    fp->input_buffer = malloc(fp->min_blk_sz);
    if (!fp->input_buffer) {
        rabin_fingerprinter_free(fp);
        return 0;
    }

    // reset to finish setup
    rabin_fingerprinter_reset(fp);

    return fp;
}

void rabin_fingerprinter_free(rabin_fingerprinter_t * fp) {
    if (fp) {
        if (fp->buffer) {
            free(fp->buffer);
            fp->buffer = NULL;
        }
        if (fp->input_buffer) {
            free(fp->input_buffer);
            fp->input_buffer = NULL;
        }
        free(fp);
        fp = NULL;
    }
}

int fingerprint_file(rabin_fingerprinter_t * fp,
                     int fileno,
                     rfp_chunk_callback cb,
                     void *userdata) {
    int ch;
    int i = 0, nread = 0;

    // if there's no file or fingerprinter, give up
    // likewise, if there's no callback there isn't much point ...
    // TODO: check file handle is valid
    if ((!fp)||(!cb))
        return 1;

    // read the file in slabs of the minimum block size into the input buffer
    while ((nread = read(fileno, fp->input_buffer, fp->min_blk_sz))) {

        // loop over the input buffer
        for (i = 0; i < nread; i++) {
            // read off the buffer
            ch = fp->input_buffer[i];

            // copy to the output buffer
            fp->buffer[fp->blk_sz] = ch;

            // rabin stuff
            fp->fingerprint *= fp->prime;
            fp->fingerprint += ch;
            fp->fingerprint -= fp->polynomial_map[fp->curr->value];

            fp->curr->value = ch;
            fp->curr = fp->curr->next;

            fp->blk_sz++;
            // if we have a block we can consider outputting ...
            if ((fp->blk_sz > fp->min_blk_sz)) {

                // ... check for it having a valid fingerprint or being too large 
                if (((fp->fingerprint & fp->avg_blk_sz) == 1) || (fp->blk_sz >= fp->max_blk_sz)) {
                    // call the callback
                    (*cb)(fp->blk_sz, fp->buffer, userdata);
                    // reset the block size
                    fp->blk_sz = 0;
                }
            }
        }
    }
    // if we have any spare bytes
    if (fp->blk_sz != 0) // send them to the callback, too!
        (*cb)(fp->blk_sz, fp->buffer, userdata);
    return 0;
}
