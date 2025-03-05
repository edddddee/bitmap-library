#pragma once
#include <cmath>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace BMP {
struct Vertex {
  int x;
  int y;
};

struct Color {
  uint8_t red{0};
  uint8_t green{0};
  uint8_t blue{0};
  uint8_t alpha{255};

  bool operator==(const Color &other) const {
    return (red == other.red) && (green == other.green) &&
           (blue == other.blue) && (alpha == other.alpha);
  }
};

struct Pixel {
  uint32_t x;
  uint32_t y;
  Color color;
};

struct FileHeader {
  uint16_t signature{0x4D42}; // 2 bytes, "BM" in ascii
  uint32_t file_size{};
  uint16_t reserved1{0};
  uint16_t reserved2{0};
  uint32_t offset_data{54};

  void Print() // For debugging puposes
  {
    int w = 12;
    std::cout.width(w);
    std::cout << std::left << "Signature: " << std::right << signature << "\n";
    std::cout.width(w);
    std::cout << std::left << "File size: " << std::right << file_size
              << " Bytes\n";
    std::cout.width(w);
    std::cout << std::left << "Reserved 1: " << std::right << reserved1 << "\n";
    std::cout.width(w);
    std::cout << std::left << "Reserved 2: " << std::right << reserved2 << "\n";
    std::cout.width(w);
    std::cout << std::left << "Offset: " << std::right << offset_data << "\n\n";
  }
};

struct Infoheader {
  uint32_t header_size{40};
  uint32_t width{};
  uint32_t height{};

  uint16_t planes{1};
  uint16_t bits_per_pixel{};
  uint32_t compression{0};
  uint32_t image_size{0};
  uint32_t x_res{0};
  uint32_t y_res{0};
  uint32_t colors_used{0};
  uint32_t colors_important{0};
};

namespace UTILS {
uint32_t bytes_to_uint32(uint8_t *data);

uint16_t bytes_to_uint16(uint8_t *data);

uint8_t *uint32_to_bytes(uint32_t data);

uint8_t *uint16_to_bytes(uint16_t data);
} // namespace UTILS

enum class BIT_DEPTH { BD_24, BD_32 };

class Bitmap {
public: // change to protected later
  FileHeader file_header{};
  Infoheader info_header{};
  std::vector<uint8_t> vec_pixels{};
  const char *filename{};
  BIT_DEPTH bit_depth{};

public:
  Bitmap(){};
  Bitmap(const char *fn) {
    filename = fn;
    Read(fn);
  }
  Bitmap(const char *fn, const uint32_t &w, const uint32_t &h,
         bool alpha = true) {
    filename = fn;
    info_header.width = w;
    info_header.height = h;
    int channels;
    int padding;
    if (alpha) {
      channels = 4;
      padding = 0;
      SetBitDepth(BIT_DEPTH::BD_32);
    } else {
      channels = 3;
      padding = w % 4;
      SetBitDepth(BIT_DEPTH::BD_24);
    }
    vec_pixels.clear();
    vec_pixels.resize((channels * w + padding) * h, 0);
    file_header.file_size =
        file_header.offset_data + (uint32_t)vec_pixels.size();
  }

public:
  bool Read(const char *fn);
  bool Write(const char *fn) const;
  bool Save() const { return Write(filename); }

public:
  // Setters
  void SetPixel(int x, int y, const Color &color);
  void SetBitDepth(const BIT_DEPTH &bd);
  void SetFileName(const char *fn) { filename = fn; }

public:
  // Drawing routines
  void Fill(const Color &color);
  // Implements Bresenham's line algorithm
  // (https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm)
  void DrawLine(int sx, int sy, int ex, int ey, Color color);
  void DrawRect(const int &x, const int &y, const int &w, const int &h,
                const Color &color);
  void FillRect(const int &x, const int &y, const int &w, const int &h,
                const Color &color);
  // Implements Bresenham's circle algorithm
  // (https://iq.opengenus.org/bresenhams-circle-drawing-algorithm/)
  void DrawCircle(const int &xc, const int &yc, const int &r,
                  const Color &color);
  void FillCircle(const int &xc, const int &yc, const int &r,
                  const Color &color);
  void DrawTriangle(const int &x1, const int &y1, const int &x2, const int &y2,
                    const int &x3, const int &y3, const Color &color);
  // Implements Bresenham's triangle rasterization algorithm
  // (http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html#algo2)
  void FillTriangle(const int &x1, const int &y1, const int &x2, const int &y2,
                    const int &x3, const int &y3, const Color &color);

