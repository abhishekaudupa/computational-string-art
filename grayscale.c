#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

int main(int argc, char **argv) {

    //open the source image file.
    FILE *input_image = fopen(argv[1], "r");

    //read the pixel array offset number.
    uint32_t pixel_array_offset;
    fseek(input_image, 10, SEEK_SET);
    fread(&pixel_array_offset, sizeof(pixel_array_offset), 1, input_image);
    printf("Pixel array starts at %u\n", pixel_array_offset);

    //copy the image metadata to memory.
    uint8_t *metadata = malloc(pixel_array_offset * sizeof(*metadata));
    rewind(input_image);
    fread(metadata, pixel_array_offset * sizeof(*metadata), 1, input_image);

    //read image dimensions.
    uint32_t img_width, img_height;
    fseek(input_image, 18, SEEK_SET);
    fread(&img_width, sizeof(img_width), 1, input_image);
    fread(&img_height, sizeof(img_height), 1, input_image);
    printf("Image size is %u x %u\n", img_width, img_height);

    //get image size.
    uint8_t *pixel_array;
    uint32_t pixel_array_size = img_width * img_height * sizeof(*pixel_array) * 4; //because 3 colors per pixel + alpha channel.

    //allocate memory for the pixel array.
    pixel_array = malloc(pixel_array_size);

    //read into pixel array.
    fseek(input_image, pixel_array_offset, SEEK_SET);
    fread(pixel_array, pixel_array_size, 1, input_image);

    //convert color pixels to grayscale.
    for(uint32_t i = 0; i < pixel_array_size; ) {

	//colors.
	uint8_t blue_ch = pixel_array[i];
	uint8_t green_ch = pixel_array[i + 1];
	uint8_t red_ch = pixel_array[i + 2];

	//weighted average according to the formula.
	uint8_t avg = round(0.2829 * blue_ch + 0.5870 * green_ch + 0.1140 * red_ch);

	//writeback.
	pixel_array[i] = avg;
	pixel_array[i + 1] = avg;
	pixel_array[i + 2] = avg;

	//increment iterator.
	i += 4; //account for alpha as well.
    }

    //open a file to copy image.
    FILE *copy_image = fopen("copy_image.bmp", "w");

    //write data into the new file.
    fwrite(metadata, pixel_array_offset, 1, copy_image);
    fwrite(pixel_array, pixel_array_size, 1, copy_image);

    //free resources in the end.
    fclose(input_image);
    free(metadata);
    free(pixel_array);
    fclose(copy_image);
}
