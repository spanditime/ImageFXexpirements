#include <iostream>
#include "./SFML/Graphics/Image.hpp"
#include <exception>
#include <cmath>
#include <random>
#include <cstdlib>
#include <chrono>

using namespace sf;

int map(int min, int max, int nmin, int nmax, int value)
{
    return (value - min) * (nmax - nmin) / (double)(max - min) + nmin;
}

double Linter(double min, double max, double v)
{
    return min + (max - min) * v;
}

double Bilinter(double ul, double ur, double dl, double dr, double x_v, double y_v)
{
    return Linter(Linter(ul, ur, x_v), Linter(dl, dr, x_v), y_v);
}

Color ColorBilinter(Color ul, Color ur, Color dl, Color dr, double x_v, double y_v)
{
    Color result = Color();
    result.r = Bilinter(ul.r, ur.r, dl.r, dr.r, x_v, y_v);
    result.g = Bilinter(ul.g, ur.g, dl.g, dr.g, x_v, y_v);
    result.b = Bilinter(ul.b, ur.b, dl.b, dr.b, x_v, y_v);
    return result;
}

double CubicInterpolation(double p[4], double x)
{
    return p[1] + 0.5 * x * (p[2] - p[0] + x * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] - p[3] + x * (3.0 * (p[1] - p[2]) + p[3] - p[0])));
}

// double BicubicInterpolate(double p[4][4], double x_v, double y_v) {
//     double arr[4];
//     arr[0] = CubicInterpolation(p[0], y_v);
//     arr[1] = CubicInterpolation(p[1], y_v);
//     arr[2] = CubicInterpolation(p[2], y_v);
//     arr[3] = CubicInterpolation(p[3], y_v);
//     return CubicInterpolation(arr, x_v);
// }

double BicubicInterpolate(double p[4][4], double x_v, double y_v)
{
    double a00 = p[1][1];
    double a01 = -.5 * p[1][0] + .5 * p[1][2];
    double a02 = p[1][0] - 2.5 * p[1][1] + 2 * p[1][2] - .5 * p[1][3];
    double a03 = -.5 * p[1][0] + 1.5 * p[1][1] - 1.5 * p[1][2] + .5 * p[1][3];
    double a10 = -.5 * p[0][1] + .5 * p[2][1];
    double a11 = .25 * p[0][0] - .25 * p[0][2] - .25 * p[2][0] + .25 * p[2][2];
    double a12 = -.5 * p[0][0] + 1.25 * p[0][1] - p[0][2] + .25 * p[0][3] + .5 * p[2][0] - 1.25 * p[2][1] + p[2][2] - .25 * p[2][3];
    double a13 = .25 * p[0][0] - .75 * p[0][1] + .75 * p[0][2] - .25 * p[0][3] - .25 * p[2][0] + .75 * p[2][1] - .75 * p[2][2] + .25 * p[2][3];
    double a20 = p[0][1] - 2.5 * p[1][1] + 2 * p[2][1] - .5 * p[3][1];
    double a21 = -.5 * p[0][0] + .5 * p[0][2] + 1.25 * p[1][0] - 1.25 * p[1][2] - p[2][0] + p[2][2] + .25 * p[3][0] - .25 * p[3][2];
    double a22 = p[0][0] - 2.5 * p[0][1] + 2 * p[0][2] - .5 * p[0][3] - 2.5 * p[1][0] + 6.25 * p[1][1] - 5 * p[1][2] + 1.25 * p[1][3] + 2 * p[2][0] - 5 * p[2][1] + 4 * p[2][2] - p[2][3] - .5 * p[3][0] + 1.25 * p[3][1] - p[3][2] + .25 * p[3][3];
    double a23 = -.5 * p[0][0] + 1.5 * p[0][1] - 1.5 * p[0][2] + .5 * p[0][3] + 1.25 * p[1][0] - 3.75 * p[1][1] + 3.75 * p[1][2] - 1.25 * p[1][3] - p[2][0] + 3 * p[2][1] - 3 * p[2][2] + p[2][3] + .25 * p[3][0] - .75 * p[3][1] + .75 * p[3][2] - .25 * p[3][3];
    double a30 = -.5 * p[0][1] + 1.5 * p[1][1] - 1.5 * p[2][1] + .5 * p[3][1];
    double a31 = .25 * p[0][0] - .25 * p[0][2] - .75 * p[1][0] + .75 * p[1][2] + .75 * p[2][0] - .75 * p[2][2] - .25 * p[3][0] + .25 * p[3][2];
    double a32 = -.5 * p[0][0] + 1.25 * p[0][1] - p[0][2] + .25 * p[0][3] + 1.5 * p[1][0] - 3.75 * p[1][1] + 3 * p[1][2] - .75 * p[1][3] - 1.5 * p[2][0] + 3.75 * p[2][1] - 3 * p[2][2] + .75 * p[2][3] + .5 * p[3][0] - 1.25 * p[3][1] + p[3][2] - .25 * p[3][3];
    double a33 = .25 * p[0][0] - .75 * p[0][1] + .75 * p[0][2] - .25 * p[0][3] - .75 * p[1][0] + 2.25 * p[1][1] - 2.25 * p[1][2] + .75 * p[1][3] + .75 * p[2][0] - 2.25 * p[2][1] + 2.25 * p[2][2] - .75 * p[2][3] - .25 * p[3][0] + .75 * p[3][1] - .75 * p[3][2] + .25 * p[3][3];

    double x2 = x_v * x_v;
    double x3 = x2 * x_v;
    double y2 = y_v * y_v;
    double y3 = y2 * y_v;

    return (a00 + a01 * y_v + a02 * y2 + a03 * y3) +
           (a10 + a11 * y_v + a12 * y2 + a13 * y3) * x_v +
           (a20 + a21 * y_v + a22 * y2 + a23 * y3) * x2 +
           (a30 + a31 * y_v + a32 * y2 + a33 * y3) * x3;
}