  void DrawLine(const Vertex &vStart, const Vertex &vEnd, const Color &color);
  void DrawRect(const Vertex &vTopLeft, const Vertex &vSize,
                const Color &color);
  void FillRect(const Vertex &vTopLeft, const Vertex &vSize,
                const Color &color);
  void DrawCircle(const Vertex &vCenter, const int &r, const Color &color);
  void FillCircle(const Vertex &vCenter, const int &r, const Color &color);
  void DrawTriangle(const Vertex &v1, const Vertex &v2, const Vertex &v3,
                    const Color &color);
  void FillTriangle(Vertex v1, Vertex v2, Vertex v3, const Color &color);

public:
  // Getters
  Color GetPixelColor(const int &x, const int &y) const;
  uint32_t Width() const { return info_header.width; }
  uint32_t Height() const { return info_header.height; }
  uint32_t GetFileSize() const { return file_header.file_size; }
  BIT_DEPTH GetBitDepth() const { return bit_depth; }

public:
  void LoadFromByteArray(uint8_t *data, int n);
};

uint32_t UTILS::bytes_to_uint32(uint8_t *data) {
  uint32_t result = 0;
  for (int i = 0; i < 4; i++)
    result += data[i] * (1 << (8 * i));
  return result;
}

uint16_t UTILS::bytes_to_uint16(uint8_t *data) {
  uint16_t result = 0;
  for (int i = 0; i < 2; i++)
    result += data[i] * (1 << (8 * i));
  return result;
}

uint8_t *UTILS::uint32_to_bytes(uint32_t data) {
  uint8_t *result = new uint8_t[4];
  result[0] = (data & 0x000000FF) >> 0;
  result[1] = (data & 0x0000FF00) >> 8;
  result[2] = (data & 0x00FF0000) >> 16;
  result[3] = (data & 0xFF000000) >> 24;
  return result;
}

uint8_t *UTILS::uint16_to_bytes(uint16_t data) {
  uint8_t *result = new uint8_t[2];
  result[0] = (data & 0x00FF) >> 0;
  result[1] = (data & 0xFF00) >> 8;
  return result;
}

bool Bitmap::Read(const char *fn) {
  // Open file with name fn
  std::ifstream infile(fn, std::ios::binary);
  if (!infile.is_open()) {
    std::cout << "Failed to read " << fn << "\n";
    return false;
  }

  // Create buffer to import data later passed onto Bitmap object
  std::vector<uint8_t> buffer{std::istreambuf_iterator<char>(infile),
                              std::istreambuf_iterator<char>()};
  // Load into file header struct
  file_header.signature = UTILS::bytes_to_uint16(&buffer[0x0000]);
  file_header.file_size = UTILS::bytes_to_uint32(&buffer[0x0002]);
  file_header.reserved1 = UTILS::bytes_to_uint16(&buffer[0x0006]);
  file_header.reserved2 = UTILS::bytes_to_uint16(&buffer[0x0008]);
  file_header.offset_data = UTILS::bytes_to_uint32(&buffer[0x000A]);

  // Load into info header struct
  info_header.header_size = UTILS::bytes_to_uint32(&buffer[0x000E]);
  info_header.width = UTILS::bytes_to_uint32(&buffer[0x0012]);
  info_header.height = UTILS::bytes_to_uint32(&buffer[0x0016]);
  info_header.planes = UTILS::bytes_to_uint16(&buffer[0x001A]);
  info_header.bits_per_pixel = UTILS::bytes_to_uint16(&buffer[0x001C]);
  info_header.compression = UTILS::bytes_to_uint32(&buffer[0x001E]);
  info_header.image_size = UTILS::bytes_to_uint32(&buffer[0x0022]);
  info_header.x_res = UTILS::bytes_to_uint32(&buffer[0x0026]);
  info_header.y_res = UTILS::bytes_to_uint32(&buffer[0x002A]);
  info_header.colors_used = UTILS::bytes_to_uint32(&buffer[0x002E]);
  info_header.colors_important = UTILS::bytes_to_uint32(&buffer[0x0032]);

  // Set color depth
  switch (info_header.bits_per_pixel) {
  case 24:
    bit_depth = BIT_DEPTH::BD_24;
    break;
  case 32:
    bit_depth = BIT_DEPTH::BD_32;
    break;
  default:
    std::cout << "Unsupported bit depth: " << info_header.bits_per_pixel
              << "\n";
  }

  // Load remaining data in buffer into
  LoadFromByteArray(&buffer[file_header.offset_data],
                    file_header.file_size - file_header.offset_data);

  return true;
}

