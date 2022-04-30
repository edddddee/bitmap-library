#include "BMP.h"

int main()
{

	BMP::BITMAP myBMP("mybitmap.bmp", 1000, 1000, false);
	myBMP.Fill(BMP::Color{ 255,255,255 });
	myBMP.FillRect(200, 400, 600, 200, BMP::Color{ 0,0,255 });
	myBMP.Save();

	return 0;
}
