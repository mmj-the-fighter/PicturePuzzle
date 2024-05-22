#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "swr_pixel.h"
#include "swr_image.h"
#include "swr_utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

#define NUM_OF_CHANNELS 4

unsigned char * read_png_using_stb(const char *filename, int *textureWidth, int *textureHeight)
{
	int w, h, bpp;
	int num_of_pixels;
	int i;
	unsigned char *srcloc, *dstloc;
	unsigned char *image;
	/* stbi_info(filename, &textureWidth, &textureHeight, &bytesPerPixel); */
	unsigned char* imgdata_stb = stbi_load(filename, &w, &h, &bpp, STBI_rgb_alpha);
	if (imgdata_stb == NULL) {
		*textureWidth = 0;
		*textureHeight = 0;
		printf("Loading png file %s failed.", filename);
		return NULL;
	}

	num_of_pixels = w * h;
	image = (unsigned char *)malloc(num_of_pixels * 4 * sizeof(unsigned char));
	dstloc = image;
	srcloc = imgdata_stb;

	unsigned char rgba[4];
	for (i = 0; i < num_of_pixels; ++i) {
		rgba[0] = *srcloc++;
		rgba[1] = *srcloc++;
		rgba[2] = *srcloc++;
		rgba[3] = *srcloc++;
		*dstloc++ = rgba[2];
		*dstloc++ = rgba[1];
		*dstloc++ = rgba[0];
		*dstloc++ = rgba[3];
	}
	*textureWidth = w;
	*textureHeight = h;
	stbi_image_free(imgdata_stb);
	return image;
}

swr_image* swr_imag_read_png_using_stb(const char *filename)
{
	swr_image* p_swr_image = (swr_image*)malloc(sizeof(swr_image));
	p_swr_image->pixels = read_png_using_stb(filename, &p_swr_image->width, &p_swr_image->height);
	return p_swr_image;
}

void write_ppm_raw(const char* filename, unsigned char* pixels, int width, int height)
{
	unsigned char rgb[3];
	unsigned char *loc;
	char fn[256];
	FILE *fp;
	int i, num_of_pixels;
	time_t tm;
	
	if (filename != NULL) {
		strcpy(fn, filename);
	}
	else
	{
		/* if filename is not given, Make file name from time stamp*/
		tm = time(NULL);
		strftime(fn, sizeof(fn), "IMG_%Y_%m_%d_%H_%M_%S.ppm", localtime(&tm));
	}
	fp = fopen(fn, "wb");

	if (fp == NULL) {
		return;
	}

	/* Write ppm header */
	fprintf(fp, "P6\n%d %d\n255\n",
		width,
		height);

	/* Dump r,g,b values */
	loc = pixels;
	num_of_pixels = width * height;
	for (i = 0; i < num_of_pixels; ++i)
	{
		rgb[2] = *loc++;
		rgb[1] = *loc++;
		rgb[0] = *loc++;
		loc++;
		fwrite(rgb, sizeof(unsigned char), sizeof(rgb), fp);
	}
	fclose(fp);
}

void swr_image_write_ppm_raw(const char* filename, swr_image *image)
{
	write_ppm_raw(filename, image->pixels, image->width, image->height);
}

void write_ppm_ascii(const char* filename, unsigned char* pixels, int width, int height)
{
	unsigned char rgb[3];
	unsigned char *loc;
	char fn[256];
	FILE *fp;
	int i, num_of_pixels;
	time_t tm;

	if (filename != NULL) {
		strcpy(fn, filename);
	}
	else
	{
		/* if filename is not given, Make file name from time stamp*/
		tm = time(NULL);
		strftime(fn, sizeof(fn), "SCRN_%Y_%m_%d_%H_%M_%S.ppm", localtime(&tm));
	}
	fp = fopen(fn, "w");
	if (fp == NULL) {
		return;
	}

	/* Write ppm header */
	fprintf(fp, "P3\n%d %d\n255\n",
		width,
		height);

	/* Dump r,g,b values */
	loc = pixels;
	num_of_pixels = width * height;
	for (i = 0; i < num_of_pixels; ++i)
	{
		rgb[2] = *loc++;
		rgb[1] = *loc++;
		rgb[0] = *loc++;
		loc++;
		fprintf(fp, "\n%d\n%d\n%d", rgb[0], rgb[1], rgb[2]);
	}
	fclose(fp);
}

