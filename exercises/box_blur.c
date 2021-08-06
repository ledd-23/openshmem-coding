#include <shmem.h>
#include <shmemx.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#define width 128 //width of the image

#define height 256 //height of the image

int rank, size;

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
		data[j + i*width] = shmem_g(&array_a[j + i*width], 0);
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
		int y = i / height;
		int x = i - y * height;
		//printf("Rank %d has x=%d and y =%d\n", rank, x, y);
		update(x, y, boxBlurKernel(src, x, y, radius), dst);
		shmem_p(&array_b[i], array_b[i], 0);
	}
}


int main(int argc, char **argv) {

	int start, end;
	shmem_init();

	rank = shmem_my_pe();
	
	//printf("length=%d\n", sizeof(array_a)/sizeof(int32_t));

	size = shmem_n_pes();
	
	int task_length = (height*width) / size;
	
	start = rank * task_length;
	if (start + task_length >= height*width)
		end = height*width - 1;
	else
		end = start + task_length - 1;
	printf("Rank %d has start=%d and end=%d\n", rank, start, end);
	shmem_barrier_all();

	if (rank == 0){
		for (int j = 0; j < height*width; j ++){
			array_a[j] = j;
		}
	}
	
	shmem_barrier_all();
	
	//TODO: fix big overheasd
	blur(array_a, array_b, start, end, 3);
	
	shmem_barrier_all();
	
	/*if (rank == 0){
		for (int i = 0; i < width*height; i ++)
			printf("src[%d] = %d to dst[%d] = %d\n", i, src[i], i, dst[i]);
	}*/
	
	shmem_finalize();
}