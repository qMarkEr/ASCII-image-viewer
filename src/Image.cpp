#define STB_IMAGE_IMPLEMENTATION
#include "Image.h"

int Image::to_grayscale(int i, int j) {
    return static_cast<int>(
        0.299f * (*this)(i, j, 0) +
        0.587f * (*this)(i, j, 1) +
        0.114f * (*this)(i, j, 2)
    );
}

int Image::get_grad_value(int (*kernel)[3], int i, int j) {
    int res = 0;
    for (int i_ = -1; i_ < 2; ++i_)
        for (int j_ = -1; j_ < 2; ++j_) {
            auto gs = to_grayscale(i + i_, j + j_);
            auto k = kernel[i_ + 1][j_ + 1];
            res += gs * k;
            }
    return res;
}

void Image::PutPixel(float mag, float dir, const unsigned char r, const unsigned char g, const unsigned char b) {
    char pixel = GRAD[static_cast<int>(VALUE * static_cast<float>(std::max(r, std::max(g, b))) / 255.0f)];

    if (mag >= THRESHOLD) {
        float splitters[8] = {
            -(7.0f / 8.0) * M_PI,
            -(5.0f / 8.0) * M_PI,
            -(3.0f / 8.0) * M_PI,
            -(1.0f / 8.0) * M_PI,
            (1.0f / 8.0) * M_PI,
            (3.0f / 8.0) * M_PI,
            (5.0f / 8.0) * M_PI,
            (7.0f / 8.0) * M_PI,
        };
        if ((std::abs(dir) > splitters[7]) || (std::abs(dir) < splitters[4]))
            pixel =  '|';
        else if ((dir >= splitters[0] && dir <= splitters[1]) ||
                (dir >= splitters[4] && dir <= splitters[5]))
            pixel = '/';
        else if ((dir >= splitters[1] && dir <= splitters[2]) ||
                 (dir >= splitters[5] && dir <= splitters[6]))
            pixel = '_';
        else if ((dir >= splitters[2] && dir <= splitters[3]) ||
                 (dir >= splitters[6] && dir <= splitters[7]))
            pixel = '\\';
    }
    printf("\x1b[38;2;%d;%d;%dm%c", r, g, b, pixel);
}

void Image::Gradients(int **mag, float **dir, int &max_mag) {
    int x_filter[3][3] = {
            {-1, 0, 1},
            {-2, 0, 2},
            {-1, 0, 1}
    };

    int y_filter[3][3] = {
            {-1, -2, -1},
            { 0,  0,  0},
            { 1,  2,  1}
    };
    for (int i = 1; i < h - 1; ++i) {
        for (int j = 1; j < w - 1; ++j) {
            int x = get_grad_value(x_filter, i, j);
            int y = get_grad_value(y_filter, i, j);
            mag[i][j] = x * x + y * y;
            if (mag[i][j] > max_mag) max_mag = mag[i][j];
            dir[i][j] = std::atan2(y, x);
        }
    }
}

Image::Image(const std::string &filepath) {
    raw_data = stbi_load(filepath.c_str(), &w, &h, &c, 0);
    if (raw_data == nullptr) throw std::runtime_error("No such file!\n");
}

Image::Image(int w, int h, int c) : w(w), h(h), c(c) { raw_data = (unsigned char*) malloc(w * h * c); }; // stb_image allocates memory in C-style so yeah...

int Image::Width() { return w; }

int Image::Height() { return h; }

int Image::Channels() { return c; }

unsigned char &Image::operator()(int i, int j, int k) {
    if (i >= h || j >= w || k >= c)
        throw std::out_of_range("wrong image indexation\n");
    return raw_data[i * w * c + j * c + k];
}

unsigned char Image::operator()(int i, int j, int k) const {
    if (i >= h || j >= w || k >= c)
        throw std::out_of_range("wrong image indexation\n");
    return raw_data[i * w * c + j * c + k];
}

void Image::Show() {
    int** mag = new int*[h];
    float** dir = new float*[h];

    for (int i = 0; i < h; ++i) {
        mag[i] = new int[w];
        dir[i] = new float[w];
        for (int j = 0; j < w; ++j) {
            mag[i][j] = 0;
            dir[i][j] = 0;
        }
    }
    int mmax = 0;
    Gradients(mag, dir, mmax);
    for (int i = 0; i < Height(); ++i) {
        for (int j = 0; j < Width(); ++j)
            if (c > 3 && (*this)(i, j, 3) == 0) std::cout << " ";
            else
                PutPixel(
                     static_cast<float>(mag[i][j]) / mmax, dir[i][j],
                     (*this)(i, j, 0),
                     (*this)(i, j, 1),
                     (*this)(i, j, 2)
                );
        std::cout << "\n";
    }
}

Image Image::Resize(int width, int height) {
    Image res = Image(width, height, this->c);
    float x_ratio = static_cast<float>(w - 1.0f) / static_cast<float>(width - 1);
    float y_ratio = static_cast<float>(h - 1.0f) / static_cast<float>(height - 1);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            float x_orig = x_ratio * j;
            float y_orig = y_ratio * i;

            int x_l = static_cast<int>(floorf(x_orig));
            int x_h = static_cast<int>(ceilf(x_orig));
            x_h = x_h >= w ? w - 1 : x_h;

            int y_l = static_cast<int>(floorf(y_orig));
            int y_h = static_cast<int>(ceilf(y_orig));
            y_h = y_h >= h ? h - 1 : y_h;

            float x_w = x_orig - x_l;
            float y_w = y_orig - y_l;

            for (int k = 0; k < c; ++k) {
                res(i, j, k) = (*this)(y_l, x_l, k) * (1 - x_w) * (1 - y_w) +
                               (*this)(y_l, x_h, k) * x_w * (1 - y_w) +
                               (*this)(y_h, x_l, k) * y_w * (1 - x_w) +
                               (*this)(y_h, x_h, k) * x_w * y_w;
            }
        }
    }

    return res;
}

Image::~Image() {
    stbi_image_free(raw_data);
}