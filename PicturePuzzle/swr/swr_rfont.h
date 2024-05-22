#ifndef _SWR_RFONT_H_
#define _SWR_RFONT_H_

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct _swr_rfont
	{
		unsigned char *font_image_pixels;
		unsigned char font_base_char;
		int font_image_width;
		int font_image_height;
		int font_cell_width;
		int font_cell_height;
		int font_height;
		int font_width[256];	
	}swr_rfont;
	
	swr_rfont * swr_rfont_create(const char* font_image_filename, const char* font_properties_csv_filename);
	void swr_rfont_destroy(swr_rfont* font);

	void swr_rfont_print_info(swr_rfont* font);
	



#ifdef __cplusplus
}
#endif

#endif