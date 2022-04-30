#include "BMP.h"

using namespace BMP;

//-------------------------------------------//
//     BEGIN BMP UTIL FUNCTION DEFINITIONS   //
//-------------------------------------------//

uint32_t UTILS::bytes_to_uint32(BYTE* data)
{
	uint32_t result = 0;
	for (int i = 0; i < 4; i++)
		result += data[i] * (1 << (8 * i));
	return result;
}

uint16_t UTILS::bytes_to_uint16(BYTE* data)
{
	uint16_t result = 0;
	for (int i = 0; i < 2; i++)
		result += data[i] * (1 << (8 * i));
	return result;
}

BYTE* UTILS::uint32_to_bytes(uint32_t data)
{
	BYTE* result = new BYTE[4];
	result[0] = (data & 0x000000FF) >> 0;
	result[1] = (data & 0x0000FF00) >> 8;
	result[2] = (data & 0x00FF0000) >> 16;
	result[3] = (data & 0xFF000000) >> 24;
	return result;
}

BYTE* UTILS::uint16_to_bytes(uint16_t data)
{
	BYTE* result = new BYTE[2];
	result[0] = (data & 0x00FF) >> 0;
	result[1] = (data & 0xFF00) >> 8;
	return result;
}

BYTE* UTILS::palette_to_bytes(vector<Color> palette)
{
	size_t n = palette.size();
	BYTE* result = new BYTE[4 * n];
	for (auto i = 0; i < 4 * n; i += 4)
	{
		result[i + 0] = palette[i].red;
		result[i + 1] = palette[i].green;
		result[i + 2] = palette[i].blue;
		result[i + 3] = 0;
	}
	return result;
}

//-------------------------------------------//
//     END BMP UTIL FUNCTION DEFINITIONS     //
//-------------------------------------------//


//---------------------------------------//
//       BEGIN CLASS BITMAP METHODS      //
//---------------------------------------//

BITMAP::BITMAP(const char* fn, const uint32_t& w, const uint32_t& h, bool alpha)
{
	filename = fn;
	info_header.width = w;
	info_header.height = h;
	int channels;
	int padding;
	if (alpha)
	{
		channels = 4;
		padding = 0;
		SetBitDepth(BIT_DEPTH::BD_32);
	}
	else
	{
		channels = 3;
		padding = w % 4;
		SetBitDepth(BIT_DEPTH::BD_24);
	}
	vec_pixels.clear();
	vec_pixels.resize((channels * w + padding) * h, 0);
	file_header.file_size = file_header.offset_data + (uint32_t)vec_pixels.size();
}

bool BITMAP::Read(const char* fn)
{
	// Open file with name fn
	basic_ifstream<BYTE> infile(fn, ios::binary);
	if (!infile.is_open())
	{
		return false; // Could not open file
	}

	// Create buffer to import data later passed onto BITMAP object
	vector<BYTE> buffer(istreambuf_iterator<BYTE>(infile), {});

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
	switch (info_header.bits_per_pixel)
	{
	case 24:
		bit_depth = BIT_DEPTH::BD_24;
		break;
	case 32:
		bit_depth = BIT_DEPTH::BD_32;
		break;
	default:
		cout << "Unsupported bit depth: " << info_header.bits_per_pixel << endl;
	}

	// Load remaining data in buffer into 
	LoadFromByteArray(&buffer[file_header.offset_data], file_header.file_size - file_header.offset_data);

	return true;
}

