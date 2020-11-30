#pragma once
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>

#define GLYPH_COUNT 256
typedef enum { qfalse, qtrue } qboolean;

typedef struct
{
	short		width;					// number of pixels wide
	short		height;					// number of scan lines
	short		horizAdvance;			// number of pixels to advance to the next char
	short		horizOffset;			// x offset into space to render glyph
	int			baseline;				// y offset
	float		s;						// x start tex coord
	float		t;						// y start tex coord
	float		s2;						// x end tex coord
	float		t2;						// y end tex coord
} glyphInfo_t;

typedef struct fontInfo_s {
	glyphInfo_t		mGlyphs[GLYPH_COUNT];

	short			mPointSize;
	short			mHeight;				// max height of font
	short			mAscender;
	short			mDescender;

	short			mKoreanHack;
	short			isSDF;
} fontInfo_t;

typedef struct fontData_s {
	std::string ttfPath;
	std::string outPath;
	float		size;
	bool		isSDF;
	fontInfo_t fontInfo;
} fontData_t;

void createFontAtlas(fontData_t info);

void writePng(const std::string &filename, std::vector<unsigned char>& image, unsigned width, unsigned height);