Color ColorBicubicInterpolate(Color c[4][4], double x_v, double y_v)
{
    double r[4][4];
    double g[4][4];
    double b[4][4];
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            r[x][y] = c[x][y].r;
            g[x][y] = c[x][y].g;
            b[x][y] = c[x][y].b;
        }
    }
    Color result = Color();
    result.r = BicubicInterpolate(r, x_v, y_v);
    result.g = BicubicInterpolate(g, x_v, y_v);
    result.b = BicubicInterpolate(b, x_v, y_v);

    return result;
}

typedef void (*effectDelegate)(size_t x, size_t y, Image &inBuff, Image &outBuff);

void PseudoToner(size_t x, size_t y, Image &inBuff, Image &outBuff)
{
    size_t width = inBuff.getSize().x, height = inBuff.getSize().y;
    Color clr = inBuff.getPixel(x, y);
    Color nclr = Color();
    if (clr.r >= 128)
        nclr.r = 255;
    else
        nclr.r = 0;
    if (clr.g >= 128)
        nclr.g = 255;
    else
        nclr.g = 0;
    if (clr.b >= 128)
        nclr.b = 255;
    else
        nclr.b = 0;

    outBuff.setPixel(x, y, nclr);

    char rd = -nclr.r + clr.r;
    char gd = -nclr.g + clr.g;
    char bd = -nclr.b + clr.b;

    if (x + 1 < width)
    {
        nclr = inBuff.getPixel(x + 1, y);
        nclr.r += rd / 16 * 7;
        nclr.g += gd / 16 * 7;
        nclr.b += bd / 16 * 7;
        inBuff.setPixel(x + 1, y, nclr);
    }
    if (x + 1 < width && y + 1 < height)
    {
        nclr = inBuff.getPixel(x + 1, y + 1);
        nclr.r += rd / 16;
        nclr.g += gd / 16;
        nclr.b += bd / 16;
        inBuff.setPixel(x + 1, y + 1, nclr);
    }
    if (y + 1 < height)
    {
        nclr = inBuff.getPixel(x, y + 1);
        nclr.r += rd / 16 * 5;
        nclr.g += gd / 16 * 5;
        nclr.b += bd / 16 * 5;
        inBuff.setPixel(x, y + 1, nclr);
    }
    if (x != 0 && y + 1 < height)
    {
        nclr = inBuff.getPixel(x - 1, y + 1);
        nclr.r += rd / 16 * 3;
        nclr.g += gd / 16 * 3;
        nclr.b += bd / 16 * 3;
        inBuff.setPixel(x - 1, y + 1, nclr);
    }
}

