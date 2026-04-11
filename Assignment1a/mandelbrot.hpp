#include <vector>
#include <thread>
#include <atomic>

#include "pixel.hpp"
#include "image.hpp"

class Mandelbrot {
    Image image;
    int max_iterations = 2048;
    std::atomic<int> next_row{0};

public:
    Mandelbrot(int rows, int cols, int max_iterations)
        : image(rows, cols, {0, 0, 0}), max_iterations(max_iterations) { }

    void compute(int num_threads = 1) {
        next_row = 0;

        if (num_threads <= 1) {
            worker(1, 0);
            return;
        }

        std::vector<std::thread> threads;
        threads.reserve(num_threads);

        for (int tid = 0; tid < num_threads; ++tid) {
            threads.emplace_back(&Mandelbrot::worker, this, num_threads, tid);
        }

        for (auto &t : threads) {
            t.join();
        }
    }

    void worker(int num_threads, int thread_id = 0) {
        unsigned char color = (254 * (thread_id + 1)) / num_threads % 254;
        constexpr int CHUNK = 8;

        const double xscale = 2.0 / image.width;
        const double yscale = 2.0 / image.height;

        while (true) {
            int start = next_row.fetch_add(CHUNK, std::memory_order_relaxed);
            if (start >= image.height) break;

            int end = start + CHUNK;
            if (end > image.height) end = image.height;

            for (int y = start; y < end; ++y) {
                double cy = (static_cast<double>(y) / image.height - 0.5) * 2.0;

                for (int x = 0; x < image.width; ++x) {
                    double cx = (static_cast<double>(x) / image.width - 0.75) * 2.0;

                    if (check_pixel(cx, cy)) {
                        image[y][x] = {color, color, color};
                    } else {
                        image[y][x] = {255, 255, 255};
                    }
                }
            }
        }
    }

    bool check_pixel(double cx, double cy) {
        double zr = 0.0;
        double zi = 0.0;

        for (int i = 0; i < max_iterations; ++i) {
            double zr2 = zr * zr;
            double zi2 = zi * zi;

            if (zr2 + zi2 > 16.0) return false;

            zi = 2.0 * zr * zi + cy;
            zr = zr2 - zi2 + cx;
        }
        return true;
    }

    void save_to_ppm(std::string filename) {
        image.save_to_ppm(filename);
    }
};