bool Bitmap::Write(const char *fn) const {
  // Open/Create new file with name stored in fn
  std::ofstream outfile(fn, std::ios::binary);

  if (!outfile.is_open())
    return false; // Could not open/create file

  // Create byte buffer to store output data
  std::vector<uint8_t> buffer(file_header.file_size);

  // Write file header data
  memcpy(&buffer[0x0000], UTILS::uint16_to_bytes(file_header.signature),
         2 * sizeof(uint8_t));
  memcpy(&buffer[0x0002], UTILS::uint32_to_bytes(file_header.file_size),
         4 * sizeof(uint8_t));
  memcpy(&buffer[0x0004], UTILS::uint16_to_bytes(file_header.reserved1),
         2 * sizeof(uint8_t));
  memcpy(&buffer[0x0006], UTILS::uint16_to_bytes(file_header.reserved2),
         2 * sizeof(uint8_t));
  memcpy(&buffer[0x000A], UTILS::uint32_to_bytes(file_header.offset_data),
         4 * sizeof(uint8_t));

  // Write header info data
  memcpy(&buffer[0x000E], UTILS::uint32_to_bytes(info_header.header_size),
         4 * sizeof(uint8_t));
  memcpy(&buffer[0x0012], UTILS::uint32_to_bytes(info_header.width),
         4 * sizeof(uint8_t));
  memcpy(&buffer[0x0016], UTILS::uint32_to_bytes(info_header.height),
         4 * sizeof(uint8_t));

  memcpy(&buffer[0x001A], UTILS::uint16_to_bytes(info_header.planes),
         2 * sizeof(uint8_t));
  memcpy(&buffer[0x001C], UTILS::uint16_to_bytes(info_header.bits_per_pixel),
         2 * sizeof(uint8_t));

  memcpy(&buffer[0x001E], UTILS::uint32_to_bytes(info_header.compression),
         4 * sizeof(uint8_t));
  memcpy(&buffer[0x0022], UTILS::uint32_to_bytes(info_header.image_size),
         4 * sizeof(uint8_t));
  memcpy(&buffer[0x0026], UTILS::uint32_to_bytes(info_header.x_res),
         4 * sizeof(uint8_t));
  memcpy(&buffer[0x002A], UTILS::uint32_to_bytes(info_header.y_res),
         4 * sizeof(uint8_t));
  memcpy(&buffer[0x002E], UTILS::uint32_to_bytes(info_header.colors_used),
         4 * sizeof(uint8_t));
  memcpy(&buffer[0x0032], UTILS::uint32_to_bytes(info_header.colors_important),
         4 * sizeof(uint8_t));

  // write pixel data
  memcpy(&buffer[file_header.offset_data], &vec_pixels[0],
         vec_pixels.size() * sizeof(uint8_t));

  outfile.write((const char *)&buffer[0], buffer.size());
  if (outfile.good()) {
    std::cout << "Bitmap saved to " << filename << "\n";
    return true;
  } else {
    std::cout << "Failed to save " << filename << "\n";
    return false;
  }
}

