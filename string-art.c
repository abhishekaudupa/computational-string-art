#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

//thread render function prototype.
void generate_thread(uint32_t nail_from, 
	uint32_t nail_to, 
	uint32_t nail_count, 
	uint8_t *pixel_array, 
	uint32_t img_width);

//thread accumulation function prototype.
void copy_thread(uint8_t *destination, uint8_t *source, uint32_t array_size);

//circular clipping function prototype.
void circular_clip_canvas(uint8_t *pixel_array, uint32_t img_width);

//string art generator function prototype.
void make_string_art(uint8_t *grayscale_array,
	uint8_t *working_array,
	uint8_t *accumulation_array,
	uint32_t img_width);

//driver function.
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
    uint8_t *pixel_array, *input_image_grayscale;
    uint32_t pixel_array_size = img_width * img_height * sizeof(*pixel_array); 

    //allocate memory for the grayscale image data.
    input_image_grayscale = malloc(pixel_array_size);

    //read pixels from image file.
    fseek(input_image, pixel_array_offset, SEEK_SET);
    for(uint32_t i = 0; i < pixel_array_size; ++i) {

	//read individual channel values (4 bytes).
	uint8_t blue_ch, green_ch, red_ch, alpha_ch;
	fread(&blue_ch, 1, 1, input_image);
	fread(&green_ch, 1, 1, input_image);
	fread(&red_ch, 1, 1, input_image);
	fread(&alpha_ch, 1, 1, input_image);

	//weighted average according to the formula, to obtain grayscale pixel.
	uint8_t avg = round(0.2829 * blue_ch + 0.5870 * green_ch + 0.1140 * red_ch);

	//copy to array.
	input_image_grayscale[i] = avg;
    }

    //allocate memory for the pixel array. 
    pixel_array = malloc(pixel_array_size);

    //allocate memory to save all threads.
    uint8_t *thread_accumulation_array = malloc(pixel_array_size);

    //clear the thread accum. array (aka make it white).
    for(uint32_t i = 0; i < pixel_array_size; ++i)
	thread_accumulation_array[i] = 255;

    /*//generate all possible threads on the canvas.
      uint32_t nail_count = 30;
      uint32_t thread_count = 1;
      for(uint32_t nail_from = 0; nail_from <= nail_count - 2; ++nail_from) {
      for(uint32_t nail_to = nail_from + 1; nail_to <= nail_count - 1; ++nail_to) {

    //print thread count message.
    printf("Rendering thread %u\n", thread_count++);

    //render a single thread.
    generate_thread(nail_from, nail_to, nail_count, pixel_array, img_width);

    //copy the generated thread to accumulation array.
    copy_thread(thread_accumulation_array, pixel_array, pixel_array_size);
    }
    }*/

    //generate string art here instead...
    make_string_art(input_image_grayscale, pixel_array, thread_accumulation_array, img_width);

    //clip the canvas.
    circular_clip_canvas(thread_accumulation_array, img_width);

    //open a file to render the line.
    FILE *copy_image = fopen("string-art.bmp", "w");

    //write metadata into the new file.
    fwrite(metadata, pixel_array_offset, 1, copy_image);

    //write pixel data into the new file.
    for(uint32_t i = 0; i < img_width * img_width; ++i) {

	//write the colors to file, thrice.
	fwrite(thread_accumulation_array + i, sizeof(*thread_accumulation_array), 1, copy_image);
	fwrite(thread_accumulation_array + i, sizeof(*thread_accumulation_array), 1, copy_image);
	fwrite(thread_accumulation_array + i, sizeof(*thread_accumulation_array), 1, copy_image);

	//write alpha.
	uint8_t alpha = 255;
	fwrite(&alpha, sizeof(alpha), 1, copy_image);
    }

    //free resources in the end.
    fclose(input_image);
    free(metadata);
    free(pixel_array);
    free(input_image_grayscale);
    free(thread_accumulation_array);
    fclose(copy_image);
}