void swr_image_write_ppm_ascii(const char* filename, swr_image *image)
{
	write_ppm_ascii(filename, image->pixels, image->width, image->height);
}

unsigned char * read_ppm_raw(const char *filename, enum ByteOrder byteOrder, int *textureWidth, int *textureHeight)
{
	FILE *fp;
	char c;
	int width = 0, height = 0, color_depth = 0;
	unsigned char rgb[3];
	char buffer[3];
	unsigned char *image = 0;
	unsigned char* loc;
	int num_pixels;
	int i;

	fp = fopen(filename, "rb");
	if (fp == NULL) {
		printf("Cannot open file %s", filename);
		return NULL;
	}

	/* read first three characters */
	i = 0;
	while (i < 3) {
		c = fgetc(fp);
		if (c == EOF) {
			printf("\n%s : Unexpected end of file", filename);
			fclose(fp);
			return NULL;
		}
		buffer[i++] = c;
	}

	/* check for P6 */
	if (buffer[0] != 'P' || buffer[1] != '6' || !isspace(buffer[2])) {
		printf("\n%s : P6(raw ppm signature) not found", filename);
		fclose(fp);
		return NULL;
	}

	/* check for comment */
	c = fgetc(fp);
	if (c == '#') {
		while ((c = fgetc(fp)) != '\n') {
			if (c == EOF) {
				printf("\n%s : Unexpected end of file", filename);
				fclose(fp);
				return NULL;
			}
		}
	}
	else if (c == EOF) {
		printf("\n%s : Unexpected end of file", filename);
		fclose(fp);
		return NULL;
	}
	else {
		ungetc(c, fp);
	}

	/* read width */
	fscanf(fp, "%d", &width);

	/* read height */
	fscanf(fp, "%d", &height);

	/* read color depth */
	fscanf(fp, "%d", &color_depth);

	c = fgetc(fp);
	if (c != '\n') {
		return NULL;
	}

	/* read colors */
	num_pixels = width * height;
	image = (unsigned char *)malloc(num_pixels * 4 * sizeof(unsigned char));
	loc = image;
	
	for (i = 0; i < num_pixels; ++i) {
		fread(rgb, sizeof(unsigned char), 3, fp);
		*loc++ = rgb[2];
		*loc++ = rgb[1];
		*loc++ = rgb[0];
		*loc++ = 255;
	}
	*textureWidth = width;
	*textureHeight = height;
	return image;
}

swr_image * swr_image_read_ppm_raw(const char *filename, enum ByteOrder byteOrder)
{
	swr_image* p_swr_image = (swr_image*)malloc(sizeof(swr_image));
	p_swr_image->pixels = read_ppm_raw(filename, byteOrder, &p_swr_image->width, &p_swr_image->height);
	return p_swr_image;
}

unsigned char * read_ppm_ascii(const char *filename, int *textureWidth, int *textureHeight)
{
	FILE *fp;
	char c;
	int width = 0, height = 0, color_depth = 0;
	int r = 0, g = 0, b = 0;
	char buffer[3];
	unsigned char *image = 0;
	unsigned char* loc;
	int num_pixels;
	int i, rv;
	int comment_is_there = 0;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Cannot open file %s", filename);
		return NULL;
	}

	/* read first three characters */
	i = 0;
	while (i < 3) {
		c = fgetc(fp);
		if (c == EOF) {
			printf("\n%s : Unexpected end of file", filename);
			fclose(fp);
			return NULL;
		}
		buffer[i++] = c;
	}

	/* check for P3 */
	if (buffer[0] != 'P' || buffer[1] != '3' || !isspace(buffer[2])) {
		printf("\n%s : P3(ascii ppm signature) not found", filename);
		fclose(fp);
		return NULL;
	}

	/* check for comment */
	c = fgetc(fp);
	if (c == '#') {
		while ((c = fgetc(fp)) != '\n') {
			if (c == EOF) {
				printf("\n%s : Unexpected end of file", filename);
				fclose(fp);
				return NULL;
			}
		}
	}
	else if (c == EOF) {
		printf("\n%s : Unexpected end of file", filename);
		fclose(fp);
		return NULL;
	}
	else {
		ungetc(c, fp);
	}

	/* read width */
	fscanf(fp, "%d", &width);

	/* read height */
	fscanf(fp, "%d", &height);

	/* read color depth */
	fscanf(fp, "%d", &color_depth);

	/* read colors */
	num_pixels = width * height;
	image = (unsigned char *)malloc(num_pixels * 4 * sizeof(unsigned char));
	loc = image;
	for (i = 0; i < num_pixels; ++i) {
		rv = fscanf(fp, "%d%d%d", &r, &g, &b);
		*loc++ = b;
		*loc++ = g;
		*loc++ = r;
		*loc++ = 255;
		if (rv == EOF) {
			break;
		}
	}
	*textureWidth = width;
	*textureHeight = height;
	return image;
}

