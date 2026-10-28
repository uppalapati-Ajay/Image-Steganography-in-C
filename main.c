/*Documentation


Description:

This project is used to hide secret file information inside a BMP
image using Steganography.

The program supports two operations: Encoding and Decoding.

Encoding hides a secret file inside the source BMP image by
modifying the Least Significant Bits (LSB) of image bytes.

Decoding extracts the hidden secret file from the encoded BMP
image and saves it into a separate output file.

The program uses a magic string to identify whether the image
contains hidden data.

It also stores and retrieves the secret file extension, file size
and file data.


main() - Starting point of the program.
         It checks the command line arguments and calls the
         required encoding or decoding function.

check_operation_type() - Identifies the operation requested by the user.
                         It returns e_encode for encoding,
                         e_decode for decoding and
                         e_unsupported for invalid operations.


ENCODING FUNCTIONS:

read_and_validate_encode_args() - Validates the source BMP image,secret file and output image arguments.

open_files() - Opens the source image, secret file and output stego image.

get_image_size_for_bmp() - Reads the width and height of the BMP image and calculates its image capacity.

get_file_size() - Finds the size of the secret file in bytes.

check_capacity() - Checks whether the source image has enough capacity to store the secret file.

copy_bmp_header() - Copies the 54-byte BMP header from the source image to the output image.

encode_magic_string() - Stores the magic string in the LSBs of the image bytes.

encode_secret_file_extn_size() - Encodes the size of the secret file extension.

encode_secret_file_extn() - Encodes the secret file extension into the image.

encode_secret_file_size() - Encodes the size of the secret file.

encode_secret_file_data() - Reads the secret file data and hide it inside the image.

encode_byte_to_lsb() - Encodes one character into the LSBs of eight image bytes.

encode_size_to_lsb() - Encodes an integer into the LSBs of 32 image bytes.

copy_remaining_img_data() - Copies the remaining image bytes into the output image.

do_encoding() - Controls the complete encoding process by calling the required encoding functions.


DECODING FUNCTIONS:

read_and_validate_decode_args() - Validates the stego image and output file arguments.

open_decode_files() - Opens the encoded BMP image for reading.

check_magic_string() - Extracts and verifies the magic string to check whether the image is encoded.

decode_secret_file_extn_size() - Extracts the size of the hidden file extension.

decode_secret_file_extn() - Extracts the hidden file extension.

decode_secret_file_size() - Extracts the original secret file size.

decode_secret_file_data() - Extracts the hidden file data and writes it into the output file.

decode_byte_from_lsb() - Extracts one character from the LSBs of eight image bytes.

decode_size_from_lsb() - Extracts an integer from the LSBs of 32 image bytes.

do_decoding() - Controls the complete decoding process and saves the recovered secret file.


COMMON FILES:

MAGIC_STRING - A predefined string "#*" used to identify an encoded image.

Status - An enumeration used to indicate e_success or e_failure.

OperationType - An enumeration used to identify encoding,decoding or an unsupported operation.

EncodeInfo - A structure that stores encoding-related file pointers, file names, image capacity and file details.

DecodeInfo - A structure that stores decoding-related file pointers, file names, extension and file size.


SAMPLE INPUT:

1. Encoding:

./a.out -e beautiful.bmp secret.txt stego_image.bmp

2. Decoding:

./a.out -d stego_image.bmp recovered_secret.txt


SAMPLE OUTPUT:

1. Encoding:

INFO: Operation type is Encode
INFO: Arguments are valid for encoding
width = 1024
height = 768
INFO: Encoding successful

2. Decoding:

INFO: Operation type is Decode
INFO: Arguments are valid for decoding
INFO: Decoding successful


SAMPLE RECOVERED FILE CONTENT:

SARDAR: CODE RED

At 03:33 AM, the security cameras went offline.
No alarms. No forced entry.

One message appeared on the screen:

"WE KNOW YOU ARE HERE."

The system shut down. All records were erased.

Do not trust the terminal. Do not open unknown files.

CODE RED ACTIVE.

FINAL WARNING: YOU ARE BEING WATCHED.

*/
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

OperationType check_operation_type(char *argv[]);

int main(int argc, char *argv[]) // .aout -e <source_file> <secret_data_file> <optional>
{
    //step 1 -> check check_operation_type(argv[1]) is returning e_encode or not
        // yes -> EncodeInfo encInfo;
            // check read_and_validate_encode_args(argv, &encInfo) is returning e_success or e_failure
                // failure -> print error msg and stop
                // success -> print success msg
                            // check do_encoding(&encInfo) is returning e_success or e_failure
                                    // failure -> print error msg and stop
                                    // success -> print success msg and stop

    EncodeInfo encInfo;
    uint img_size;
    int status;

    if (argc < 2)
    {
        printf("ERROR: Missing arguments\n");
        return 1;
    }

    status = check_operation_type(argv);

    if (status == e_encode)
    {
        printf("INFO: Operation type is Encode\n");

        status = read_and_validate_encode_args(argv, &encInfo);

        if (status == e_success)
        {
            printf("INFO: Arguments are valid for encoding\n");

            if (do_encoding(&encInfo) == e_success)
            {
                printf("INFO: Encoding successful\n");
            }
            else
            {
                printf("ERROR: Encoding failed\n");
                return 1;
            }
        }
        else
        {
            printf("ERROR: Invalid arguments for encoding\n");
            printf("For Encode -> ./a.out -e beautiful.bmp secret.txt stego_img.bmp\n");
            return 1;
        }
    }
    else if (status == e_decode)
    {
        printf("INFO: Operation type is Decode\n");

        DecodeInfo decInfo;

        status = read_and_validate_decode_args(argv, &decInfo);

        if (status == e_success)
        {
            printf("INFO: Arguments are valid for decoding\n");

            if (do_decoding(&decInfo) == e_success)
            {
                printf("INFO: Decoding successful\n");
            }
            else
            {
                printf("ERROR: Decoding failed\n");
                return 1;
            }
        }
        else
        {
            printf("ERROR: Invalid arguments for decoding\n");
            printf("For Decode -> ./a.out -d stego.bmp [recovered_secret.txt]\n");
            return 1;
        }
    }
    else
    {
        printf("INFO: Operation type is Unsupported\n");
        printf("For Encode -> ./a.out -e beautiful.bmp secret.txt stego_img.bmp\n");
        printf("For Decode -> ./a.out -d stego.bmp [recovered_secret.txt]\n");
        return 1;
    }

    return 0;
}

OperationType check_operation_type(char *argv[])
{
    //step 1 -> check symbol is -e or not
        // yes -> return e_encode

    // step 2 -> check symbol is -d or not
        // yes -> return e_decode

    // return e_unsupported

    if (strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}
