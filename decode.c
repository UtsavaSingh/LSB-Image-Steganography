#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function Definitions for Decoding */

/* Read and validate command line argument
 * Input: Command line Argument count and Arguments wtih File name info
 * Output: File names stored in encoded Info
 * Return: e_success or e_failure
 */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
    if(argc == 3 || argc == 4)
    {
        if(strcmp(strstr(argv[2], "."), ".bmp") == 0)
        {
            decInfo -> stego_image_fname = argv[2];
        }
        else
        {
            printf("ERROR : Stego image %s format should be .bmp\n", argv[2]);
            return e_failure;
        }

        if(argv[3] != NULL)
        {
            if((strcmp(strstr(argv[3], "."), ".txt") == 0) || (strcmp(strstr(argv[3], "."), ".c") == 0) || (strcmp(strstr(argv[3], "."), ".sh") == 0))
            {
                decInfo -> decode_fname = argv[3];
            }
            else
            {
                printf("ERROR : Output file %s extension should be .txt or .c or .sh\n", argv[3]);
                return e_failure;
            }
        }
        else
        {
            decInfo -> decode_fname = NULL;
        }
        return e_success;
    }
    else
    {
        fprintf(stderr,"ERROR : Arguments are not in this format\n");
        printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [Output file]\n");
        printf("./lsb_steg: Deconding: ./lsb_steg -d <.bmp file> [output file]\n");
        return e_failure;
    }
}

/* Decoding the secret file data from stego image
 * Input: FILE info of stego image and secret file
 * Output: Decodes the data from stego image to decode file
 * Return: e_success or e_failure;
 */
Status do_decoding(DecodeInfo *decInfo)
{

    // Open files needed for decoding
    if(open_decode_files(decInfo) == e_success)
    {
        printf("INFO : Done\n");
    }
    else
    {
        printf("ERROR : Opening files are failure\n");
        return e_failure;
    }

    printf("INFO : ## Decoding Procedure Started ##\n");
    // Decode magic string from stego image
    if(decode_magic_string(MAGIC_STRING, decInfo) == e_success)
    {
        printf("INFO : Done\n");
    }
    else
    {
        printf("ERROR : Failed to decode magic string\n");
        return e_failure;
    }

    // Decode secret file extension size from stego image
    if(decode_secret_file_extn_size(decInfo -> fptr_stego_image, decInfo) == e_success)
    {
        printf("INFO : Done, it's %d bytes\n", decInfo->size_decode_file_extn);
    }
    else
    {
        printf("ERROR : Decoding output file extension size failure\n");
        return e_failure;
    }

    // Decode secret file extension from stego image
    if(decode_secret_file_extn(decInfo) == e_success)
    {
        printf("INFO : Done, it's %s file\n", decInfo->extn_decode_file);
        // Check for output file provided or not
        if (decInfo->decode_fname == NULL)
        {
            if (strncmp(decInfo->extn_decode_file, ".txt", 4) == 0)
            {
                decInfo->decode_fname = "decoded.txt";
                printf("INFO : Output file not mentioned. Creating %s as default\n", "decoded.txt");
            }
            else if (strncmp(decInfo->extn_decode_file, ".sh", 3) == 0)
            {
                decInfo->decode_fname = "decoded.sh";
                printf("INFO : Output file not mentioned. Creating %s as default\n", "decoded.sh");
            }
            else
            {
                decInfo->decode_fname = "decoded.c";
                printf("INFO : Output file not mentioned. Creating %s as default\n", "decode.c");
            }

            if (open_output_file(decInfo) == e_success)
            {
                printf("INFO : Opened %s\n", decInfo->decode_fname);
            }
            else
            {
                return e_failure;
            }
        }
        else
        {
            printf("INFO : Output file is mentioned\n");
            if(strncmp(strstr(decInfo->decode_fname, "."), decInfo->extn_decode_file, strlen(strstr(decInfo->decode_fname, "."))) == 0)
            {
                if (open_output_file(decInfo) == e_success)
                {
                    printf("INFO : Opened %s\n", decInfo->decode_fname);
                }
                else
                {
                    return e_failure;
                }
            }
            else
            {
                printf("ERROR : Output file (%s) extension is not matched with decoded file extension (%s)\n", decInfo->decode_fname, decInfo->extn_decode_file);
                return e_failure;
            }
        }
    printf("INFO : Done. Opened all required files\n");
    }
    else
    {
        printf("ERROR : Decoding output file extension failure\n");
        return e_failure;
    }

    // Decode secret file size from stego image
    if(decode_secret_file_size(decInfo) == e_success)
    {
        printf("INFO : Done, it's %d bytes\n", decInfo->size_decode_file);
    }
    else
    {
        printf("ERROR : Decoding %s file size failure\n", decInfo->decode_fname);
        return e_failure;
    }

    // Decode secret file data from stego image
    if(decode_secret_file_data(decInfo) == e_success)
    {
        printf("INFO : Done\n");
    }
    else
    {
        printf("ERROR : Decoding %s file data failure\n", decInfo->decode_fname);
        return e_failure;
    }
    return e_success;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure
 */
Status open_decode_files(DecodeInfo *decInfo)
{
    printf("INFO : Opening required files to decode\n");
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

    	return e_failure;
    }
    else
    {
        printf("INFO : Opened %s\n", decInfo -> stego_image_fname);
    }
    // No failure return e_success
    return e_success;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Decode file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure
 */
Status open_output_file(DecodeInfo *decInfo)
{
    // Opening output file
    decInfo->fptr_decode = fopen(decInfo->decode_fname, "w");
    // Do Error handling
    if (decInfo->fptr_decode == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decode_fname);

    	return e_failure;
    }
    return e_success;
}

