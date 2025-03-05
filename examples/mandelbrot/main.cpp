#include "bmp.h"
#include <vector>
#include <print>
#include <limits>
#include <cmath>
#include <complex>
#include <chrono>

#include <omp.h>

int IterMandelbrot(double a, double b, int maxIter = 1000)
{
    int n = 0;
    double re_z{0};
    double im_z{0};
    while (n < maxIter && re_z * re_z + im_z * im_z < 4)
    {
        n++;
        double prev_re_z(re_z);
        re_z = re_z * re_z - im_z * im_z + a;
        im_z = 2 * prev_re_z * im_z + b;
    }
    return n;
}

struct Frame
{
    double x_center{};
    double y_center{};
    double x_width{};
    double y_height{};
    int frame_w{};
    int frame_h{};
    int max_iter{};

    Frame(double xc, double yc, double xw, double yh, int w, int h, int max_iter)
        : x_center(xc), y_center(yc), x_width(xw), y_height(yh), frame_w(w), frame_h(h), max_iter(max_iter)
    {
    }

    std::vector<int> generate()
    {
        const int n_pixels = frame_h * frame_w;
        std::vector<int> iter_data(n_pixels);

        double dx(this->x_width / double(this->frame_w));
        double dy(this->y_height / double(this->frame_h));
        double x = this->x_center - 0.5 * this->x_width;
        double y = this->y_center + 0.5 * this->y_height;

        #pragma omp parallel for
        for (int i = 0; i < n_pixels; i++)
        {
            int px = i % frame_w;
            int py = i / frame_w;
            double a(x + px * dx);
            double b(y - py * dy);
            iter_data[i] = IterMandelbrot(a, b, this->max_iter);
        }
        return iter_data;
    }
};

int main()
{
    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;

    auto t0 = high_resolution_clock::now();

    const int WIDTH = 1<<12;
    const int HEIGHT = 1<<12;
    const double xc = -0.5;
    const double yc = 0;
    const double xw = 3;
    const double yh = 3;
    const int max_iter = 64;

    Frame frame(xc, yc, xw, yh, WIDTH, HEIGHT, max_iter);
    auto iter_data = frame.generate();

    auto t1 = high_resolution_clock::now();
    auto ms_taken = duration_cast<milliseconds>(t1 - t0);

    std::println("Performed in {}", ms_taken);

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