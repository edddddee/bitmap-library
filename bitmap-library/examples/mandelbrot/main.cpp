#include "bmp.h"
#include <vector>
#include <print>
#include <limits>
#include <cmath>
#include <complex>
#include <omp.h>
#include <gmpxx.h>

int IterMandelbrot(mpf_class a, mpf_class b, int maxIter = 1000)
{
    int n = 0;
    mpf_class re_z{0};
    mpf_class im_z{0};
    while (n < maxIter && re_z * re_z + im_z * im_z > 4)
    {
        n++;
        mpf_class prev_re_z(re_z);
        re_z = re_z * re_z - im_z * im_z + a;
        im_z = 2 * prev_re_z * im_z + b;
    }
    return n;
}

struct Frame
{
    mpf_class x_center{};
    mpf_class y_center{};
    mpf_class x_width{};
    mpf_class y_height{};
    int frame_w{};
    int frame_h{};
    int max_iter{};

    Frame(mpf_class xc, mpf_class yc, mpf_class xw, mpf_class yh, int w, int h, int max_iter)
        : x_center(xc), y_center(yc), x_width(xw), y_height(yh), frame_w(w), frame_h(h), max_iter(max_iter)
    {
    }

    std::vector<int> generate()
    {
        const int n_pixels = frame_h * frame_w;
        std::vector<int> iter_data(n_pixels);

        mpf_class dx(this->x_width / mpf_class(this->frame_w));
        mpf_class dy(this->y_height / mpf_class(this->frame_h));
        mpf_class x = this->x_center - 0.5 * this->x_width;
        mpf_class y = this->y_center + 0.5 * this->y_height;

        #pragma omp parallel for
        for (int i = 0; i < n_pixels; i++)
        {
            int px = i % frame_w;
            int py = i / frame_w;
            mpf_class a(x + px * dx);
            mpf_class b(y - py * dy);
            iter_data[i] = IterMandelbrot(a, b, this->max_iter);
        }
        return iter_data;
    }
};

int main()
{
    const int WIDTH = 500 * 1;
    const int HEIGHT = 500 * 1;
    const mpf_class xc = 0.5 * (-0.750222 - 0.749191);
    const mpf_class yc = 0.5 * (0.031161 + 0.031752);
    const mpf_class xw = 0.750222 - 0.749191;
    const mpf_class yh = xw;
    const int max_iter = 1000;

    Frame frame(xc, yc, xw, yh, WIDTH, HEIGHT, max_iter);
    auto iter_data = frame.generate();
    BMP::Bitmap image("mandelbrot.bmp", WIDTH, HEIGHT, false);

    #pragma omp parallel for
    for (int pixel_idx = 0; pixel_idx < WIDTH * HEIGHT; pixel_idx++)
    {
        int x = pixel_idx % WIDTH;
        int y = HEIGHT - pixel_idx / WIDTH - 1;
        int iter = iter_data[pixel_idx];
        double strength = static_cast<double>(iter) / static_cast<double>(max_iter);
        uint8_t c = std::numeric_limits<uint8_t>::max() - static_cast<uint8_t>(std::round(static_cast<double>(std::numeric_limits<uint8_t>::max()) * strength));
        image.SetPixel(x, y, BMP::Color{c, c, c});
    }
    image.Save();
}