sf::Uint8 mask[16][4][4]{
    {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {255, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 0, 255, 0}, {0, 0, 0, 0}, {255, 0, 0, 0}},
    {{0, 0, 0, 0}, {0, 0, 255, 0}, {0, 0, 0, 0}, {255, 0, 255, 0}},
    {{0, 0, 0, 0}, {255, 0, 255, 0}, {0, 0, 0, 0}, {255, 0, 255, 0}},
    {{0, 0, 0, 0}, {255, 0, 255, 0}, {0, 255, 0, 0}, {255, 0, 255, 0}},
    {{0, 0, 0, 255}, {255, 0, 255, 0}, {0, 255, 0, 0}, {255, 0, 255, 0}},
    {{0, 0, 0, 255}, {255, 0, 255, 0}, {0, 255, 0, 255}, {255, 0, 255, 0}},
    {{0, 255, 0, 255}, {255, 0, 255, 0}, {0, 255, 0, 255}, {255, 0, 255, 0}},
    {{0, 255, 0, 255}, {255, 0, 255, 0}, {0, 255, 0, 255}, {255, 255, 255, 0}},
    {{0, 255, 0, 255}, {255, 0, 255, 255}, {0, 255, 0, 255}, {255, 255, 255, 0}},
    {{0, 255, 0, 255}, {255, 0, 255, 255}, {0, 255, 0, 255}, {255, 255, 255, 255}},
    {{0, 255, 0, 255}, {255, 255, 255, 255}, {0, 255, 0, 255}, {255, 255, 255, 255}},
    {{0, 255, 0, 255}, {255, 255, 255, 255}, {255, 255, 0, 255}, {255, 255, 255, 255}},
    {{0, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 0, 255}, {255, 255, 255, 255}},
    {{0, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255}, {255, 255, 255, 255}}};

void NormalizedPseudoToner(size_t x, size_t y, Image &inBuff, Image &outBuff)
{
    size_t width = inBuff.getSize().x, height = inBuff.getSize().y;
    size_t x0 = (x / 4) * 4, y0 = (y / 4) * 4;
    int count = 0, r = 0, g = 0, b = 0;
    for (int xo = 0; xo < 4; xo++)
    {
        for (int yo = 0; yo < 4; yo++)
        {
            int yn = y0 + yo, xn = x0 + xo;
            if (xn < width && yn < height)
            {
                sf::Color clr = inBuff.getPixel(xn, yn);
                r += clr.r;
                g += clr.g;
                b += clr.b;
                count++;
            }
        }
    }
    Color nclr;
    nclr.r = mask[(r / count) / 16][x - x0][y - y0];
    nclr.g = mask[(g / count) / 16][x - x0][y - y0];
    nclr.b = mask[(b / count) / 16][x - x0][y - y0];
    outBuff.setPixel(x, y, nclr);
}

void GrayScale(size_t x, size_t y, Image &inBuff, Image &outBuff)
{
    Color clr = inBuff.getPixel(x, y);
    unsigned int max = std::max(std::max(clr.r, clr.g), clr.b);
    unsigned int min = std::min(std::min(clr.r, clr.g), clr.b);
    max = (max + min) / 2;
    clr.r = max;
    clr.g = max;
    clr.b = max;
    outBuff.setPixel(x, y, clr);
}

