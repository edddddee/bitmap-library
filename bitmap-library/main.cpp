#include "BMP.h"

int main()
{
	// Creates 1920x1080 uncompressed 24-bit RGB bitmap with all pixels set to magenta rgb(255, 0, 255)
	BMP::BITMAP myBMP("mybitmap.bmp", 1920, 1080, false);
	myBMP.Fill(BMP::Color{ 255,0,255 });
	myBMP.Save();

	return 0;
}