void Bitmap::SetBitDepth(const BIT_DEPTH &bd) {
  bit_depth = bd;
  switch (bd) {
  case BIT_DEPTH::BD_24:
    info_header.bits_per_pixel = 24;
    break;
  case BIT_DEPTH::BD_32:
    info_header.bits_per_pixel = 32;
    break;
  }
}

void Bitmap::SetPixel(int x, int y, const Color &color) {
  int w = (int)info_header.width;
  int h = (int)info_header.height;
  if (x < 0 || y < 0 || x >= w || y >= h) // Pixel coordinate outside of bitmap
    return;

  switch (bit_depth) {
  case BIT_DEPTH::BD_24: {
    int padding = w % 4;
    int idx = 3 * (y * w + x) + y * padding;
    vec_pixels[idx + 0] = color.blue;
    vec_pixels[idx + 1] = color.green;
    vec_pixels[idx + 2] = color.red;
    break;
  }
  case BIT_DEPTH::BD_32: {
    int idx = 4 * (y * w + x);
    vec_pixels[idx + 0] = color.blue;
    vec_pixels[idx + 1] = color.green;
    vec_pixels[idx + 2] = color.red;
    vec_pixels[idx + 3] = color.alpha;
    break;
  }
  }
}

void Bitmap::Fill(const Color &color) {
  uint32_t h = Height();
  uint32_t w = Width();
  // x as inner loop for better performance
  for (int y = (int)h - 1; y >= 0; y--)
    for (int x = 0; x < (int)w; x++)
      SetPixel(x, y, color);
}

void BMP::Bitmap::DrawLine(int sx, int sy, int ex, int ey, Color color) {
  int dx = ex - sx;
  int dy = ey - sy;

  if (dx < 0) {
    std::swap(sx, ex);
    std::swap(sy, ey);
    dx = -dx;
    dy = -dy;
  }
  // Horizontal line
  if (dy == 0) {
    for (; sx <= ex; sx++)
      SetPixel(sx, sy, color);
  }
  // Vertical line
  else if (dx == 0) {
    if (dy < 0)
      std::swap(sy, ey);
    for (; sy <= ey; sy++)
      SetPixel(sx, sy, color);
  }
  // Slope -1 <= m <= 1
  else if (abs(dx) >= abs(dy)) {
    int y_inc = 1;
    if (dy < 0) {
      y_inc = -1;
      dy = -dy;
    }
    int D = 2 * dy - dx;
    for (; sx <= ex; sx++) {
      SetPixel(sx, sy, color);
      if (D > 0) {
        D += 2 * (dy - dx);
        sy += y_inc;
      } else {
        D += 2 * dy;
      }
    }
  }
  // Slope abs(m) > 1
  else {
    if (dy < 0) {
      std::swap(sy, ey);
      std::swap(sx, ex);
      dy = -dy;
      dx = -dx;
    }
    int x_inc = 1;
    if (dx < 0) {
      x_inc = -1;
      dx = -dx;
    }
    int D = 2 * dx - dy;
    for (; sy <= ey; sy++) {
      SetPixel(sx, sy, color);
      if (D > 0) {
        D += 2 * (dx - dy);
        sx += x_inc;
      } else {
        D += 2 * dx;
      }
    }
  }
}

void BMP::Bitmap::DrawRect(const int &x, const int &y, const int &w,
                           const int &h, const Color &color) {
  DrawLine(x, y, x + w - 1, y, color);
  DrawLine(x, y, x, y + h - 1, color);
  DrawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
  DrawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
}

void BMP::Bitmap::FillRect(const int &x, const int &y, const int &w,
                           const int &h, const Color &color) {
  for (int yi = (int)(y + h - 1); yi >= (int)y; yi--)
    for (int xi = x; xi <= (int)(x + w - 1); xi++)
      SetPixel(xi, yi, color);
}