/*
 * Function to render a thread in the supplied pixel array.
 *
 * Input:
 * a. The two nail numbers between which threads are to be strung.
 * b. Total nail count on the canvas.
 * c. The pixel array into which data is to be written.
 * d. The image's width in pixels.
 * 
 * Output:
 * Pixel array is filled with data that renders the requested thread.
 * 
 * Constraints:
 * a. 0 <= nail_from < nail_to < nail_count.
 * b. Pixels array must have (img_width * img_width) number of bytes in it.
 */
void generate_thread(uint32_t nail_from, 
	uint32_t nail_to, 
	uint32_t nail_count, 
	uint8_t *pixel_array, 
	uint32_t img_width) {

    //nail pitch circle radius.
    double r = img_width / 2.0;

    //nail center angles subtended.
    double theta_a = 2.0 * M_PI * nail_from / nail_count, theta_b = 2.0 * M_PI * nail_to / nail_count;

    //sines and cosines of angles.
    double s_a = sin(theta_a), s_b = sin(theta_b), s_ba = sin(theta_b - theta_a);
    double c_a = cos(theta_a), c_b = cos(theta_b);

    //slope of the line.
    double m = (s_b - s_a) / (c_b - c_a);

    //y-intercept of the line.
    double c = r * (1 - m - s_ba / (c_b - c_a));

    //sanity check.
    //printf("m is %lf and c is %lf\n", m, c);

    //core thickness.
    double t_c = 0.5;

    //fray thickness.
    double t_f = 0.5 * t_c;

    //thread core shade value.
    double d_p = 200.0;

    //loop thru x-axis' units of length.
    for(uint32_t i = 0; i < img_width; ++i) {		//y-coordinate
	for(uint32_t j = 0; j < img_width; ++j) {	//x-coordinate

	    //calculate the distance of the pixel from the line.
	    double d = fabs(m * j + c - i) / sqrt(1 + m * m);

	    //run d through the thread formula.
	    if(d >= 0 && d < t_c)
		d = d_p;
	    else if(d >= t_c && d < t_c + t_f)
		d = d_p + (d - t_c) * (255.0 - d_p) / t_f;
	    else
		d = 255.0;

	    //store this value in each pixel.
	    pixel_array[i * img_width + j] = d;
	}
    }
}

/*
 * Function to copy a thread image from one array
 * and accumulate it into another one.
 * 
 * Input:
 * a. The source array containing the thread image data.
 * b. The destination array to save the above thread image data.
 * c. Array size.
 *
 * Output:
 * Thread image data added to the destination array.
 */
void copy_thread(uint8_t *destination, uint8_t *source, uint32_t array_size) {

    //loop thru all pixels.
    for(uint32_t i = 0; i < array_size; ++i) {

	//calculate the resulting pixel shade value.
	uint32_t shade_val = 510 - destination[i] - source[i];

	//write to pixel, inverted.
	if(shade_val > 255)
	    destination[i] = 0;
	else
	    destination[i] = 255 - shade_val;
    }
}

/*
 * Function to circular clip an image.
 * 
 * Inputs:
 * a. The pixel array.
 * b. The image width.
 * 
 * Output:
 * All pixels beyond the radius (= img_width/2) are
 * shaded white. The center of the circle is at a distance
 * of (img_width/2) from both x and y axes.
 */
void circular_clip_canvas(uint8_t *pixel_array, uint32_t img_width) {

    //radius of the circle.
    double r = img_width / 2;

    //loop thru pixels.
    for(uint32_t i = 0; i < img_width; ++i)		//y-coordinate.
	for(uint32_t j = 0; j < img_width; ++j) {	//x-coordinate.

	    //render pixel white, if beyond radius.
	    if(((i - r) * (i - r) + (j - r) * (j - r)) > (r * r))
		pixel_array[i * img_width + j] = 255;
	}
}

void make_string_art(uint8_t *grayscale_array,
	uint8_t *working_array,
	uint8_t *accumulation_array,
	uint32_t img_width) {
    //let's go.
}
