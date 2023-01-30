#pragma once
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
#include <cmath>



typedef uint8_t BYTE;

namespace BMP
{
	struct Vertex
	{
		int x;
		int y;
	};

	struct Color
	{
		uint8_t red{ 0 };
		uint8_t green{ 0 };
		uint8_t blue{ 0 };
		uint8_t alpha{ 255 };
	};

	struct Pixel
	{
		uint32_t x;
		uint32_t y;
		Color color;
	};

	struct FileHeader
	{
		uint16_t signature{ 0x4D42 }; // 2 bytes, "BM" in ascii
		uint32_t file_size;
		uint16_t reserved1{ 0 };
		uint16_t reserved2{ 0 };
		uint32_t offset_data{ 54 };

		void Print() // For debugging puposes
		{
			int w = 12;
			std::cout.width(w);
			std::cout << std::left << "Signature: " << std::right << signature << "\n";
			std::cout.width(w);
			std::cout << std::left << "File size: " << std::right << file_size << " Bytes\n";
			std::cout.width(w);
			std::cout << std::left << "Reserved 1: " << std::right << reserved1 << "\n";
			std::cout.width(w);
			std::cout << std::left << "Reserved 2: " << std::right << reserved2 << "\n";
			std::cout.width(w);
			std::cout << std::left << "Offset: " << std::right << offset_data << "\n\n";
		}
	};

	struct Infoheader
	{
		uint32_t header_size{ 40 };
		uint32_t width;
		uint32_t height;

		uint16_t planes{ 1 };
		uint16_t bits_per_pixel;
		uint32_t compression{ 0 };
		uint32_t image_size{ 0 };
		uint32_t x_res{ 0 };
		uint32_t y_res{ 0 };
		uint32_t colors_used{ 0 };
		uint32_t colors_important{ 0 };
	};

	namespace UTILS
	{
		uint32_t bytes_to_uint32(BYTE* data);

		uint16_t bytes_to_uint16(BYTE* data);

		BYTE* uint32_to_bytes(uint32_t data);

		BYTE* uint16_to_bytes(uint16_t data);
	}

	enum class BIT_DEPTH { BD_24, BD_32 };

	class BITMAP
	{
	public: // change to protected later
		FileHeader file_header;
		Infoheader info_header;
		std::vector<BYTE> vec_pixels;
		const char* filename;
		BIT_DEPTH bit_depth;

	public:
		BITMAP(const char* fn = "img.bmp")
		{
			filename = fn;
			bit_depth = BIT_DEPTH::BD_24;
			file_header = FileHeader{};
			info_header = Infoheader{};
		}
		BITMAP(
			const char* fn,
			const uint32_t& w,
			const uint32_t& h,
			bool alpha = true
		);

	public:
		bool Read(const char* fn);
		bool Write(const char* fn) const;
		bool Save() const { return Write(filename); }

	public:
		// Setters
		void SetPixel(int x, int y, const Color& color);
		void SetBitDepth(const BIT_DEPTH& bd);
		void SetFileName(const char* fn) { filename = fn; }


	public:
		// Drawing routines
		void Fill(const Color& color);
		// Implements Bresenham's line algorithm (https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm)
		void DrawLine(
			int sx, int sy,
			int ex, int ey,
			Color color);
		void DrawRect(
			const int& x, const int& y,
			const int& w, const int& h,
			const Color& color);
		void FillRect(
			const int& x, const int& y,
			const int& w, const int& h,
			const Color& color);
		// Implements Bresenham's circle algorithm (https://iq.opengenus.org/bresenhams-circle-drawing-algorithm/)
		void DrawCircle(
			const int& xc, const int& yc,
			const int& r,
			const Color& color);
		void FillCircle(
			const int& xc, const int& yc,
			const int& r,
			const Color& color);
		void DrawTriangle(
			const int& x1, const int& y1,
			const int& x2, const int& y2,
			const int& x3, const int& y3,
			const Color& color
		);
		// Implements Bresenham's triangle rasterization algorithm (http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html#algo2)
		void FillTriangle(
			const int& x1, const int& y1,
			const int& x2, const int& y2,
			const int& x3, const int& y3,
			const Color& color
		);

		void DrawLine(const Vertex& vStart, const Vertex& vEnd, const Color& color);
		void DrawRect(const Vertex& vTopLeft, const Vertex& vSize, const Color& color);
		void FillRect(const Vertex& vTopLeft, const Vertex& vSize, const Color& color);
		void DrawCircle(const Vertex& vCenter, const int& r, const Color& color);
		void FillCircle(const Vertex& vCenter, const int& r, const Color& color);
		void DrawTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Color& color);
		void FillTriangle(Vertex v1, Vertex v2, Vertex v3, const Color& color);


	public:
		// Getters
		Color GetPixelColor(const int& x, const int& y) const;
		uint32_t Width() const { return info_header.width; }
		uint32_t Height() const { return info_header.height; }
		uint32_t GetFileSize() const { return file_header.file_size; }
		BIT_DEPTH GetBitDepth() const { return bit_depth; }

	public:
		void LoadFromByteArray(BYTE* data, int n);

	};
};

