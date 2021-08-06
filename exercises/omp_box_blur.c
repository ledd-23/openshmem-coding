#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <omp.h>

#define width 1920 //width of the image

#define height 1080 //height of the image

int32_t array_a[width*height]; //original image
 
int32_t array_b[width*height]; //blurred image

//Return a specified pixel in the image
int32_t apply(int x, int y, int32_t data[]) {
	return data[y * width + x];
}

//Update a specified pixel in the image
void update(int x, int y, int32_t c, int32_t data[]) {
	data[y * width + x] = c;
}

//Get the red component
int32_t red(int32_t c) {
	return (0xFF000000 & c) >> 24;
}

//Get the green component
int32_t green(int32_t c) {
	return (0x00FF0000 & c) >> 16;
}

//Get the blue component
int32_t blue(int32_t c) {
	return (0x0000FF00 & c) >> 8;
}

//Get the alpha component
int32_t alpha(int32_t c) {
	return (0x000000FF & c) >> 0;
}

//Create RGBA from separate components
int32_t rgba(int32_t r, int32_t g, int32_t b, int32_t a) {
	return (r << 24) | (g << 16) | (b << 8) | (a << 0);
}

//Restricts the integer into the specified range
int clamp(int v, int min, int max) {
	if (v < min) 
		return min;
	else if (v > max)
		return max;
	else
		return v;
}

//Computes the blurred RGBA of a pixel
int32_t boxBlurKernel(int32_t data[], int x, int y, int radius) {
	int32_t r = 0;
	int32_t g = 0;
	int32_t b = 0;
	int32_t a = 0;
	int32_t num = 0;
	for (int i = clamp(y - radius, 0, height - 1); i <= clamp(y + radius, 0, height - 1); i ++){
		for(int j = clamp(x - radius, 0, width - 1); j <= clamp(x + radius, 0, width - 1); j ++)
{
		r += red(apply(j, i, data));
		g += green(apply(j, i, data));
		b += blue(apply(j, i, data));
		a += alpha(apply(j, i, data));
		num += 1;
		}

	}
	return rgba(r / num, g / num, b / num, a / num);
}

//Blurs the row of src into dst going from left to right
void blur(int32_t src[], int32_t dst[], int from, int end, int radius) {
	for (int i = from; i <= end; i ++){
		int y = i / width;
		int x = i - y * width;
		update(x, y, boxBlurKernel(src, x, y, radius), dst);
	}
}


int main(int argc, char **argv) {
	int size = omp_get_max_threads();
	int task_length = (height*width) / size;
	
	for (int j = 0; j < height*width; j ++){
		array_a[j] = j;
	}
	
	#pragma omp parallel
	{
		int start = omp_get_thread_num()*task_length;
		int end = start + task_length - 1;
		if (end > width*height - 1)
			end = width*height - 1;
		blur(array_a, array_b, start, end, 3);
	}
	
	/*for (int i = 0; i < width*height; i ++)
		printf("src[%d] = %d to dst[%d] = %d\n", i, array_a[i], i, array_b[i]);*/
	
}
