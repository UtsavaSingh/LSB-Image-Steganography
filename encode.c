#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions for Encoding */

/* Read and validate command line argument
 * Input: Command line Argument count and Arguments wtih File name info
 * Output: File names stored in encoded Info
 * Return: e_success or e_failure
 */
Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
    // Checking argument count
    if(argc == 4 || argc == 5)
    {
        if(strcmp(strstr(argv[2], "."), ".bmp") == 0)
        {
            encInfo -> src_image_fname = argv[2];
        }
        else
        {
            printf("ERROR : Source image %s format should be .bmp\n", argv[2]);
            return e_failure;
        }

        if((strcmp(strstr(argv[3], "."), ".txt") == 0) || (strcmp(strstr(argv[3], "."), ".c") == 0) || (strcmp(strstr(argv[3], "."), ".sh") == 0))
        {
            encInfo -> secret_fname = argv[3];
        }
        else
        {
            printf("ERROR : Secret file %s format should be .txt or .c or .sh\n", argv[3]);
            return e_failure;
        }

        if(argv[4] != NULL)
        {
            if(strcmp(strstr(argv[4], "."), ".bmp") == 0)
            {
                encInfo -> stego_image_fname = argv[4];
            }
            else
            {
                printf("ERROR : Stego image %s format should be .bmp\n", argv[4]);
                return e_failure;
            }
        }
        else
        {
            encInfo -> stego_image_fname = "default.bmp";
        }
        return e_success;
    }
    else
    {
        fprintf(stderr,"ERROR : Arguments are not in this format\n");
        printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt/.c/.sh file> [Output file]\n");
        printf("./lsb_steg: Deconding: ./lsb_steg -d <.bmp file> [output file]\n");
        return e_failure;
    }
}

/* Encoding the secret file data to stego image
 * Input: FILE info of image, secret file and stego image
 * Output: Encodes the data in secret to stego image
 * Return: e_success or e_failure;
 */
