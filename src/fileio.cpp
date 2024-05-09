/*************************************************************************************************/
/*  File I/O utilities                                                                           */
/*************************************************************************************************/

#include "fileio.h"
#include "common.h"
#include "fpng.h"



/*************************************************************************************************/
/*  Generic file I/O utils                                                                       */
/*************************************************************************************************/

void* dvz_read_file(const char* filename, DvzSize* size)
{
    /* The returned pointer must be freed by the caller. */
    void* buffer = NULL;
    DvzSize length = 0;
    FILE* f = fopen(filename, "rb");

    if (!f)
    {
        log_error("Could not find %s.", filename);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    length = (DvzSize)ftell(f);
    if (size != NULL)
        *size = length;
    fseek(f, 0, SEEK_SET);
    buffer = (void*)malloc((size_t)length);
    fread(buffer, 1, (size_t)length, f);
    fclose(f);

    return buffer;
}



char* dvz_read_npy(const char* filename, DvzSize* size)
{
    /* Tiny NPY reader that requires the user to know in advance the data type of the file. */

    /* The returned pointer must be freed by the caller. */
    char* buffer = NULL;
    DvzSize length = 0;
    int nread = 0, err = 0;

    FILE* f = fopen(filename, "rb");
    if (!f)
    {
        log_error("the file %s does not exist", filename);
        return NULL;
    }

    // Determine the total file size.
    fseek(f, 0, SEEK_END);
    length = (DvzSize)ftell(f);
    fseek(f, 0, SEEK_SET);

    // const uint32_t MAGIC_SIZE = 6;
    // char magic[MAGIC_SIZE];

    // // Read NPY header.
    // nread = fread(magic, sizeof(char), MAGIC_SIZE, f);
    // if (!nread)
    //     goto error;

    // TODO: check the magic
    // TODO: check the version numbers

    // Determine the header size.
    uint16_t header_len = 0;
    err = fseek(f, 8, SEEK_SET);
    if (err)
        goto error;
    nread = fread(&header_len, sizeof(uint16_t), 1, f);
    if (!nread)
        goto error;
    log_trace("npy file header size is %d bytes", header_len);
    ASSERT(header_len > 0);
    header_len += 10; // NOTE: the header len does NOT include the 10 bytes with the magic string,
                      // the version, the header length

    // Jump to the beginning of the data buffer.
    fseek(f, 0, header_len);
    length -= header_len;
    if (size != NULL)
        *size = length;
    err = fseek(f, header_len, SEEK_SET);
    if (err)
        goto error;

    // Read the data buffer.
    buffer = (char*)calloc((size_t)length, 1);
    ANN(buffer);
    fread(buffer, 1, (size_t)length, f);
    fclose(f);

    return buffer;

error:
    log_error("unable to read the NPY file %s", filename);
    return NULL;
}



int dvz_write_bytes(const char* filename, const char* mode, DvzSize size, const uint8_t* bytes)
{
    FILE* fp;
    fp = fopen(filename, mode);
    if (fp == NULL)
        return 1;
    fwrite(bytes, size, 1, fp);
    fclose(fp);
    return 0;
}



/*************************************************************************************************/
/*  Image file I/O utils                                                                         */
/*************************************************************************************************/

int dvz_write_ppm(const char* filename, uint32_t width, uint32_t height, const uint8_t* image)
{
    // from https://github.com/SaschaWillems/Vulkan/blob/master/examples/screenshot/screenshot.cpp
    FILE* fp;
    fp = fopen(filename, "wb");
    if (fp == NULL)
        return 1;
    // ppm header
    char buffer[256];
    snprintf(buffer, 256, "P6\n%d\n%d\n255\n", (int)width, (int)height);
    fwrite(buffer, strlen(buffer), 1, fp);
    // Write the RGB image.
    fwrite(image, width * height * 3, 1, fp);
    fclose(fp);
    return 0;
}



uint8_t* dvz_read_ppm(const char* filename, int* width, int* height)
{
    FILE* fp;
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        return NULL;
    }

    // ppm header
    // int width, height, depth, c;
    int c;
    char buff[16];

    // read image format
    if (!fgets(buff, sizeof(buff), fp))
    {
        log_error("unable to read image form in  %s", filename);
        fclose(fp);
        return NULL;
    }

    // check the image format
    if (buff[0] != 'P' || buff[1] != '6')
    {
        log_error("invalid image format (must be 'P6') in  %s", filename);
        fclose(fp);
        return NULL;
    }

    // check for comments
    c = getc(fp);
    while (c == '#')
    {
        while (getc(fp) != '\n')
            ;
        c = getc(fp);
    }

    ungetc(c, fp);
    // read image size information
    if (fscanf(fp, "%d %d", width, height) != 2)
    {
        log_error("invalid image size (error loading '%s')", filename);
        return NULL;
    }

    // read rgb component
    int b;
    if (fscanf(fp, "%d", &b) != 1)
    {
        log_error("invalid rgb component (error loading '%s')", filename);
        return NULL;
    }
    ASSERT(b == 255);
    while (fgetc(fp) != '\n')
        ;

    uint32_t size = (uint32_t)(*width * *height * 3);
    ASSERT(size > 0);
    uint8_t* image = (uint8_t*)calloc(size, sizeof(uint8_t));
    fread(image, 1, size, fp);
    fclose(fp);
    return image;
}



/*************************************************************************************************/
/*  PNG I/O                                                                                      */
/*************************************************************************************************/

int dvz_write_png(const char* filename, uint32_t width, uint32_t height, const uint8_t* rgb)
{
    ANN(filename);
    ANN(rgb);
    ASSERT(width > 0);
    ASSERT(height > 0);

    fpng::fpng_init();
    fpng::fpng_encode_image_to_file(filename, rgb, width, height, 3, 0);
    return 0;
}



int dvz_make_png(uint32_t width, uint32_t height, const uint8_t* rgb, DvzSize* size, void** out)
{
    ANN(rgb);
    ANN(size);
    ANN(out);
    ASSERT(width > 0);
    ASSERT(height > 0);

    fpng::fpng_init();
    std::vector<uint8_t> outvec;
    fpng::fpng_encode_image_to_memory(rgb, width, height, 3, outvec, 0);
    *size = outvec.size();
    *out = malloc(*size);
    ANN(*out);
    memcpy(*out, outvec.data(), *size);
    return 0;
}



uint8_t* dvz_load_png(DvzSize size, unsigned char* bytes, uint32_t* width, uint32_t* height)
{
    ASSERT(size > 0);
    ANN(bytes);
    ANN(width);
    ANN(height);

    // Decode the image from memory
    std::vector<uint8_t> image_data;
    uint32_t img_width, img_height;
    uint32_t channels;
    bool success =
        fpng::fpng_decode_memory(bytes, size, image_data, img_width, img_height, channels, 3);

    if (!success)
    {
        fprintf(stderr, "Failed to decode PNG image\n");
        return NULL;
    }

    ASSERT(img_width > 0);
    ASSERT(img_height > 0);
    ASSERT(image_data.size() > 0);

    // Assign the width and height to the pointers provided
    *width = img_width;
    *height = img_height;

    // Check if the decoded image format is RGB (3 channels)
    if (channels != 3)
    {
        fprintf(stderr, "Decoded image is not in RGB format\n");
        return NULL;
    }

    // Allocate memory for the decoded image
    uint8_t* output = (uint8_t*)malloc(img_width * img_height * channels);
    if (output == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for the decoded image\n");
        return NULL;
    }

    // Copy the decoded data to the allocated buffer
    memcpy(output, image_data.data(), img_width * img_height * channels);

    return output;
}
