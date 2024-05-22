#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "swr_rfont.h"
#include "swr_image.h"

/*
Font rendering is based on the app at
https://github.com/CodeheadUK/CBFG
*/
swr_rfont * swr_rfont_create(const char* font_image_filename, const char* font_properties_csv_filename)
{
	FILE *fp;
	char line[1024];
	char str[1024];
	int linenum;
	int i,k,charindex;
	/* create font */
	swr_rfont* f = NULL;
	size_t bytes = sizeof(swr_rfont);
	f = (swr_rfont*)malloc(bytes);
	if (f == NULL)
	{
		return NULL;
	}
	f->font_image_pixels = NULL;

	/* load fontmap pixels */
	f->font_image_pixels = read_ppm_raw(font_image_filename, LE, &f->font_image_width, &f->font_image_height);
	if (f->font_image_pixels == NULL)
	{
		swr_rfont_destroy(f);
		return NULL;
	}
	fp = fopen(font_properties_csv_filename, "r");
	if (fp == NULL) {
		printf("Cannot open file: %s", font_properties_csv_filename);
		swr_rfont_destroy(f);
		return NULL;
	}

	linenum = 0;
	charindex = 0;
	while (fgets(line, 1024, fp))
	{
		++linenum;
		if (linenum == 1 || linenum == 2 || linenum == 6) {
			//printf("%s", line);
			continue;
		}
		
		if (linenum < 9) {
			printf("\n");
			//read first part
			i = 0;
			while (line[i] != ',') {
				str[i] = line[i];
				++i;
			}
			str[i] = '\0';
			//printf("%s", str);
			//printf("\t");

			if (!strcmp("Cell Width", str)) {
				//read second part
				k = 0;
				++i;
				while (line[i] != '\n') {
					str[k] = line[i];
					++i;
					++k;
				}
				str[k] = '\0';
				f->font_cell_width =  atoi(str);
			}
			else if (!strcmp("Cell Height", str)) {
				//read second part
				k = 0;
				++i;
				while (line[i] != '\n') {
					str[k] = line[i];
					++i;
					++k;
				}
				str[k] = '\0';
				f->font_cell_height = atoi(str);
			}
			else if (!strcmp("Base Char", str)) {
				//read second part
				k = 0;
				++i;
				while (line[i] != '\n') {
					str[k] = line[i];
					++i;
					++k;
				}
				str[k] = '\0';
				f->font_base_char = atoi(str);
			}
			else if (!strcmp("Font Height", str))
			{
				//read second part
				k = 0;
				++i;
				while (line[i] != '\n') {
					str[k] = line[i];
					++i;
					++k;
				}
				str[k] = '\0';
				f->font_height = atoi(str);
			}
		}
		else if(charindex < 256) {
			//skip first part
			i = 0;
			while (line[i] != ',') {
				++i;
			}
			//read second part
			k = 0;
			++i;
			while (line[i] != '\n') {
				str[k] = line[i];
				++i;
				++k;
			}
			str[k] = '\0';
			f->font_width[charindex] = atoi(str);
			++charindex;
		}

	}
	//print_font_info(f);
	fclose(fp);
	return f;
}
void swr_rfont_destroy(swr_rfont* font)
{

	if (font != NULL){
		if (font->font_image_pixels != NULL) {
			swr_destroy_pixels(font->font_image_pixels);
		}
		free(font);
		font = NULL;
	}
	
}
/*
typedef struct _swr_rfont
{
unsigned char *font_image_pixels;
int font_image_width;
int font_imag_height;
int font_cell_width;
int font_cell_height;
int font_height;
int font_width[256];
unsigned char font_base_char;
}swr_rfont;
*/
void swr_rfont_print_info(swr_rfont* font)
{
	int i;
	printf("\nimg width %d", font->font_image_width);
	printf("\nimg height %d", font->font_image_height);
	printf("\nfont cell width %d", font->font_cell_width);
	printf("\nfont cell height %d", font->font_cell_height);
	printf("\nfont height %d", font->font_height);
	for (i = 0; i < 255;i++)
		printf("\nfont width of char %d is %d", i, font->font_width[i]);
}