void BlackAndWhite(size_t x, size_t y, Image &inBuff, Image &outBuff)
{
    Color clr = inBuff.getPixel(x, y);
    unsigned int max = std::max(std::max(clr.r, clr.g), clr.b);
    unsigned int min = std::min(std::min(clr.r, clr.g), clr.b);
    max = ((max + min) / 2) / 128 * 255;
    clr.r = max;
    clr.g = max;
    clr.b = max;
    outBuff.setPixel(x, y, clr);
}

void BlackAndWhiteWithNoise(size_t x, size_t y, Image &inBuff, Image &outBuff)
{
    Color clr = inBuff.getPixel(x, y);
    unsigned int max = std::max(std::max(clr.r, clr.g), clr.b);
    unsigned int min = std::min(std::min(clr.r, clr.g), clr.b);
    max = ((max + min) / 2) / 128 * 255;
    unsigned int r = (rand() % 2) * 255;
    if (max == 0)
        max += r;

    clr.r = max;
    clr.g = max;
    clr.b = max;
    outBuff.setPixel(x, y, clr);
}

void sNoise(size_t x, size_t y, Image &inBuff, Image &outBuff)
{
    Color clr = inBuff.getPixel(x, y);
    int r = rand() % 256;

    if (r < clr.r)
    {
        clr.r = 255;
    }
    else
    {
        clr.r = 0;
    }

    if (r < clr.g)
    {
        clr.g = 255;
    }
    else
    {
        clr.g = 0;
    }

    if (r < clr.b)
    {
        clr.b = 255;
    }
    else
    {
        clr.b = 0;
    }

    outBuff.setPixel(x, y, clr);
}

void NearestScaling(size_t x, size_t y, Image &inBuff, Image &outBuff)
{
    size_t ratio = outBuff.getSize().x / inBuff.getSize().x + 1;

    size_t x_ = x / ratio;
    size_t y_ = y / ratio;

    if (x >= ratio)
        x_ = (x - 1) / ratio;
    if (y >= ratio)
        y_ = (y - 1) / ratio;

    Color result;

    double x_v = ((double)(x - x_ * ratio)) / ratio;
    double y_v = ((double)(y - y_ * ratio)) / ratio;

    if (x_v <= 0.5 && y_v <= 0.5)
        result = inBuff.getPixel(x_, y_);
    else if (x_v <= 0.5)
        result = inBuff.getPixel(x_, y_ + 1);
    else if (y_v <= 0.5)
        result = inBuff.getPixel(x_ + 1, y_);
    else
        result = inBuff.getPixel(x_ + 1, y_ + 1);

    outBuff.setPixel(x, y, result);
}

void BilinearScaling(size_t x, size_t y, Image &inBuff, Image &outBuff)
{
    size_t ratio = outBuff.getSize().x / inBuff.getSize().x + 1;

    size_t x_ = x / ratio;
    size_t y_ = y / ratio;

    if (x >= ratio)
        x_ = (x - 1) / ratio;
    if (y >= ratio)
        y_ = (y - 1) / ratio;

    Color ul = inBuff.getPixel(x_, y_),
          ur = inBuff.getPixel(x_ + 1, y_),
          dl = inBuff.getPixel(x_, y_ + 1),
          dr = inBuff.getPixel(x_ + 1, y_ + 1);

    double x_v = ((double)(x - x_ * ratio)) / ratio;
    double y_v = ((double)(y - y_ * ratio)) / ratio;

    Color result = ColorBilinter(ul, ur, dl, dr, x_v, y_v);

    outBuff.setPixel(x, y, result);
}

void BicubicScaling(size_t x, size_t y, Image &inBuff, Image &outBuff)
{
    size_t ratio = outBuff.getSize().x / inBuff.getSize().x + 1;

    size_t x_ = (x - 1) / ratio - 2;
    size_t y_ = (y - 1) / ratio - 2;

    if (x <= 3 * ratio + 1)
        x_ = 0;
    if (y <= 3 * ratio + 1)
        y_ = 0;
    // std::cout<<x<<","<<y<<":"<<x_<<","<<y_<<"\n";

    Color c[4][4];
    for (int xo = 0; xo < 4; xo++)
    {
        for (int yo = 0; yo < 4; yo++)
        {
            c[xo][yo] = inBuff.getPixel(x_ + xo, y_ + yo);
        }
    }

    double x_v = (x - x_ * ratio) / (3. * ratio);
    double y_v = (y - y_ * ratio) / (3. * ratio);

    Color result = ColorBicubicInterpolate(c, x_v, y_v);

    outBuff.setPixel(x, y, result);
}

