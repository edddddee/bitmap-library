#include "BMP.h"

int main()
{

	BMP::BITMAP myBMP("circle.bmp", 1000, 1000, false);
	myBMP.Fill(BMP::Color{ 255,255,255 });
	myBMP.FillCircle(500, 500, 150, BMP::Color{ 0,255,0 });
	myBMP.Save();

	return 0;
}