/* Decode secret data from image data
 * Input: secret data size, file pointer of stego image file, decode info 
 * Output: Gets source image data and decodes it with secret data by calling another function
 * Return: e_success or e_failure
 */
Status decode_data_from_image(int size, FILE *fptr_stego_image, DecodeInfo *decInfo)
{
    for(int i = 0; i < size; i++)
    {
        fread(decInfo -> image_data, sizeof(char), 8, fptr_stego_image);
        decInfo -> decode_data[i] = decode_byte_from_lsb(decInfo -> image_data);
    }
    return e_success;
}

/* Decode Magic string from stego image
 * Input: magic string and FILEs info
 * Output: Decode magic string from stego image
 * Return: e_success or e_failure
 */

Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    printf("INFO : Decoding magic string signature\n");
    fseek(decInfo -> fptr_stego_image, 54, SEEK_SET);
    decode_data_from_image(strlen(magic_string), decInfo -> fptr_stego_image, decInfo);
    if(strncmp(magic_string, decInfo -> decode_data, strlen(magic_string)) == 0)
    {
        printf("INFO : Decoded and matched\n");
        return e_success;
    }
    else
    {
        printf("ERROR : Magic string not matched");
        return e_failure;
    }
}

/* Decode the secret file extension size from stego image
 * Input: FILEs info
 * Output: Decode file extension size from stego image
 * Return: e_success or e_failure
 */

Status decode_secret_file_extn_size(FILE *fptr_stego_image, DecodeInfo *decInfo)
{
    printf("INFO : Decoding output file extension size\n");
    char str[32];
    fread(str, sizeof(char), 32, fptr_stego_image);
    decInfo -> size_decode_file_extn = decode_size_from_lsb(str);
    return e_success;
}

/* Decodes secret size data from image 8 byte data
 * Input: Image 32 byte data
 * Output: Decoded size
 * Return: decoded size from lsb
 */

int decode_size_from_lsb(char *buffer)
{
    int decoded_size = 0;
    for(int i = 0; i < 32; i++)
    {
        decoded_size = ((int)(buffer[i] & 0X01) << (31 - i)) | decoded_size;
    }
    return decoded_size;
}

/* Decode file extension from stego image
 * Input: Files info
 * Output: Secret data file extension from stego image
 * Return: e_success or e_failure
 */

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    printf("INFO : Decoding output file extension\n");
    decode_data_from_image(decInfo -> size_decode_file_extn, decInfo->fptr_stego_image, decInfo);
    strcpy(decInfo ->extn_decode_file, decInfo ->decode_data);
    return e_success;
}

/* Decode secret file size data from stego image
 * Input: Files info
 * Output: Decode secret file size data from stego image
 * Return: e_success or e_failure
 */

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    printf("INFO : Decoding %s File size\n", decInfo->decode_fname);
    char str[32];
    fread(str, sizeof(char), 32, decInfo -> fptr_stego_image);
    decInfo -> size_decode_file = decode_size_from_lsb(str);
    return e_success;
}

/* Decode secret file data from stego image file
 * Input: Files info
 * Output: Decodes secret data from stego image
 * Return: e_success or e_failure
 */

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("INFO : Decoding %s file data\n", decInfo->decode_fname);
    char secret_buff[decInfo -> size_decode_file];
    for(int i = 0; i < (decInfo -> size_decode_file - 0); i++)
    {
        fread(decInfo -> image_data, sizeof(char), 8, decInfo -> fptr_stego_image);
        secret_buff[i] = decode_byte_from_lsb(decInfo -> image_data);
    }
    fwrite(secret_buff, sizeof(char), (decInfo -> size_decode_file-1), decInfo -> fptr_decode);
    return e_success;
}

/* Decodes secret byte data from image 8 byte data
 * Input: Image 8 byte data
 * Output: Decode data 
 * Return: decoded byte
 */
char decode_byte_from_lsb(char *image_buffer)
{
    char decoded_byte = 0X00;
    for(int i = 0; i < 8; i++)
    {
        decoded_byte = ((image_buffer[i] & 0X01) << (7 - i)) | decoded_byte;
    }
    return decoded_byte;
}