swr_image * swr_image_read_ppm_ascii(const char *filename)
{
	swr_image* p_swr_image = (swr_image*)malloc(sizeof(swr_image));
	p_swr_image->pixels = read_ppm_ascii(filename, &p_swr_image->width, &p_swr_image->height);
	return p_swr_image;
}


unsigned char * create_gradient_image(int ar, int ag, int ab, int aa, int br, int bg, int bb, int ba, int width, int height)
{
	int i;
	double t, delta;
	int pitch = width * NUM_OF_CHANNELS;
	unsigned char* loc;
	unsigned char* pixels = (unsigned char*)malloc(pitch * height);

	if (pixels == NULL) {
		return NULL;
	}

	/* Fill first row with gradient */
	loc = pixels;
	for (i = 0, t = 0.0, delta = 1.0 / width;
		i < width;
		++i, t += delta)
	{
		*loc++ = (unsigned char)(ab + (bb - ab) * t);
		*loc++ = (unsigned char)(ag + (bg - ag) * t);
		*loc++ = (unsigned char)(ar + (br - ar) * t);
		*loc++ = (unsigned char)(aa + (ba - aa) * t);
	}

	/* Copy first row over all rows */
	loc = pixels + pitch;
	for (i = 1; i < height; ++i)
	{
		memcpy(loc, pixels, pitch);
		loc += pitch;
	}
	return pixels;
}

swr_image * swr_image_create_gradient_image(int ar, int ag, int ab, int aa, int br, int bg, int bb, int ba, int width, int height)
{
	swr_image* p_swr_image = (swr_image*)malloc(sizeof(swr_image));
	p_swr_image->pixels = create_gradient_image(ar, ag, ab, aa, br, bg, bb, ba, width, height);
	p_swr_image->width = width;
	p_swr_image->height = height;
	return p_swr_image;
}

unsigned char* swr_crop_image_pixels(unsigned char* pixels, int width, int height, int crop_start_x, int crop_start_y, int crop_width, int crop_height, int *res_width, int *res_height)
{
	int src_x, src_y, dst_x, dst_y;
	int clipped_crop_start_x;
	int clipped_crop_start_y;
	int clipped_crop_width;
	int clipped_crop_height;
	size_t  cropped_image_size;
	unsigned char* cropped_image_pixels;
	unsigned char *dst_loc, *src_loc;
	int src_pitch;
	int dst_pitch;

	/*compute intersection*/
	swr_rect crop_rect, image_rect, clipped_crop_rect;
	crop_rect.left = crop_start_x;
	crop_rect.top = crop_start_y;
	crop_rect.width = crop_width;
	crop_rect.height = crop_height;
	image_rect.left = 0;
	image_rect.top = 0;
	image_rect.width = width;
	image_rect.height = height;
	swr_intersect_rect_rect(&crop_rect, &image_rect, &clipped_crop_rect);
	clipped_crop_start_x = clipped_crop_rect.left;
	clipped_crop_start_y = clipped_crop_rect.top;
	clipped_crop_width = clipped_crop_rect.width;
	clipped_crop_height = clipped_crop_rect.height;
	src_pitch = width * NUM_OF_CHANNELS;
	dst_pitch = clipped_crop_width * NUM_OF_CHANNELS;
	cropped_image_size = clipped_crop_width * clipped_crop_height * NUM_OF_CHANNELS * sizeof(unsigned char);
	cropped_image_pixels = (unsigned char*)malloc(cropped_image_size);
	int r, g, b, a;
	int src_offset;
	int dst_offset;

	for (src_y = clipped_crop_start_y, dst_y = 0;
		src_y < (clipped_crop_start_y + clipped_crop_height);
		++src_y, ++dst_y)
	{
		for (src_x = clipped_crop_start_x, dst_x = 0;
			src_x < (clipped_crop_start_x + clipped_crop_width);
			++src_x, ++dst_x)
		{
			src_offset = (src_pitch * src_y) + (src_x * NUM_OF_CHANNELS);
			src_loc = pixels + src_offset;
			dst_offset = (dst_pitch * dst_y) + (dst_x * NUM_OF_CHANNELS);
			dst_loc = cropped_image_pixels + dst_offset;
			//b
			b = *src_loc;
			*dst_loc = *src_loc;
			++dst_loc;
			++src_loc;
			

			//g
			g = *src_loc;
			*dst_loc = *src_loc;
			++dst_loc;
			++src_loc;

			//r
			r = *src_loc;
			*dst_loc = *src_loc;
			++dst_loc;
			++src_loc;

			//a
			a = *src_loc;
			*dst_loc = *src_loc;
		}
	}	
	*res_width = clipped_crop_width;
	*res_height = clipped_crop_height;

	return cropped_image_pixels;
}

