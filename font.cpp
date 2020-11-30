#include "JAFont.h"
#include <fstream>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "msdfgen/msdfgen.h"
#include "msdfgen/msdfgen-ext.h"

static const size_t final_buffer_width = 1024; //
static const size_t final_buffer_height = 1024;
static const size_t spaceBetweenGlyphs = 16;
static const size_t spaceBetweenRows = 16;
static const size_t additionalPixels = 32;

/*
std::vector<unsigned char> createBitmapGlyph(msdfgen::FontHandle* font, unsigned int glyph, size_t* width, size_t* height) {
	return ;
}
*/


std::vector<unsigned char> createSdfGlyph(msdfgen::FontHandle* font, unsigned int glyph, size_t* width, size_t* height) {
	size_t outputWidth = 32;
	size_t outputHeight = 32;
	const float pxRange = 3.0f;
	msdfgen::Shape shape;
	msdfgen::Shape::Bounds bounds;

	msdfgen::loadGlyph(shape, font, glyph);
	if (!shape.validate() || shape.contours.size() <= 0) {
		std::cout << "Bad shape!" << std::endl;
		return (std::vector<unsigned char> )0;
	}
	shape.normalize();
	shape.inverseYAxis = true;

	bounds = shape.getBounds(pxRange);
	double b_r = bounds.r, b_l = bounds.l, b_t = bounds.t, b_b = bounds.b;
	outputWidth = b_r - b_l;
	outputHeight = b_t - b_b;
	std::vector<unsigned char> output((outputWidth-2) * (outputHeight-2));
	//shape.bound(b_l, b_b, b_r, b_t);

	msdfgen::Vector2 scale(5, 5);
	msdfgen::Vector2 frame(outputWidth, outputHeight);
	msdfgen::Vector2 dims(b_r - b_l, b_t - b_b);
	double avgScale = .5 * (scale.x + scale.y);
	//outputWidth = abs(b_r - b_l);
	//outputHeight = abs(b_t - b_b);
	msdfgen::Vector2 translate(-1, -b_b);

	if (dims.x * frame.y < dims.y * frame.x) {
		translate.set(.5 * (frame.x / frame.y * dims.y - dims.x) - b_l, -b_b);
		scale = avgScale = frame.y / dims.y;
	}
	else {
		translate.set(-b_l, .5 * (frame.y / frame.x * dims.x - dims.y) - b_b);
		scale = avgScale = frame.x / dims.x;
	}
	translate += .5 * pxRange / scale;

	double range = pxRange / std::min(scale.x, scale.y);
	msdfgen::Bitmap<float> bitmap(outputWidth, outputHeight);
	generateSDF(bitmap, shape, range, scale, translate);

	size_t final_out_w = outputWidth - 2;
	size_t final_out_h = outputHeight - 2;
	//convert 
	size_t x =	0, y = 0;
	while (y < final_out_h) {
		x = 0;
		while (x < final_out_w) {
			/*
			if (y == 0 || y == final_out_h -1 || x == 0 || x == final_out_w-1) {
				output[(y * final_out_w) + x] = 0xff;
				x++;
				continue;
			}
			*/
			output[(y * final_out_w) + x] = msdfgen::clamp(int(*bitmap(x+2, y) * 0x100), 0xff);
			x++;
		}
		y++;
	}

	*width = final_out_w;
	*height = final_out_h;

	return output;
}





void writeFontData(fontData_t data) {
	std::ofstream stream(data.outPath + ".fontdat", std::ofstream::binary);
	for (int i = 0; i < GLYPH_COUNT; i++) {
		stream.write((const char*)&data.fontInfo.mGlyphs[i].width, sizeof(data.fontInfo.mGlyphs[i].width));
		stream.write((const char*)&data.fontInfo.mGlyphs[i].height, sizeof(data.fontInfo.mGlyphs[i].height));
		stream.write((const char*)&data.fontInfo.mGlyphs[i].horizAdvance, sizeof(data.fontInfo.mGlyphs[i].horizAdvance));
		stream.write((const char*)&data.fontInfo.mGlyphs[i].horizOffset, sizeof(data.fontInfo.mGlyphs[i].horizOffset));
		stream.write((const char*)&data.fontInfo.mGlyphs[i].baseline, sizeof(data.fontInfo.mGlyphs[i].baseline));
		stream.write((const char*)&data.fontInfo.mGlyphs[i].s, sizeof(data.fontInfo.mGlyphs[i].s));
		stream.write((const char*)&data.fontInfo.mGlyphs[i].t, sizeof(data.fontInfo.mGlyphs[i].t));
		stream.write((const char*)&data.fontInfo.mGlyphs[i].s2, sizeof(data.fontInfo.mGlyphs[i].s2));
		stream.write((const char*)&data.fontInfo.mGlyphs[i].t2, sizeof(data.fontInfo.mGlyphs[i].t2));
	}
	stream.write((const char*)&data.fontInfo.mPointSize, sizeof(data.fontInfo.mPointSize));
	stream.write((const char*)&data.fontInfo.mHeight, sizeof(data.fontInfo.mHeight));
	stream.write((const char*)&data.fontInfo.mAscender, sizeof(data.fontInfo.mAscender));
	stream.write((const char*)&data.fontInfo.mDescender, sizeof(data.fontInfo.mDescender));
	stream.write((const char*)&data.fontInfo.mKoreanHack, sizeof(data.fontInfo.mKoreanHack));
	stream.write((const char*)&data.fontInfo.isSDF, sizeof(data.fontInfo.isSDF));


	std::cout << "----------------------------------------" << "\n";
	std::cout << "mPointSize: " << data.fontInfo.mPointSize << "\n";
	std::cout << "mHeight: " << data.fontInfo.mHeight << "\n";
	std::cout << "mAscender: " << data.fontInfo.mAscender << "\n";
	std::cout << "mDescender: " << data.fontInfo.mDescender << std::endl;
	stream.close();
}

