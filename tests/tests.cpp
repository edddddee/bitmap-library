#include <cassert>
#include <print>

#include "../bmp.h"

BMP::Color RED = {255, 0, 0, 255};
BMP::Color GREEN = {0, 255, 0, 255};
BMP::Color BLUE = {0, 0, 255, 255};
BMP::Color WHITE = {255, 255, 255, 255};
BMP::Color BLACK = {0, 0, 0, 255};

// Function to test Reading an image, and checking it's colors.
void TestExampleImage() {
  // 200x200 24-bit bitmap
  BMP::Bitmap bmp24;
  bmp24.Read("bmp_24.bmp");
  //   std::println("size of bmp_24: {}", image.vec_pixels.size());
  for (int x = 0; x < 200; x++) {
    for (int y = 0; y < 200; y++) {

      if (x + y <= 200) {
        assert(bmp24.GetPixelColor(x, y) == RED);
      } else if (y > x) {
        assert(bmp24.GetPixelColor(x, y) == GREEN);
      } else {
        assert(bmp24.GetPixelColor(x, y) == BLUE);
      }
    }
  }
  // Modifiera filen
  for (int x = 0; x < 200; x++) {
    for (int y = 0; y < 200; y++) {
      BMP::Color c = bmp24.GetPixelColor(x, y);
      if (c == RED) {
        // Ändra röda pixlar till lila
        bmp24.SetPixel(x, y, BMP::Color{255, 0, 255, 255});
      }
    }
    // Testa Write-metoden och spara den modifierade bilden.
    // Implicit testas Save-metoden också, eftersom Save endast anropar Write
    // fast med samma filnamn som sig själv.
    bmp24.Write("test_output/bmp24_modified.bmp");
  }
}

void TestFill() {
  BMP::Bitmap fill("test_output/fill.bmp", 100, 100);
  fill.Fill(GREEN);
  fill.Save();
}
void TestLine() {
  BMP::Bitmap line("test_output/line.bmp", 100, 100);
  line.Fill(WHITE);
  line.DrawLine(0, 10, 90, 50, RED);
  line.Save();
}

void TestRect() {
  BMP::Bitmap rect("test_output/rect.bmp", 100, 100);
  rect.Fill(WHITE);
  rect.DrawRect(30, 70, 50, 20, BLUE);
  rect.Save();
}
void TestRectFilled() {

  BMP::Bitmap rect("test_output/rectfilled.bmp", 100, 100);
  rect.Fill(WHITE);
  rect.FillRect(30, 70, 50, 20, BLUE);
  rect.Save();
}
void TestCircle() {

  BMP::Bitmap circle("test_output/circle.bmp", 100, 100);
  circle.Fill(BLACK);
  circle.DrawCircle(50, 50, 50, RED);
  circle.Save();
}
void TestCircleFilled() {
  BMP::Bitmap circle("test_output/circlefilled.bmp", 100, 100);
  circle.Fill(BLACK);
  circle.FillCircle(50, 50, 50, RED);
  circle.Save();
}
void TestTriangle() {

  BMP::Bitmap triangle("test_output/triangle.bmp", 100, 100);
  triangle.Fill(BLACK);
  triangle.DrawTriangle(0, 0, 50, 99, 99, 0, BLUE);
  triangle.Save();
}
void TestTriangleFilled() {
  BMP::Bitmap triangle("test_output/trianglefilled.bmp", 100, 100);
  triangle.Fill(BLACK);
  triangle.FillTriangle(0, 0, 50, 99, 99, 0, BLUE);
  triangle.Save();
}

// Testa LoadFromByteArray genom att kopiera rå byte-data från bmp_24.bmp till
// en ny fil
void TestLoadFromByteArray() {
  BMP::Bitmap blank("test_output/bmp_24_clone.bmp", 200, 200, false);
  BMP::Bitmap bmp24("bmp_24.bmp");
  blank.LoadFromByteArray(bmp24.vec_pixels.data(), bmp24.vec_pixels.size());
  blank.Save();
}

int main() {
  TestExampleImage();
  TestFill();
  TestLine();
  TestRect();
  TestRectFilled();
  TestCircle();
  TestCircleFilled();
  TestTriangle();
  TestTriangleFilled();
  TestLoadFromByteArray();
}
