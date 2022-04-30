#pragma once
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
#include <stdint.h>
#include <cmath>

using namespace std;

typedef uint8_t BYTE;

namespace BMP
{
	struct Color
	{
		uint8_t red{ 0 };
		uint8_t green{ 0 };
		uint8_t blue{ 0 };
		uint8_t alpha{ 0 };
	};

	struct Pixel
	{
		uint32_t x;
		uint32_t y;
		Color color;
	};

	struct FileHeader
	{
		uint16_t signature{ 0x4D42 }; // 2 bytes, should be "BM" when cast to ascii
		uint32_t file_size;
		uint16_t reserved1{ 0 };
		uint16_t reserved2{ 0 };
		uint32_t offset_data{ 54 };

		void Print()
		{
			int w = 12;
			cout.width(w);
			cout << left << "Signature: " << right << signature << endl;
			cout.width(w);
			cout << left << "File size: " << right << file_size << " Bytes" << endl;
			cout.width(w);
			cout << left << "Reserved 1: " << right << reserved1 << endl;
			cout.width(w);
			cout << left << "Reserved 2: " << right << reserved2 << endl;
			cout.width(w);
			cout << left << "Offset: " << right << offset_data << endl << endl;
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

		BYTE* palette_to_bytes(vector<Color> palette);
	}

	enum class BIT_DEPTH { BD_24, BD_32 };

	class BITMAP
	{
	public: // change to protected later
		FileHeader file_header;
		Infoheader info_header;
		vector<BYTE> pixel_data;
		const char* filename;
		BIT_DEPTH bit_depth;

	public:
		BITMAP(const char* fn) : filename(fn) { Read(fn); }
		BITMAP() : filename("my_bitmap.bmp"), bit_depth(BIT_DEPTH::BD_24) { file_header = FileHeader{}, info_header = Infoheader{}; }
		BITMAP(const char* fn, const uint32_t& w, const uint32_t& h, bool alpha = true);

	public:
		void Read(const char* fn);
		void Write(const char* fn);
		void Save() { Write(filename); }

	public:
		// Setters
		void SetPixel(uint32_t x, uint32_t y, Color color);
		void SetBitDepth(BIT_DEPTH);
		void SetFileName(const char* fn) { filename = fn; }


	public:
		// Drawing routines
		void Fill(Color color);
		void DrawLine(int sx, int sy, int ex, int ey, Color color);
		void DrawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, Color color);
		void FillRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, Color color);

	public:
		// Getters
		Color GetPixelColor(uint32_t x, uint32_t y);
		uint32_t Width() { return info_header.width; }
		uint32_t Height() { return info_header.height; }
		uint32_t GetFileSize() { return file_header.file_size; }
		BIT_DEPTH GetBitDepth() { return bit_depth; }

	public:
		void LoadFromByteArray(BYTE* data, int n);

	};
};