void createFontAtlas(fontData_t data) {
	//Init freetype
	FT_Library  library;
	FT_Error error = FT_Init_FreeType(&library);
	if (error)
	{
		std::cout << "FT_Init_Freetype: error" << std::endl;
	}

	//Load font
	FT_Face     face;
	error = FT_New_Face(library, data.ttfPath.c_str(), 0, &face);
	if (error)
	{
		std::cout << "FT_New_Face: error" << std::endl;
	}
	error = FT_Set_Char_Size(face, 640, 640, 96, 96);

	msdfgen::FontHandle* fH = msdfgen::adoptFreetypeFont(face);

	data.fontInfo.mHeight = (int)((float)(face->ascender >> 6) + (float)(face->descender >> 6));
	data.fontInfo.mAscender = (int)((float)(face->ascender >> 6) * data.size);
	data.fontInfo.mDescender = abs((int)((float)(face->descender >> 6) * data.size));//* size_scale;
	data.fontInfo.mPointSize = (int)(data.size * 10.0f);
	data.fontInfo.mKoreanHack = 0;
	data.fontInfo.isSDF = qtrue;

	std::vector<unsigned char> final_buffer(final_buffer_width * final_buffer_height, 1);
	std::vector<unsigned char> glyph_buffer;
	static int currentBufferPositionX = 0, currentBufferPositionY = 0;

	size_t bitmap_width = 0;
	size_t bitmap_height = 0;
	unsigned int glyph = 0;
	size_t maxHeight = 0;

	for (unsigned int i = 32; i < 256; i++) { // creating glyphs
		if (!(i <= 0x7E || i >= 0xA0)) continue;
		glyph = 0;
		bitmap_width = 0;
		bitmap_height = 0;

		if (i >= 192) { /// Load russian symbols
			glyph = i + 848;
		}
		else {
			glyph = i;
		}

		std::cout << "Generating Character #" << glyph << std::endl;
		glyph_buffer = createSdfGlyph(fH, glyph, &bitmap_width, &bitmap_height);

		if (bitmap_height > maxHeight) {
			maxHeight = bitmap_height;
		}

		if ((currentBufferPositionX + bitmap_width + spaceBetweenGlyphs) > final_buffer_width) {
			currentBufferPositionX = 0;
			currentBufferPositionY += maxHeight + spaceBetweenRows;
			maxHeight = 0;
		}

		//resolving glyph metrics
		data.fontInfo.mGlyphs[i].width = bitmap_width;
		data.fontInfo.mGlyphs[i].height = bitmap_height;
		data.fontInfo.mGlyphs[i].horizAdvance = (abs(face->glyph->metrics.horiAdvance >> 6));
		data.fontInfo.mGlyphs[i].horizOffset = face->glyph->metrics.horiBearingX >> 6; //errr
		data.fontInfo.mGlyphs[i].baseline = face->glyph->metrics.horiBearingY >> 6;
		data.fontInfo.mGlyphs[i].s = (float)currentBufferPositionX / (float)final_buffer_width;
		data.fontInfo.mGlyphs[i].t = (float)currentBufferPositionY / (float)final_buffer_height;

		data.fontInfo.mGlyphs[i].s2 = (float)((float)currentBufferPositionX + (float)bitmap_width) / (float)final_buffer_width;
		data.fontInfo.mGlyphs[i].t2 = (float)((float)currentBufferPositionY + (float)bitmap_height) / (float)final_buffer_height;

		//copying glyph into buffer
		size_t y = 0;
		while (y < bitmap_height) {
			size_t x = 0;
			while (x < bitmap_width) {
				//assume buffer is enough large
				size_t mainBufPos = ((currentBufferPositionY + y) * final_buffer_width) + (currentBufferPositionX + x);
				size_t bitmapBufPos = (y * bitmap_width) + x;
				final_buffer[mainBufPos] = glyph_buffer[bitmapBufPos];
				x++;
			}
			y++;
		}
		currentBufferPositionX += bitmap_width + spaceBetweenGlyphs;


	}

	writePng(data.outPath, final_buffer, final_buffer_width, final_buffer_height);
	writeFontData(data);
}