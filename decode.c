#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/*
 * Read and validate Decode args from argv
 * Inputs: Stego Image file and output file name
 * Output: FILE pointer information for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // step 1 -> check argv[2] is having .bmp or not
    // yes -> store the file name into decInfo -> stego_image_fname = argv[2]
    // no -> return e_failure
    if (argv[2] == NULL || strstr(argv[2], ".bmp") == NULL)
    {
        printf("ERROR: Stego image file is invalid\n");
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];

    // step 2 -> check output file name is given or not
    // yes -> store the output file name
    // no -> store default output file name
    if (argv[4] != NULL)
    {
        decInfo->output_fname = argv[4];
    }
    else if (argv[3] != NULL)
    {
        decInfo->output_fname = argv[3];
    }
    else
    {
        decInfo->output_fname = "decoded_file";
    }

    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }

    // Output file will be opened after decoding the secret extension
    decInfo->fptr_output = NULL;

    return e_success;
}

Status check_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char buffer[8];
    char decoded_magic[20];
    int i;

    // Move to image data after the 54-byte BMP header
    rewind(decInfo->fptr_stego_image);
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    // Read and decode the magic string
    for (i = 0; i < (int)strlen(magic_string); i++)
    {
        if (fread(buffer, sizeof(char), 8, decInfo->fptr_stego_image) != 8)
        {
            printf("ERROR: Unable to read magic string\n");
            return e_failure;
        }

        decoded_magic[i] = decode_byte_from_lsb(buffer);
    }
    decoded_magic[i] = '\0';

    // Compare decoded magic string with MAGIC_STRING
    if (strcmp(decoded_magic, magic_string) == 0)
    {
        return e_success;
    }

    printf("ERROR: Magic string not found. Image is not encoded\n");
    return e_failure;
}

Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo)
{
    char buffer[32];

    // Read 32 bytes from stego image
    if (fread(buffer, sizeof(char), 32, decInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }

    // Decode the extension size
    *size = decode_size_from_lsb(buffer);

    if (*size <= 0 || *size >= (int)sizeof(decInfo->extn_secret_file))
    {
        printf("ERROR: Invalid secret file extension size\n");
        return e_failure;
    }

    return e_success;
}

Status decode_secret_file_extn(int size, DecodeInfo *decInfo)
{
    char buffer[8];
    int i;

    // Read and decode the extension one character at a time
    for (i = 0; i < size; i++)
    {
        if (fread(buffer, sizeof(char), 8, decInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }

        decInfo->extn_secret_file[i] = decode_byte_from_lsb(buffer);
    }

    decInfo->extn_secret_file[size] = '\0';

    return e_success;
}

Status decode_secret_file_size(int *file_size, DecodeInfo *decInfo)
{
    char buffer[32];

    // Read 32 bytes from stego image
    if (fread(buffer, sizeof(char), 32, decInfo->fptr_stego_image) != 32)
    {
        return e_failure;
    }

    // Decode the secret file size
    *file_size = decode_size_from_lsb(buffer);

    if (*file_size < 0)
    {
        printf("ERROR: Invalid secret file size\n");
        return e_failure;
    }

    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];
    char data;
    int i;

    // Read secret data and write it into output file
    for (i = 0; i < decInfo->size_secret_file; i++)
    {
        if (fread(buffer, sizeof(char), 8, decInfo->fptr_stego_image) != 8)
        {
            return e_failure;
        }

        data = decode_byte_from_lsb(buffer);

        if (fwrite(&data, sizeof(char), 1, decInfo->fptr_output) != 1)
        {
            return e_failure;
        }
    }

    return e_success;
}

char decode_byte_from_lsb(char *image_buffer)
{
    int i;
    char data = 0;

    // Repeat for 8 bits
    for (i = 0; i < 8; i++)
    {
        // Get LSB from image byte
        int bit = image_buffer[i] & 1;

        // Shift data left and add the extracted bit
        data = (data << 1) | bit;
    }

    return data;
}

int decode_size_from_lsb(char *image_buffer)
{
    int i;
    int size = 0;

    // Repeat for 32 bits
    for (i = 0; i < 32; i++)
    {
        // Get LSB from image byte
        int bit = image_buffer[i] & 1;

        // Shift size left and add the extracted bit
        size = (size << 1) | bit;
    }

    return size;
}

Status do_decoding(DecodeInfo *decInfo)
{
    int extn_size;

    // step 1 -> check open_decode_files(decInfo) is returning e_success or not
    if (open_decode_files(decInfo) != e_success)
    {
        return e_failure;
    }

    // step 2 -> check magic string is present or not
    if (check_magic_string(MAGIC_STRING, decInfo) != e_success)
    {
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }

    // step 3 -> decode secret file extension size
    if (decode_secret_file_extn_size(&extn_size, decInfo) != e_success)
    {
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }

    // step 4 -> decode secret file extension
    if (decode_secret_file_extn(extn_size, decInfo) != e_success)
    {
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }

    // step 5 -> decode secret file size
    if (decode_secret_file_size(&decInfo->size_secret_file, decInfo) != e_success)
    {
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }

    // step 6 -> open output file
    // If the given output name has no extension, add the decoded extension
    if (strrchr(decInfo->output_fname, '.') == NULL)
    {
        char output_name[256];

        if (snprintf(output_name, sizeof(output_name), "%s%s",
                     decInfo->output_fname, decInfo->extn_secret_file) >= (int)sizeof(output_name))
        {
            printf("ERROR: Output file name is too long\n");
            fclose(decInfo->fptr_stego_image);
            return e_failure;
        }

        decInfo->fptr_output = fopen(output_name, "wb");
    }
    else
    {
        decInfo->fptr_output = fopen(decInfo->output_fname, "wb");
    }

    if (decInfo->fptr_output == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to create output file\n");
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }

    // step 7 -> decode secret file data
    if (decode_secret_file_data(decInfo) != e_success)
    {
        fclose(decInfo->fptr_stego_image);
        fclose(decInfo->fptr_output);
        return e_failure;
    }

    fclose(decInfo->fptr_stego_image);
    fclose(decInfo->fptr_output);

    // return e_success
    return e_success;
}