template <int a00, int a01, int a02, int a10, int a11, int a12, int a20, int a21, int a22, bool UseDelimeter>
void ApplyMatrixFilter(size_t x, size_t y, Image &inBuff, Image &outBuff)
{
    int delimeter = a11;
    size_t width = outBuff.getSize().x, height = outBuff.getSize().y;
    if (UseDelimeter)
    {
        delimeter += (a00) * int(x != 0 && y != 0);
        delimeter += (a01) * int(y != 0);
        delimeter += (a02) * int(y != 0 && x != width - 1);

        delimeter += (a10) * int(x != 0);
        // a11 - center
        delimeter += (a12) * int(x != width - 1);

        delimeter += (a20) * int(x != 0 && y != height - 1);
        delimeter += (a21) * int(y != height - 1);
        delimeter += (a22) * int(x != width - 1 && y != height - 1);
    }
    int r = 0, g = 0, b = 0;

    if (x != 0 && y != 0)
    {
        sf::Color clr = inBuff.getPixel(x - 1, y - 1);
        r += a00 * clr.r;
        g += a00 * clr.g;
        b += a00 * clr.b;
    }
    if (y != 0)
    {
        sf::Color clr = inBuff.getPixel(x, y - 1);
        r += a01 * clr.r;
        g += a01 * clr.g;
        b += a01 * clr.b;
    }
    if (y != 0 && x != width - 1)
    {
        sf::Color clr = inBuff.getPixel(x + 1, y - 1);
        r += a02 * clr.r;
        g += a02 * clr.g;
        b += a02 * clr.b;
    }
    if (x != 0)
    {
        sf::Color clr = inBuff.getPixel(x - 1, y);
        r += a10 * clr.r;
        g += a10 * clr.g;
        b += a10 * clr.b;
    }
    { // central pixel
        sf::Color clr = inBuff.getPixel(x, y);
        r += a11 * clr.r;
        g += a11 * clr.g;
        b += a11 * clr.b;
    }
    if (x != width - 1)
    {
        sf::Color clr = inBuff.getPixel(x + 1, y);
        r += a12 * clr.r;
        g += a12 * clr.g;
        b += a12 * clr.b;
    }
    if (x != 0 && y != height - 1)
    {
        sf::Color clr = inBuff.getPixel(x - 1, y + 1);
        r += a20 * clr.r;
        g += a20 * clr.g;
        b += a20 * clr.b;
    }
    if (y != height - 1)
    {
        sf::Color clr = inBuff.getPixel(x, y + 1);
        r += a21 * clr.r;
        g += a21 * clr.g;
        b += a21 * clr.b;
    }
    if (x != width - 1 && y != height - 1)
    {
        sf::Color clr = inBuff.getPixel(x + 1, y + 1);
        r += a22 * clr.r;
        g += a22 * clr.g;
        b += a22 * clr.b;
    }

    if (UseDelimeter)
    {
        if (delimeter == 0)
            delimeter = 1;
        r /= delimeter;
        g /= delimeter;
        b /= delimeter;
    }

    if (r < 0)
        r = 0;
    if (g < 0)
        g = 0;
    if (b < 0)
        b = 0;
    if (r > 255)
        r = 255;
    if (g > 255)
        g = 255;
    if (b > 255)
        b = 255;

    outBuff.setPixel(x, y, sf::Color(r, g, b));
}

void EmbossingSecondStep(size_t x, size_t y, Image &inBuff, Image &outBuff)
{
    sf::Color clr = outBuff.getPixel(x, y);
    int L = map(0, 255, 30, 255, clr.r);
    // int L = 60 + clr.r;
    if (L > 255)
        L = 255;
    outBuff.setPixel(x, y, sf::Color(L, L, L));
}

