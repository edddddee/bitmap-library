# BMP

### C++ datastruktur för att skapa, läsa, redigera och spara Bitmap-filer (.bmp).

- Läsa och ändra färg på enskilda pixlar
- Metoder för att rita primitiva former (linje, rektangel, cirkel, triangel)


### Kan användas för...
- digital bildbehandling och bildredigering (av bmp-bilder)

# Dokumentation
## Datatyper
**Bitmap**

Konstruktorer som laddar in bitmap-filen från *fn* i minnet.
```C++
Bitmap(const char *fn);
Bitmap(const char *fn, const uint32_t &w, const uint32_t &h, bool alpha = true);
```
**Vertex** 

Kan användas till olika ritrutiner men alla funktioner har varianter some endast tar x- och y-koordinaterna var för sig som argument.
```C++
struct Vertex {
  int x;
  int y;
}
```
**Color**

Beskriver en 24- eller 32-bitars färg.
```C++
struct Color
{
  uint8_t red{0};
  uint8_t green{0};
  uint8_t blue{0};
  uint8_t alpha{255};
};
```
**Pixel**

Beskriver pixelns läge i bilden och dess färgvärde.
```C++
struct Pixel
{
  uint32_t x;
  uint32_t y;
  Color color;
};
```

## Funktioner
**Läsa/spara fil**
```C++
bool Bitmap::Read(const char *fn); // Läser in en filen
bool Bitmap::Write(const char *fn) const; // Sparar den nuvarande bilden i minnet till en fil
bool Bitmap::Save() const { return Write(filename); } // Sparar över den nuvarande bilden i samma fil som laddats in
```
**Setters**
```C++
void Bitmap::SetPixel(int x, int y, const Color &color); // Ändrar en pixels färg
void Bitmap::SetBitDepth(const BIT_DEPTH &bd); // Ändrar bitdjupet i bilden
void Bitmap::SetFileName(const char *fn) { filename = fn; } // Ändrar filens namn
```
**Getters**
```C++
Color Bitmap::GetPixelColor(const int &x, const int &y) const;
uint32_t Bitmap::Width() const;
uint32_t Bitmap::Height() const;
uint32_t Bitmap::GetFileSize() const;
BIT_DEPTH Bitmap::GetBitDepth() const;
```
**Draw routines**
```C++
// Fyller hela bilden av en viss färg.
void Bitmap::Fill(const Color &color); 

// Ritar en linje från (sx, sy) till (ex, ey)
// Använder Bresenham's line algorithm (https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm)
void Bitmap::DrawLine(int sx, int sy,int ex, int ey, Color color); 

// Ritar en icke ifylld rektangel
void Bitmap::DrawRect(const int &x, const int &y,const int &w, const int &h,const Color &color);

// Ritar en ifylld rektangel
void Bitmap::FillRect(const int &x, const int &y,const int &w, const int &h,const Color &color); 

// Ritar en icke ifylld cirkel
// Använder Bresenham's circle algorithm (https://iq.opengenus.org/bresenhams-circle-drawing-algorithm/)
void Bitmap::DrawCircle(const int &xc, const int &yc,const int &r,const Color &color);

// Ritar en ifylld cirkel
void Bitmap::FillCircle(const int &xc, const int &yc,const int &r,const Color &color);

// Ritar en icke ifylld triangel
void Bitmap::DrawTriangle(const int &x1, const int &y1,const int &x2, const int &y2,const int &x3, const int &y3,const Color &color);

// Ritar en ifylld triangel
// Använder Bresenham's triangle rasterization algorithm (http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html#algo2)
void Bitmap::FillTriangle(const int &x1, const int &y1,const int &x2, const int &y2,const int &x3, const int &y3,const Color &color); 
 ```
 
 **Draw routines, vertex-varianten**
 
 Motsvarar ovanstående funktioner, endast funktionsargumenten skiljer.
 ```C++
void Bitmap::DrawLine(const Vertex &vStart, const Vertex &vEnd, const Color &color);
void Bitmap::DrawRect(const Vertex &vTopLeft, const Vertex &vSize, const Color &color);
void Bitmap::FillRect(const Vertex &vTopLeft, const Vertex &vSize, const Color &color);
void Bitmap::DrawCircle(const Vertex &vCenter, const int &r, const Color &color);
void Bitmap::FillCircle(const Vertex &vCenter, const int &r, const Color &color);
void Bitmap::DrawTriangle(const Vertex &v1, const Vertex &v2, const Vertex &v3, const Color &color);
void Bitmap::FillTriangle(Vertex v1, Vertex v2, Vertex v3, const Color &color);
```

**Övrigt**

Laddar in data direkt från en byte-array. Kan vara användbart t.ex. för att förenkla interoperationer med andra program eller bibliotek.
```C++
void Bitmap::LoadFromByteArray(uint8_t *data, int n);
```

# Exempelanvändning
```C++
#include "bmp.h"
int main() {
  // Load into memory
  BMP::Bitmap my_bmp = BMP::Bitmap("image.bmp");
  
  // Put a filled red circle at x=200, y=200 with radius 50
  my_bmp.FillCircle(200, 200, 50, BMP::Color{255, 0, 0});
  
  // Save the edited image
  my_bmp.Write("image2.bmp");
  return 0;
}
```
