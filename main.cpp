#include "JAFont.h"

#pragma comment(lib, "msdfgen.lib")
#pragma comment(lib, "freetype.lib")

int main(int argc, char* argv[])
{
	std::cout << (" =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
	std::cout << (" JAfont by UniqueUlysees\n");
	std::cout << (" A Tool for creating fonts for Jedi Academy from TrueType font files.\n");
	std::cout << (" Version 1.0\n");
	std::cout << (" =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");

	fontData_t data;
	data.ttfPath = "D:/GitHub/JAFont/Debug/Helvetica 400.ttf";
	data.outPath = "anewhope";
	data.size = 0.5f;
	data.isSDF = true;
	/* TESTING 
	std::vector<std::string> args(argv, argv + argc);
	if (args.size() <= 1) {
		std::cout << (" usage: JAfont <command> <fontfile> <size> <JAfontname>\n");
		std::cout << (" example: jafont -create verdana.ttf 16 anewhope\n\n");
		return 0;
	}
	fontData_t data;
	std::string type = args[1];
	data.ttfPath = args[2];
	data.size = std::stoi(args[3]);
	data.outPath = args[4];
	if (type == "-bitmap") {
		data.isSDF = false;
	}
	else if (type == "-sdf") {
		data.isSDF = true;
	}
	*/
	createFontAtlas(data);
	system("pause");
	return 1;
}
