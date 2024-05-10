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

    //convert color pixels to white.
    for(uint32_t i = 0; i < pixel_array_size; ) {

	//writeback just white color.
	pixel_array[i] = 255;
	pixel_array[i + 1] = 255;
	pixel_array[i + 2] = 255;

	//increment iterator.
	i += 4; //account for alpha as well.
    }

    //draw a thread from nail-7 to nail-25.

    //nail count and threading points.
    uint32_t m_a = 7, m_b = 25, n = 50;

    //nail pitch circle radius.
    double r = img_width / 2.0;

    //nail center angles subtended.
    double theta_a = 2.0 * M_PI * m_a / n, theta_b = 2.0 * M_PI * m_b / n;

    //sines and cosines of angles.
    double s_a = sin(theta_a), s_b = sin(theta_b), s_ba = sin(theta_b - theta_a);
    double c_a = cos(theta_a), c_b = cos(theta_b);

    //slope of the line.
    double m = (s_b - s_a) / (c_b - c_a);

    //y-intercept of the line.
    double c = r * (1 - m - s_ba / (c_b - c_a));

    //sanity check.
    printf("m is %lf and c is %lf\n", m, c);

    //loop thru x-axis' units of length.
    for(uint32_t i = 0; i < img_width; ++i) {		//y-coordinate
	for(uint32_t j = 0; j < img_width; ++j) {	//x-coordinate

	    //calculate the distance of the pixel from the line.
	    double d = fabs(m * j + c - i) / sqrt(1 + m * m);

	    //attenuate if necessary.
	    if(d > 255)
		d = 255;

	    //store this value in each pixel.
	    pixel_array[4 * (i * img_width + j)] = d;
	    pixel_array[4 * (i * img_width + j) + 1] = d;
	    pixel_array[4 * (i * img_width + j) + 2] = d;

	    //alpha: opaque.
	    pixel_array[4 * (i * img_width + j) + 3] = 255;
	}
    }

    //open a file to render the line.
    FILE *copy_image = fopen("line.bmp", "w");

    //write data into the new file.
    fwrite(metadata, pixel_array_offset, 1, copy_image);
    fwrite(pixel_array, pixel_array_size, 1, copy_image);

    //free resources in the end.
    fclose(input_image);
    free(metadata);
    free(pixel_array);
    fclose(copy_image);
}
