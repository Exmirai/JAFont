#include "JAFont.h"
#include "lodepng/lodepng.h"

void writePng(const std::string &filename, std::vector<unsigned char>& image, unsigned width, unsigned height) {
	std::vector<unsigned char> png;

	unsigned error = lodepng::encode(png, image, width, height, LodePNGColorType::LCT_GREY, 8);
	if (!error) lodepng::save_file(png, filename + ".png");

	//if there's an error, display it
	if (error) std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}