void BMP::Bitmap::DrawCircle(const int &xc, const int &yc, const int &r,
                             const Color &color) {
  auto setpixel_all_octants = [&](const int &x, const int &y) {
    SetPixel(xc + x, yc + y, color);
    SetPixel(xc - x, yc + y, color);
    SetPixel(xc + x, yc - y, color);
    SetPixel(xc - x, yc - y, color);
    SetPixel(xc + y, yc + x, color);
    SetPixel(xc - y, yc + x, color);
    SetPixel(xc + y, yc - x, color);
    SetPixel(xc - y, yc - x, color);
  };

  int y = r;
  int x = 0;
  int D = 3 - (2 * r);
  setpixel_all_octants(x, y);
  while (y >= x++) {
    if (D > 0) {
      y--;
      D += 4 * (x - y) + 10;
    } else {
      D += 4 * x + 6;
    }
    setpixel_all_octants(x, y);
  }
}

void BMP::Bitmap::FillCircle(const int &xc, const int &yc, const int &r,
                             const Color &color) {
  auto line_drawing_helper = [&](const int &x, const int &y) {
    DrawLine(xc - x, yc + y, xc + x, yc + y, color);
    DrawLine(xc - x, yc - y, xc + x, yc - y, color);
    DrawLine(xc - y, yc + x, xc + y, yc + x, color);
    DrawLine(xc - y, yc - x, xc + y, yc - x, color);
  };

  int y = r;
  int x = 0;
  int D = 3 - (2 * r);
  line_drawing_helper(x, y);
  while (y >= x++) {
    if (D > 0) {
      y--;
      D += 4 * (x - y) + 10;
    } else {
      D += 4 * x + 6;
    }
    line_drawing_helper(x, y);
  }
}

void BMP::Bitmap::DrawTriangle(const int &x1, const int &y1, const int &x2,
                               const int &y2, const int &x3, const int &y3,
                               const Color &color) {
  DrawLine(x1, y1, x2, y2, color);
  DrawLine(x1, y1, x3, y3, color);
  DrawLine(x2, y2, x3, y3, color);
}

void BMP::Bitmap::FillTriangle(const int &x1, const int &y1, const int &x2,
                               const int &y2, const int &x3, const int &y3,
                               const Color &color) {
  FillTriangle(Vertex{x1, y1}, Vertex{x2, y2}, Vertex{x3, y3}, color);
}

void BMP::Bitmap::DrawLine(const Vertex &vStart, const Vertex &vEnd,
                           const Color &color) {
  DrawLine(vStart.x, vStart.y, vEnd.x, vEnd.y, color);
}

void BMP::Bitmap::DrawRect(const Vertex &vTopLeft, const Vertex &vSize,
                           const Color &color) {
  DrawRect(vTopLeft.x, vTopLeft.y, vSize.x, vSize.y, color);
}

void BMP::Bitmap::FillRect(const Vertex &vTopLeft, const Vertex &vSize,
                           const Color &color) {
  FillRect(vTopLeft.x, vTopLeft.y, vSize.x, vSize.y, color);
}

void BMP::Bitmap::DrawCircle(const Vertex &vCenter, const int &r,
                             const Color &color) {
  DrawCircle(vCenter.x, vCenter.y, r, color);
}

void BMP::Bitmap::FillCircle(const Vertex &vCenter, const int &r,
                             const Color &color) {
  FillCircle(vCenter.x, vCenter.y, r, color);
}

void BMP::Bitmap::DrawTriangle(const Vertex &v1, const Vertex &v2,
                               const Vertex &v3, const Color &color) {
  DrawTriangle(v1.x, v1.y, v2.x, v2.y, v3.x, v3.y, color);
}

