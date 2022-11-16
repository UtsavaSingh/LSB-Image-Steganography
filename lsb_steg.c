/*
 * Name            : Utsava Kumar Singh
 * Date            : 10-11-2022
 * Description     : Code to encode secret file data to source image file by creating new image file with option '-e' and decode data 
                     from new image file to new text file with option '-d'
 * Input:	For option 'e'
		a) Source image file name (.bmp)
		b) Secret text file name (.c / .sh / .txt)
		c) Output image file name (.bmp) (optional)
		Example - ./lsb_steg -e <.bmp file> <.txt/.c/.sh file> [Output file]
		For option 'd'
		a) Stego image file name (.bmp)
		b) Output text file name (.c / .sh / .txt) (optional)
		Example - ./lsb_steg -d <.bmp file> [output file]
 * Output:	Data in secret file copied to Output text file
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

/* Check operation type */
OperationType check_operation_type(char *argv[]);

int main(int argc, char **argv)
{
    //validate the command line arguments count
    if(argc > 2 && argc < 6)
    {
        //Input argument validation
        int operation = check_operation_type(argv);

        if(operation == e_encode)
        {
            printf("  ## Selected encoding ##\n");
            EncodeInfo encInfo;
            if(read_and_validate_encode_args(argc, argv, &encInfo) == e_success)
            {
                printf("INFO : Read and validate function is successfully executed\n");
                // Encoding secret data
                if(do_encoding(&encInfo) == e_success)
                {
                    printf("INFO : ## Encoding done successfully ##\n");
                    fclose(encInfo.fptr_src_image);
                    fclose(encInfo.fptr_secret);
                    fclose(encInfo.fptr_stego_image);
                }
                else
                {
                    printf("ERROR : Encoding operation failed\n");
                    return 1;
                }
            }
            else
            {
                printf("ERROR : Read and validate function is failure\n");
                return 1;
            }
        }
        else if(operation == e_decode)
        {
            printf("  ## Selected decoding ##\n");
            DecodeInfo decInfo;
            if(read_and_validate_decode_args(argc, argv, &decInfo) == e_success)
            {
                printf("INFO : Read and validate function is successfully executed\n");
                // Decoding secret data
                if(do_decoding(&decInfo) == e_success)
                {
                    printf("INFO : ## Decoding done successfully ##\n");
                    fclose(decInfo.fptr_stego_image);
                    fclose(decInfo.fptr_decode);
                }
                else
                {
                    printf("ERROR : Decoding operation failed\n");
                    return 1;
                }
            }
            else
            {
                printf("ERROR : Read and validate function failed\n");
                return 1;
            }
        }
        else
        {
            printf("ERROR : Operation is invalid\n");
            printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [Output file]\n./lsb_steg: Deconding: ./lsb_steg -d <.bmp file> [output file]\n");
            return 1;
        }
    }
    else
    {
        printf("ERROR : Please execute file like below");
        printf("./lsb_steg: Encoding: ./lsb_steg -e <.bmp file> <.txt file> [Output file]\n./lsb_steg: Deconding: ./lsb_steg -d <.bmp file> [output file]\n");
        return 1;
    }

    return 0;
}

/* Check for operation to be performed
 * Input: Command line arguments
 * Output: Operation to be performed
 * Return: integer constant corresponding to operation
 */
OperationType check_operation_type(char *argv[])
{
    if((strcmp(argv[1], "-e")) == 0)
    {
    	return e_encode;
    }
    else if((strcmp(argv[1], "-d")) == 0)
    {
    	return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
