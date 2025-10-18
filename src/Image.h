#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
extern "C" {
    #include "stb_image.h"
};
#include <iostream>
#include <cmath>

#define GRAD " .-=+*x#$&X@"
#define VALUE (sizeof(GRAD) - 1)
#define THRESHOLD 0.05

class Image {
private:
    int w, h, c;
    unsigned char* raw_data;

    int to_grayscale(int i, int j);

    int get_grad_value(int kernel[3][3], int i, int j);

    void PutPixel(float mag, float dir, const unsigned char r, const unsigned char g, const unsigned char b);

    void Gradients(int** mag, float** dir, int& max_mag);

public:
    Image(const std::string& filepath);
    Image(int w, int h, int c);
    ~Image();

    int Width();
    int Height();
    int Channels();

    unsigned char& operator ()(int i, int j, int k);
    unsigned char operator ()(int i, int j, int k) const;

    void Show();
    Image Resize(int width, int height);

//    void Write() {
//        stbi_write_png("sky2.png", w, h, c, raw_data, w * c);
//    }

};

#endif