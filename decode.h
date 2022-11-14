#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types
#include "common.h" // Contains magic string and other common macro

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _DecodeInfo
{
    /* Stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;
    char image_data[MAX_IMAGE_BUF_SIZE];

    /* Decoded File Info */
    char *decode_fname;
    FILE *fptr_decode;
    char extn_decode_file[MAX_FILE_SUFFIX];
    char decode_data[MAX_SECRET_BUF_SIZE];
    int size_decode_file_extn;
    int size_decode_file;

} DecodeInfo;


/* Encoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_output_file(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_data_from_image(int size, FILE *fptr_stego_image, DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
char decode_byte_from_lsb(char *image_buffer);

/* Decode secret file extension size */
Status decode_secret_file_extn_size(FILE *fptr_stego_image, DecodeInfo *decInfo);

/* Decodes secret size data from stego image 8 byte data */
int decode_size_from_lsb(char *buffer);

/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

#endif