void Shrinking(size_t x, size_t y, Image &inBuff, Image &outBuff)
{
    sf::Color clr = inBuff.getPixel(x, y);
    size_t width = inBuff.getSize().x, height = inBuff.getSize().y;
    bool cond = true;
    for (int xo = -1; xo <= 1; xo++)
    {
        for (int yo = -1; yo <= 1; yo++)
        {
            int xn = x + xo, yn = y + yo;
            if (xn >= 0 && xn < width && yn >= 0 && yn < height)
            {
                Color clro = inBuff.getPixel(xn, yn);
                cond = cond && !clro.r;
            }
        }
    }
    if (!cond)
    {
        clr.r = 255;
        clr.g = 255;
        clr.b = 255;
    }
    outBuff.setPixel(x, y, clr);
}

void Expansion(size_t x, size_t y, Image &inBuff, Image &outBuff)
{
    sf::Color clr = inBuff.getPixel(x, y);
    size_t width = inBuff.getSize().x, height = inBuff.getSize().y;
    bool cond = true;
    for (int xo = -1; xo <= 1; xo++)
    {
        for (int yo = -1; yo <= 1; yo++)
        {
            int xn = x + xo, yn = y + yo;
            if (xn >= 0 && xn < width && yn >= 0 && yn < height)
            {
                Color clro = inBuff.getPixel(xn, yn);
                cond = cond && clro.r;
            }
        }
    }
    if (!cond)
    {
        clr.r = 0;
        clr.g = 0;
        clr.b = 0;
    }
    outBuff.setPixel(x, y, clr);
}

effectDelegate Blur = ApplyMatrixFilter<1, 2, 1, 2, 4, 2, 1, 2, 1, true>;
effectDelegate Sharpness = ApplyMatrixFilter<-1, -2, -1, -2, 22, -2, -1, -2, -1, true>;
effectDelegate Edges = ApplyMatrixFilter<0, -1, 0, -1, 4, -1, 0, -1, 0, false>;
// effectDelegate Embossing = ApplyMatrixFilter<0,1,0,1,0,-1,0,-1,0,false>;
effectDelegate Embossing = ApplyMatrixFilter<0, -3, 0, -3, 0, 3, 0, 3, 0, false>;
effectDelegate CUSTOMMEGAFILTER = ApplyMatrixFilter<-1, 1, -1, 1, 0, 1, -1, 1, -1, true>;

void applyShaderASync(Image image, Image &outBuff, effectDelegate effect)
{
    size_t width = outBuff.getSize().x;
    size_t height = outBuff.getSize().y;
#pragma omp parallel num_threads(4)
    {
#pragma omp parallel for
        for (int i = 0; i < height * width; i++)
        {
            size_t x = i % width;
            size_t y = i / height;
            effect(x, y, image, outBuff);
            // #pragma omp ordered
            // {
            // if(i>2540000)
            // std::cout<<i<<"\n";
            // }
        }
    }
}

void applyShaderSync(Image image, Image &outBuff, effectDelegate effect)
{
    size_t width = outBuff.getSize().x;
    size_t height = outBuff.getSize().y;

    for (int i = 0; i < height * width; i++)
    {
        size_t x = i % width;
        size_t y = i / height;
        effect(x, y, image, outBuff);
    }
}

