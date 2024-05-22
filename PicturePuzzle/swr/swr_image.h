#ifndef _SWR_IMAGE_H_
#define _SWR_IMAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct swr_image
	{
		int width;
		int height;
		unsigned char* pixels;
	}swr_image;

	typedef struct swr_image_tile
	{
		int x;
		int y;
		int row;
		int column;
		swr_image* tile;
	}swr_image_tile;

	typedef struct swr_slice_image_result
	{
		swr_image_tile* tiles;
		int tile_count;
	}swr_slice_image_result;


	enum ByteOrder { LE, BE };

	/* load png using stb lib */
	unsigned char * read_png_using_stb(const char *filename, int *textureWidth, int *textureHeight);
	swr_image* swr_imag_read_png_using_stb(const char *filename);

	/* save image in raw ppm format */
	void write_ppm_raw(const char* filename, unsigned char* pixels, int width, int height);
	void swr_image_write_ppm_raw(const char* filename, swr_image *image);

	/* save image in ascii ppm format */
	void write_ppm_ascii(const char* filename, unsigned char* pixels, int width, int height);
	void swr_image_write_ppm_ascii(const char* filename, swr_image *image);
	
	/* load image of raw ppm format */
	unsigned char * read_ppm_raw(const char *filename, enum ByteOrder byteOrder, int *textureWidth, int *textureHeight);
	swr_image * swr_image_read_ppm_raw(const char *filename, enum ByteOrder byteOrder);

	/* load image of ascii ppm format */
	unsigned char * read_ppm_ascii(const char *filename, int *textureWidth, int *textureHeight);
	swr_image * swr_image_read_ppm_ascii(const char *filename);

	/* make a gradient image */
	unsigned char * create_gradient_image(int ar, int ag, int ab, int aa, int br, int bg, int bb, int ba, int width, int height);
	swr_image * swr_image_create_gradient_image(int ar, int ag, int ab, int aa, int br, int bg, int bb, int ba, int width, int height);

	/* make a checker board pattern */
	unsigned char* create_checker_image(int width, int height, int tileWidth, int tileHeight, int ar, int ag, int ab, int aa, int br, int bg, int bb, int ba);
	swr_image* swr_image_create_checker_image(int width, int height, int tileWidth, int tileHeight, int ar, int ag, int ab, int aa, int br, int bg, int bb, int ba);
	
	/*clone an image*/
	unsigned char* clone_image(unsigned char* image, int width, int height, int number_of_channels);
	swr_image* swr_image_clone(swr_image *image);


	unsigned char* swr_crop_image_pixels(unsigned char* pixels, int width, int height, int crop_start_x, int crop_start_y, int crop_width, int crop_height, int *res_width, int *res_height);
	swr_image* swr_crop_image(swr_image* image, int crop_start_x, int crop_start_y, int crop_width, int crop_height);
	void swr_image_slice_image(swr_image *image, swr_slice_image_result* p_result, int num_rows, int num_cols);
	void swr_destroy_image_tile_array(swr_image_tile * t, int count);

	/* destroy image pointer safely */
	void swr_destroy_pixels(unsigned char *pixels);
	void swr_destroy_image(swr_image *image);

	void dbg_print_swr_image(swr_image *image);
	void dbg_print_swr_pixels(unsigned char *pixels, int width ,int height);

#ifdef __cplusplus
}
#endif

#endif