Status do_encoding(EncodeInfo *encInfo)
{
    // Open files needed for encoding
    if(open_encode_files(encInfo) == e_success)
    {
        printf("INFO : Done\n");
    }
    else
    {
        printf("ERROR : Opening files are failure\n");
        return e_failure;
    }

    printf("INFO : ## Encoding procedure started ##\n");
    // Check capacity of .bmp image to encode secret text
    if(check_capacity(encInfo) == e_success)
    {
        printf("INFO : Done. Found OK\n");
    }
    else
    {
        printf("ERROR : Check capacity function failure\n");
        return e_failure;
    }
    
    // Copy header of source file to stego file
    if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
    {
        printf("INFO : Done\n");
    }
    else
    {
        printf("ERROR : Copying image header failure\n");
    }

    // Encode magic string to stego image file
    if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("INFO : Done\n");
    }
    else
    {
        printf("ERROR : Failed to encode magic string\n");
        return e_failure;
    }

    // Encode secret file extension size to stego image file
    if(encode_secret_file_extn_size(strlen(encInfo -> extn_secret_file), encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
    {
        printf("INFO : Done\n");
    }
    else
    {
        printf("ERROR : Encoding secret file extension size failure\n");
        return e_failure;
    }

    // Encode secret file extension to stego image file
    if(encode_secret_file_extn(encInfo -> extn_secret_file, encInfo) == e_success)
    {
        printf("INFO : Done\n");
    }
    else
    {
        printf("ERROR : Encoding secret file extension failure =\n");
        return e_failure;
    }

    // Encode secret file size to stego image file
    if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
    {
        printf("INFO : Done\n");
    }
    else
    {
        printf("ERROR : Encoding secret file size failure\n");
        return e_failure;
    }

    // Encode secret file data to stego image file
    if(encode_secret_file_data(encInfo) == e_success)
    {
        printf("INFO : Done\n");
    }
    else
    {
        printf("ERROR : Encoding secret file data failure\n");
        return e_failure;
    }

    // Copy remaining source image data to stego image file
    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
    {
        printf("INFO : Done\n");
    }
    else
    {
        printf("ERROR : Copy remaining data failure\n");
        return e_failure;
    }
    return e_success;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_encode_files(EncodeInfo *encInfo)
{
    printf("INFO : Opening required files\n");
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR : Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
    else
    {
        printf("INFO : Opened %s\n", encInfo -> src_image_fname);
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname,"r");
    
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR : Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
    else
    {
        printf("INFO : Opened %s\n", encInfo -> secret_fname);
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR : Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
    else
    {
        printf("INFO : Opened %s\n", encInfo -> stego_image_fname);
    }

    // No failure return e_success
    return e_success;
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);

    // Return image capacity
    return width * height * 3;
}

/* Get the file size
 * Input:  File pointer
 * Return: Size of file in bytes
 */
uint get_file_size(FILE *fptr)
{
    uint file_size;
    fseek(fptr, 0, SEEK_END);
    file_size = ftell(fptr);
    rewind(fptr);
    return file_size;
}

/* To check the capacity of source image to handle secret data
 * Input: File info source image, stego image and secret file
 * Output: Get Source image capacity and store in image_capacity
 * Return: e_success or e_failure
 */
Status check_capacity(EncodeInfo *encInfo)
{
    printf("INFO : Checking for %s capacity to handle %s\n", encInfo -> src_image_fname, encInfo -> secret_fname);
    
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
    strcpy(encInfo -> extn_secret_file, strstr(encInfo -> secret_fname, "."));
    
    if(encInfo -> image_capacity > ((strlen(MAGIC_STRING) + sizeof(int) + strlen(encInfo -> extn_secret_file) + sizeof(int) + encInfo -> size_secret_file) * 8))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/* Copy the header bytes data to stego image
 * Input: FILE pointers source and stego image
 * Output: Copies header data of source image to stego image
 * Return: e_success or e_failure
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("INFO : Copying image header\n");
    char str[54];
    rewind(fptr_src_image);
    fread(str, sizeof(char), 54, fptr_src_image);
    fwrite(str, sizeof(char), 54, fptr_dest_image);
    return e_success;
}

/* Encode secret data to image data
 * Input: Secret data, secret data size, File pointer of source and stego image files
 * Output: Gets source image data and encodes it with secret data by calling another function
 * Return: e_success or e_failure
 */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
    for(int i = 0; i < size; i++)
    {
        fread(encInfo -> image_data, sizeof(char), 8, fptr_src_image);
        encode_byte_to_lsb(data[i], encInfo -> image_data);
        fwrite(encInfo ->image_data, sizeof(char), 8, fptr_stego_image);
    }
    return e_success;
}

/* Encode Magic string in stego image
 * Input: magic string and FILEs info
 * Output: Encode magic string in stego image
 * Return: e_success or e_failure
 */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO : Encoding magic string signature\n");
    encode_data_to_image(magic_string, strlen(magic_string), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
    return e_success;
}

/* Encodes secret byte data to image 8 byte data
 * Input: Secret 1 byte data and Image 8 byte data
 * Output: Encode data to image_buffer
 * Return: e_success or e_failure
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    uint mask = 1 << 7;
    for(int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0XFE) | ((data & mask) >> (7 - i));
        mask = mask >> 1;
    }
    return e_success;
}

/* Encode the secret file extenstion size to stego image
 * Input: FILEs info
 * Output: Encode file extenstion size to stego image
 * Return: e_success or e_failure
 */

Status encode_secret_file_extn_size(int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    printf("INFO : Encoding secret file extension size\n");
    char str[32];
    fread(str, sizeof(char), 32, fptr_src_image);
    encode_size_to_lsb(str, size);
    fwrite(str, sizeof(char), 32, fptr_stego_image);
    return e_success;
}

/* Encodes secret size data to image 8 byte data
 * Input: Secret 1 byte data and Image 8 byte data
 * Output: Encode data to image_buffer
 * Return: e_success or e_failure
 */

Status encode_size_to_lsb(char *buffer, int size)
{
    uint mask = 1 << 31;
    for(int i = 0; i < 32; i++)
    {
        buffer[i] = (buffer[i] & 0XFE) | ((size & mask) >> (31-i));
        mask = mask >> 1;
    }
    return e_success;
}

/* Encode file extenstion to stego image
 * Input: Files info
 * Output: Copy source image data to stego image with encoded file extenstion
 * Return: e_success or e_failure
 */

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO : Encoding secret file extension\n");
    encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

/* Encode secret file size data to stego image
 * Input: Files info
 * Output: Encode secret file size data to stego image
 * Return: e_success or e_failure
 */

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    printf("INFO : Encoding secret file size\n");
    char str[32];
    fread(str, sizeof(char), 32, encInfo -> fptr_src_image);
    encode_size_to_lsb(str, file_size);
    fwrite(str, sizeof(char), 32, encInfo -> fptr_stego_image);
    return e_success;
}

/* Encode secret file data to stego image file
 * Input: Files info
 * Output: Encodes secret data to stego image
 * Return: e_success or e_failure
 */

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO : Encoding secret file data\n");
    char secret_buff[encInfo->size_secret_file];
    fgets(secret_buff, encInfo->size_secret_file, encInfo->fptr_secret);
    encode_data_to_image(secret_buff,strlen(secret_buff), encInfo -> fptr_src_image, encInfo -> fptr_stego_image, encInfo);
    return e_success;
}

/* Copy remaining source image data to stego image
 * Input: File pointer of source and stego image
 * Output: Remaining image data copied from source image
 * Return: e_success or e_failure
 */

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO : Copying remaining image data\n");
    char ch;
    while(fread(&ch, sizeof(char), 1, fptr_src) > 0)
        fwrite(&ch, sizeof(char), 1, fptr_dest);
    return e_success;
}

