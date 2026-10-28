#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * decoding secret file from stego Image
 * Info about input and output files is
 * also stored
 */

typedef struct _DecodeInfo
{
    /* Stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Output File Info */
    char *output_fname;
    FILE *fptr_output;

    /* Secret File Info */
    char extn_secret_file[5];
    int size_secret_file;

} DecodeInfo;

/* Decoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* Check magic string */
Status check_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode extension size */
Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo);

/* Decode secret file extension */
Status decode_secret_file_extn(int size, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(int *file_size, DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode one byte from LSB of image data */
char decode_byte_from_lsb(char *image_buffer);

/* Decode an integer from LSB of image data */
int decode_size_from_lsb(char *image_buffer);

#endif