void dbg_print_swr_image(swr_image *image)
{
	unsigned char *loc;
	int pitch = image->width * NUM_OF_CHANNELS;
	int x, y;
	int r, g, b, a;

	printf("************\n");
	for (y = 0; y < image->height; ++y)
	{
		for (x = 0; x < image->width; ++x)
		{
			loc = image->pixels + (pitch * y) + (x * NUM_OF_CHANNELS);
			b = *loc;
			++loc;
			g = *loc;
			++loc;
			r = *loc;
			++loc;
			a = *loc;
			printf("%d %d %d %d\n", b, g, r, a);
		}
	}

}

void dbg_print_swr_pixels(unsigned char *pixels, int width, int height)
{
	unsigned char *loc;
	int pitch = width * NUM_OF_CHANNELS;
	int x, y;
	int r, g, b, a;

	printf("************\n");
	for (y = 0; y < height; ++y)
	{
		for (x = 0; x < width; ++x)
		{
			loc = pixels + (pitch * y) + (x * NUM_OF_CHANNELS);
			printf("%d ", loc);
			b = *loc;
			++loc;
			g = *loc;
			++loc;
			r = *loc;
			++loc;
			a = *loc;
			printf("%d %d %d %d\n", b, g, r, a);
		}
	}
}

swr_image* swr_crop_image(swr_image* image, int crop_start_x, int crop_start_y, int crop_width, int crop_height)
{
	int res_width;
	int res_height;
	
	unsigned char* pixels = swr_crop_image_pixels(
		image->pixels,
		image->width, image->height, 
		crop_start_x, crop_start_y, crop_width, crop_height,
		&res_width, &res_height);
	
	swr_image* res_img = (swr_image*)malloc(sizeof(swr_image));
	res_img->pixels = pixels;
	res_img->width = res_width;
	res_img->height = res_height;
	
	return res_img;
}

void swr_destroy_image_tile_array(swr_image_tile * t, int count)
{
	int i;
	if (t == NULL){
		return;
	}
	for (i = 0; i < count; i++){
		swr_destroy_image(t[i].tile);
	}
	free(t);
	t = NULL;

}

