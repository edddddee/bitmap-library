#include "BMP.h"

typedef uint8_t BYTE;


BMP::Color Grey(BMP::Color c)
{
	int avg = (c.red + c.green + c.blue) / 3;
	return BMP::Color{ (uint8_t)avg, (uint8_t)avg, (uint8_t)avg };
}

int main()
{
	BMP::BITMAP myBMP("mybitmap.bmp", 1920, 1080, false);
	myBMP.Fill(BMP::Color{ 255,0,255 });
	myBMP.Save();

	return 0;
}