bool BITMAP::Write(const char* fn) const
{
	// Open/Create new file with name stored in fn
	ofstream outfile(fn, ios::binary);

	if (!outfile.is_open())
		return false; // Could not open/create file

	// Create byte buffer to store output data
	vector<BYTE> buffer(file_header.file_size);

	// Write file header data
	memcpy(&buffer[0x0000], UTILS::uint16_to_bytes(file_header.signature), 2 * sizeof(BYTE));
	memcpy(&buffer[0x0002], UTILS::uint32_to_bytes(file_header.file_size), 4 * sizeof(BYTE));
	memcpy(&buffer[0x0004], UTILS::uint16_to_bytes(file_header.reserved1), 2 * sizeof(BYTE));
	memcpy(&buffer[0x0006], UTILS::uint16_to_bytes(file_header.reserved2), 2 * sizeof(BYTE));
	memcpy(&buffer[0x000A], UTILS::uint32_to_bytes(file_header.offset_data), 4 * sizeof(BYTE));

	// Write header info data
	memcpy(&buffer[0x000E], UTILS::uint32_to_bytes(info_header.header_size), 4 * sizeof(BYTE));
	memcpy(&buffer[0x0012], UTILS::uint32_to_bytes(info_header.width), 4 * sizeof(BYTE));
	memcpy(&buffer[0x0016], UTILS::uint32_to_bytes(info_header.height), 4 * sizeof(BYTE));

	memcpy(&buffer[0x001A], UTILS::uint16_to_bytes(info_header.planes), 2 * sizeof(BYTE));
	memcpy(&buffer[0x001C], UTILS::uint16_to_bytes(info_header.bits_per_pixel), 2 * sizeof(BYTE));

	memcpy(&buffer[0x001E], UTILS::uint32_to_bytes(info_header.compression), 4 * sizeof(BYTE));
	memcpy(&buffer[0x0022], UTILS::uint32_to_bytes(info_header.image_size), 4 * sizeof(BYTE));
	memcpy(&buffer[0x0026], UTILS::uint32_to_bytes(info_header.x_res), 4 * sizeof(BYTE));
	memcpy(&buffer[0x002A], UTILS::uint32_to_bytes(info_header.y_res), 4 * sizeof(BYTE));
	memcpy(&buffer[0x002E], UTILS::uint32_to_bytes(info_header.colors_used), 4 * sizeof(BYTE));
	memcpy(&buffer[0x0032], UTILS::uint32_to_bytes(info_header.colors_important), 4 * sizeof(BYTE));

	// write pixel data
	memcpy(&buffer[file_header.offset_data], &vec_pixels[0], vec_pixels.size() * sizeof(BYTE));

	outfile.write((const char*)&buffer[0], buffer.size());
	if (outfile.good())
		return true; // Successfully wrote to file
	else
		return false; // Failed to write to file
}

void BITMAP::SetPixel(int x, int y, const Color& color)
{
	uint32_t w = info_header.width;
	uint32_t h = info_header.height;
	y = h - y - 1;
	if (x * y < 0 || x >= w || y >= h)
	{
		cout << "Error: Pixel (" << x << ", " << y << ") is out of bounds." << endl;
		cout << "Dimensions are (width, height) = (" << info_header.width << ", " << info_header.height << ")" << endl;
		return;
	}
	switch (bit_depth)
	{
	case BIT_DEPTH::BD_24:
	{
		int padding = w % 4;
		int idx = 3 * (y * w + x) + y * padding;
		vec_pixels[idx + 0] = color.blue;
		vec_pixels[idx + 1] = color.green;
		vec_pixels[idx + 2] = color.red;
		break;
	}
	case BIT_DEPTH::BD_32:
	{
		int idx = 4 * (y * w + x);
		vec_pixels[idx + 0] = color.blue;
		vec_pixels[idx + 1] = color.green;
		vec_pixels[idx + 2] = color.red;
		vec_pixels[idx + 3] = color.alpha;
		break;
	}
	}

}

void BITMAP::SetBitDepth(const BIT_DEPTH& bd)
{
	bit_depth = bd;
	switch (bd)
	{
	case BIT_DEPTH::BD_24:
		info_header.bits_per_pixel = 24;
		break;
	case BIT_DEPTH::BD_32:
		info_header.bits_per_pixel = 32;
		break;
	}
}

void BITMAP::Fill(const Color& color)
{
	uint32_t h = Height();
	uint32_t w = Width();
	// x as inner loop for better performance
	for (int y = (int)h - 1; y >= 0; y--)
		for (int x = 0; x < (int)w; x++)
			SetPixel(x, y, color);
}