void swr_image_slice_image(swr_image *image, swr_slice_image_result* p_result, int num_rows, int num_cols)
{
	int x, y;
	unsigned char* loc;
	unsigned char* tile_loc;
	int width = image->width;
	int height = image->height;
	int tile_width = width / num_cols;
	int tile_height = height / num_rows;
	unsigned char* pixels = image->pixels;
	int pitch = width * NUM_OF_CHANNELS;
	int tile_pitch = tile_width * NUM_OF_CHANNELS;
	int cur_row;
	int cur_col;
	int num_tiles = num_rows * num_cols;
	int tile_array_index;
	int i;
	int effective_width;
	int effective_height;
	swr_slice_image_result result;
	unsigned char* tile_pixels;
	swr_image* tile_image;
	result.tile_count = 0;
	result.tiles = NULL;
	effective_width = num_cols*tile_width;
	effective_height = num_rows*tile_height;


	if ((effective_width != width) || (effective_height != height)){
		printf("can't slice image\n");
		printf("cropping image");
		width = effective_width;
		height = effective_height;
	}

	swr_image_tile* p_tile_array = (swr_image_tile*)malloc(sizeof(swr_image_tile)*num_tiles);

	for (tile_array_index = 0; tile_array_index < num_tiles; tile_array_index++) {
		p_tile_array[tile_array_index].column = tile_array_index % num_cols;
		p_tile_array[tile_array_index].row = tile_array_index / num_cols;
		size_t tile_size = tile_width * tile_height * NUM_OF_CHANNELS * sizeof(unsigned char);
		tile_pixels = (unsigned char*)malloc(tile_size);
		tile_image = (swr_image*)malloc(sizeof(swr_image));
		tile_image->pixels = tile_pixels;
		tile_image->width = tile_width;
		tile_image->height = tile_height;
		p_tile_array[tile_array_index].tile = tile_image;
		p_tile_array[tile_array_index].x = 0;
		p_tile_array[tile_array_index].y = 0;
	}

	y = 0;
	while ( y < height )
	{
		x = 0;
		while (x < width)
		{
			cur_col = (x / tile_width);
			cur_row = (y / tile_height);
			tile_array_index = cur_row * num_cols + cur_col;
			tile_image = p_tile_array[tile_array_index].tile;
			loc = pixels + (pitch * y) + (x * NUM_OF_CHANNELS);
			tile_loc = tile_image->pixels + 
				(tile_pitch * p_tile_array[tile_array_index].y) +
				(p_tile_array[tile_array_index].x * NUM_OF_CHANNELS);
			
			//b
			*tile_loc = *loc;
			++tile_loc;
			++loc;
			
			//g
			*tile_loc = *loc;
			++tile_loc;
			++loc;

			//r
			*tile_loc = *loc;
			++tile_loc;
			++loc;

			//a
			*tile_loc = *loc;

			++p_tile_array[tile_array_index].x;
			if (p_tile_array[tile_array_index].x >= tile_image->width){
				p_tile_array[tile_array_index].x = 0;
				++p_tile_array[tile_array_index].y;
			}
			++x;
		}
		++y;
	}
	p_result->tiles = p_tile_array;
	p_result->tile_count = num_tiles;
}

unsigned char* create_checker_image(int width, int height, int tileWidth, int tileHeight, int ar, int ag, int ab, int aa, int br, int bg, int bb, int ba)
{
	int x, y;
	int pitch = width * NUM_OF_CHANNELS;
	unsigned char* loc;
	size_t image_size = pitch * height;
	unsigned char* pixels = (unsigned char*)malloc(image_size);
	int even_row;
	int even_col;
	if (pixels == NULL) {
		return NULL;
	}

	for (y = 0; y < height; ++y)
	{
		for (x = 0; x < width; ++x)
		{
			even_col = ((x / tileWidth) % 2 == 0) ? 1 : 0;
			even_row = ((y / tileHeight) % 2 == 0) ? 1 : 0;
			loc = pixels + (pitch * y) + (x * 4);
			if ((even_row && even_col) || (!even_row && !even_col)) {
				*loc++ = ab;
				*loc++ = ag;
				*loc++ = ar;
				*loc = aa;
			}
			else {
				*loc++ = bb;
				*loc++ = bg;
				*loc++ = br;
				*loc = ba;
			}
		}
	}
	return pixels;
}

swr_image* swr_image_create_checker_image(int width, int height, int tileWidth, int tileHeight, int ar, int ag, int ab, int aa, int br, int bg, int bb, int ba)
{
	swr_image* p_swr_image = (swr_image*)malloc(sizeof(swr_image));
	p_swr_image->pixels = create_checker_image(width, height, tileWidth, tileHeight, ar, ag, ab, aa, br, bg, bb, ba);
	p_swr_image->width = width;
	p_swr_image->height = height;
	return p_swr_image;
}



unsigned char* clone_image(unsigned char* image, int width, int height, int number_of_channels)
{
	size_t image_size = width * height * number_of_channels * sizeof(unsigned char);
	unsigned char* clone = (unsigned char*)malloc(image_size);
	memcpy(clone, image, image_size);
	return clone;
}

swr_image* swr_image_clone(swr_image *src_image)
{
	swr_image* image_copy = (swr_image*)malloc(sizeof(swr_image));
	image_copy->pixels = clone_image(src_image->pixels, src_image->width, src_image->height, 4);
	image_copy->width = src_image->width;
	image_copy->height = src_image->height;
	return image_copy;
}

void swr_destroy_pixels(unsigned char *pixels)
{
	if (pixels != NULL) {
		free(pixels);
		pixels = NULL;
	}
}

void swr_destroy_image(swr_image *image)
{
	if (image != NULL) {
		swr_destroy_pixels(image->pixels);
		free(image);
		image = NULL;
	}
}