int main()
{
    unsigned int ratio;
    sf::Image image = Image();
    std::cout << image.loadFromFile("./img/file.png") << "\n";

    Image resultImage = Image(image);
    applyShaderSync(image, resultImage, PseudoToner);
    resultImage.saveToFile("./img/2d_pseudoToner.png");
    applyShaderASync(resultImage, resultImage, Blur);
    resultImage.saveToFile("./img/pseudoToner+Blur.png");

    applyShaderSync(image, resultImage, NormalizedPseudoToner);
    resultImage.saveToFile("./img/2c_NormalizedPseudoToner.png");

    applyShaderASync(image, resultImage, GrayScale);
    resultImage.saveToFile("./img/GrayScale.png");
    applyShaderSync(resultImage, resultImage, NormalizedPseudoToner);
    resultImage.saveToFile("./img/2c_GrayScaleNormalizedPseudoToner.png");

    applyShaderASync(image, resultImage, GrayScale);
    applyShaderSync(resultImage, resultImage, PseudoToner);
    resultImage.saveToFile("./img/2d_GrayScalePseudoToner.png");

    applyShaderASync(image, resultImage, GrayScale);
    applyShaderASync(resultImage, resultImage, sNoise);
    resultImage.saveToFile("./img/2b_GrayScaleNoise.png");

    applyShaderASync(image, resultImage, sNoise);
    resultImage.saveToFile("./img/2b_ColoredNoise.png");

    applyShaderASync(image, resultImage, BlackAndWhite);
    resultImage.saveToFile("./img/2a_BlackAndWhite.png");

    applyShaderASync(image, resultImage, BlackAndWhite);
    applyShaderASync(resultImage, resultImage, Shrinking);
    resultImage.saveToFile("./img/3c_Shrinking.png");

    applyShaderASync(image, resultImage, BlackAndWhite);
    applyShaderASync(resultImage, resultImage, Expansion);
    resultImage.saveToFile("./img/3c_Expansion.png");

    applyShaderASync(image, resultImage, BlackAndWhite);
    applyShaderASync(resultImage, resultImage, Expansion);
    applyShaderASync(resultImage, resultImage, Shrinking);
    resultImage.saveToFile("./img/3c_Expansion+Shrinking.png");

    applyShaderASync(image, resultImage, BlackAndWhite);
    applyShaderASync(resultImage, resultImage, Shrinking);
    applyShaderASync(resultImage, resultImage, Expansion);
    resultImage.saveToFile("./img/3c_Shrinking+Expansion.png");

    srand(2525354526);
    applyShaderASync(image, resultImage, BlackAndWhiteWithNoise);
    resultImage.saveToFile("./img/2b_BlackAndWhiteWithNoise.png");

    applyShaderASync(image, resultImage, Blur);
    resultImage.saveToFile("./img/1c_Blur.png");
    for (int i = 0; i < 9; i++)
        applyShaderASync(resultImage, resultImage, Blur);
    resultImage.saveToFile("./img/1c_BlurX10.png");

    applyShaderASync(image, resultImage, Sharpness);
    resultImage.saveToFile("./img/1c_Sharpened.png");

    applyShaderASync(image, resultImage, Edges);
    resultImage.saveToFile("./img/1c_Edges.png");

    applyShaderASync(image, resultImage, GrayScale);
    applyShaderASync(resultImage, resultImage, Embossing);
    applyShaderASync(resultImage, resultImage, EmbossingSecondStep);
    resultImage.saveToFile("./img/1c_Embossing.png");

    applyShaderASync(image, resultImage, CUSTOMMEGAFILTER);
    resultImage.saveToFile("./img/1c_CUSTOMMEGAFILTER.png");
    for (int i = 0; i < 4; i++)
        applyShaderASync(resultImage, resultImage, CUSTOMMEGAFILTER);
    resultImage.saveToFile("./img/1c_CUSTOMMEGAFILTERx5.png");

    ratio = 4;
    resultImage.create(image.getSize().x * ratio - ratio + 1, image.getSize().y * ratio - ratio + 1);
    applyShaderSync(image, resultImage, NearestScaling);
    resultImage.saveToFile("./img/1b_NearestScaling.png");

    ratio = 4;
    resultImage.create(image.getSize().x * ratio - ratio + 1, image.getSize().y * ratio - ratio + 1);
    applyShaderSync(image, resultImage, BilinearScaling);
    resultImage.saveToFile("./img/1b_BilinearScaling.png");

    ratio = 4;
    resultImage.create(image.getSize().x * ratio - ratio + 1, image.getSize().y * ratio - ratio + 1);
    applyShaderSync(image, resultImage, BicubicScaling);
    resultImage.saveToFile("./img/1b_BicubicScaling.png");

    return 0;
}