void BMP::BITMAP::DrawLine(
	int sx, int sy,
	int ex, int ey,
	Color color
)
{
	int dx = ex - sx;
	int dy = ey - sy;

	if (dx < 0)
	{
		swap(sx, ex);
		swap(sy, ey);
		dx = -dx;
		dy = -dy;
	}
	// Horizontal line
	if (dy == 0)
	{
		for (; sx <= ex; sx++)
			SetPixel(sx, sy, color);
	}
	// Vertical line
	else if (dx == 0)
	{
		if (dy < 0)
			swap(sy, ey);
		for (; sy <= ey; sy++)
			SetPixel(sx, sy, color);
	}
	// Slope -1 <= m <= 1
	else if (abs(dx) >= abs(dy))
	{
		int y_inc = 1;
		if (dy < 0)
		{
			y_inc = -1;
			dy = -dy;
		}
		int D = 2 * dy - dx;
		for (; sx <= ex; sx++)
		{
			SetPixel(sx, sy, color);
			if (D > 0)
			{
				D += 2 * (dy - dx);
				sy += y_inc;
			}
			else
			{
				D += 2 * dy;
			}
		}
	}
	// Slope abs(m) > 1
	else
	{
		if (dy < 0)
		{
			swap(sy, ey);
			swap(sx, ex);
			dy = -dy;
			dx = -dx;
		}
		int x_inc = 1;
		if (dx < 0)
		{
			x_inc = -1;
			dx = -dx;
		}
		int D = 2 * dx - dy;
		for (; sy <= ey; sy++)
		{
			SetPixel(sx, sy, color);
			if (D > 0)
			{
				D += 2 * (dx - dy);
				sx += x_inc;
			}
			else
			{
				D += 2 * dx;
			}
		}

	}
}

void BMP::BITMAP::DrawRect(
	const int& x, const int& y,
	const int& w, const int& h,
	const Color& color
)
{
	DrawLine(x, y, x + w, y, color);
	DrawLine(x, y, x, y + h, color);
	DrawLine(x + w, y, x + w, y + h, color);
	DrawLine(x, y + h, x + w, y + h, color);
}

void BMP::BITMAP::FillRect(
	const int& x, const int& y,
	const int& w, const int& h,
	const Color& color
)
{
	for (int yi = (int)(y + h); yi >= (int)y; yi--)
		for (int xi = x; xi <= (int)(x + w); xi++)
			SetPixel(xi, yi, color);
}

void BMP::BITMAP::DrawCircle(
	const int& xc, const int& yc,
	const int& r,
	const Color& color
)
{
	auto setpixel_all_octants = [&](const int& x, const int& y)
	{
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
	while (y >= x++)
	{
		if (D > 0)
		{
			y--;
			D += 4 * (x - y) + 10;
		}
		else
		{
			D += 4 * x + 6;
		}
		setpixel_all_octants(x, y);
	}
}

void BMP::BITMAP::FillCircle(
	const int& xc, const int& yc,
	const int& r,
	const Color& color
)
{
	auto line_drawing_helper = [&](const int& x, const int& y)
	{
		DrawLine(xc - x, yc + y, xc + x, yc + y, color);
		DrawLine(xc - x, yc - y, xc + x, yc - y, color);
		DrawLine(xc - y, yc + x, xc + y, yc + x, color);
		DrawLine(xc - y, yc - x, xc + y, yc - x, color);
	};

	int y = r;
	int x = 0;
	int D = 3 - (2 * r);
	line_drawing_helper(x, y);
	while (y >= x++)
	{
		if (D > 0)
		{
			y--;
			D += 4 * (x - y) + 10;
		}
		else
		{
			D += 4 * x + 6;
		}
		line_drawing_helper(x, y);
	}
}


Color BITMAP::GetPixelColor(const int& x, const int& y) const
{
	if (x * y > info_header.width * info_header.height || x * y < 0)
	{
		cout << "Error: Pixel (" << x << ", " << y << ") is out of bounds." << endl;
		cout << "Dimensions are (width, height) = (" << info_header.width << ", " << info_header.height << ")" << endl;
		return Color{ 0,0,0 };
	}

	int index = 3 * (y * info_header.width + x);
	return Color{ vec_pixels[index + 2], vec_pixels[index + 1], vec_pixels[index + 0] };
}

void BITMAP::LoadFromByteArray(BYTE* data, int n)
{
	vec_pixels.clear();
	vec_pixels.resize(n, 0);
	for (int i = 0; i < n; i++)
		vec_pixels[i] = data[i];
}

//---------------------------------------//
//       END CLASS BITMAP METHODS        //
//---------------------------------------//