void BMP::Bitmap::FillTriangle(Vertex v1, Vertex v2, Vertex v3,
                               const Color &color) {
  if (v2.y < v1.y)
    std::swap(v1, v2);
  if (v3.y < v1.y)
    std::swap(v1, v3);
  if (v3.y < v2.y)
    std::swap(v2, v3);

  int h = (int)info_header.height;

  auto fill_top_triangle = [&](const Vertex &v1, const Vertex &v2,
                               const Vertex &v3, bool top = true) {
    // v1.y <= v2.y <= v3.y
    int dy1 = v2.y - v1.y;
    int dy2 = v3.y - v1.y;
    int dx1 = v2.x - v1.x;
    int dx2 = v3.x - v1.x;

    bool steep1 = false;
    bool steep2 = false;
    if (abs(dx1) < dy1)
      steep1 = true;
    if (abs(dx2) < dy2)
      steep2 = true;

    int x_inc1 = 1;
    int x_inc2 = 1;
    if (dx1 < 0) {
      x_inc1 = -1;
      dx1 = -dx1;
    }
    if (dx2 < 0) {
      x_inc2 = -1;
      dx2 = -dx2;
    }
    int curr_x1 = v1.x;
    int curr_y1 = v1.y;
    int curr_x2 = v1.x;
    int curr_y2 = v1.y;

    int D1 = steep1 ? 2 * dx1 - dy1 : 2 * dy1 - dx1;
    int D2 = steep2 ? 2 * dx2 - dy2 : 2 * dy2 - dx2;
    int D1_inc1 = steep1 ? 2 * (dx1 - dy1) : 2 * (dy1 - dx1);
    int D2_inc1 = steep2 ? 2 * (dx2 - dy2) : 2 * (dy2 - dx2);
    int D1_inc2 = steep1 ? 2 * dx1 : 2 * dy1;
    int D2_inc2 = steep2 ? 2 * dx2 : 2 * dy2;
    while (curr_y1 < v2.y || curr_y2 < v2.y) {
      if (curr_y1 <= curr_y2) {
        if (D1 > 0) {
          D1 += D1_inc1;
          if (steep1)
            curr_x1 += x_inc1;
          else
            curr_y1++;
        } else {
          D1 += D1_inc2;
        }
        if (steep1)
          curr_y1++;
        else
          curr_x1 += x_inc1;
      }
      if (curr_y2 < curr_y1) {
        if (D2 > 0) {
          D2 += D2_inc1;
          if (steep2)
            curr_x2 += x_inc2;
          else
            curr_y2++;
        } else {
          D2 += D2_inc2;
        }
        if (steep2)
          curr_y2++;
        else
          curr_x2 += x_inc2;
      }
      if (curr_y1 == curr_y2) {
        if (top)
          DrawLine(curr_x1, curr_y1, curr_x2, curr_y2, color);
        else // bottom
          DrawLine(curr_x1, h - curr_y1 - 1, curr_x2, h - curr_y2 - 1, color);
      }
    }
  };

  auto fill_bottom_triangle = [&](const Vertex &v1, const Vertex &v2,
                                  const Vertex &v3) {
    fill_top_triangle(Vertex{v3.x, h - v3.y - 1}, Vertex{v2.x, h - v2.y - 1},
                      Vertex{v1.x, h - v1.y - 1}, false);
  };

  fill_top_triangle(v1, v2, v3);
  fill_bottom_triangle(v1, v2, v3);
}

Color Bitmap::GetPixelColor(const int &x, const int &y) const {
  int w = (int)info_header.width;
  int h = (int)info_header.height;
  if (x < 0 || y < 0 || x >= w || y >= h) {
    std::cout << "Error: Pixel (" << x << ", " << y << ") is out of bounds.\n";
    std::cout << "Dimensions are (width, height) = (" << info_header.width
              << ", " << info_header.height << ")\n";
    return Color{0, 0, 0};
  }

  int index = 3 * (y * info_header.width + x);
  return Color{vec_pixels[index + 2], vec_pixels[index + 1],
               vec_pixels[index + 0]};
}

void Bitmap::LoadFromByteArray(uint8_t *data, int n) {
  vec_pixels.clear();
  vec_pixels.resize(n, 0);
  for (int i = 0; i < n; i++)
    vec_pixels[i] = data[i];
}

}; // namespace BMP
