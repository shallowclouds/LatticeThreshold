//
// Created by shall on 2019/8/1.
//
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "CJPG.h"

CJPG::CJPG(const char *filename)
{
    FILE* file = fopen(filename, "rb");
    if (file == nullptr)
    {
        fprintf(stderr, "file not found.\n");
        return;
    }
    this->data = stbi_load_from_file(file, &width, &height, &channel, 0);
    if (data == nullptr)
    {
        fprintf(stderr, "file is not jpeg format.\n");
        return;
    }
}

CJPG::CJPG(const CJPG& rhs)
:width(rhs.get_width()), height(rhs.get_height()), channel(rhs.get_channel())
{
    data = new byte[width * height * channel];
    memcpy(data, rhs.get_data_ptr(), width * height * channel);
}

CJPG::CJPG(byte* data_t, int h, int w, int c)
:data(data_t), height(h), width(w), channel(c)
{

}

CJPG::~CJPG()
{
    stbi_image_free(data);
}

bool CJPG::set_pixel(int x, int y, int r, int g, int b)
{
    if (data == nullptr)
    {
        fprintf(stderr, "has no image content.\n");
        return false;
    }
    data[channel * x * width + y * channel + 0] = r;
    data[channel * x * width + y * channel + 1] = g;
    data[channel * x * width + y * channel + 2] = b;
    return true;
}

bool CJPG::set_pixel(int x, int y, pixel p)
{
    return set_pixel(x, y, p.r, p.g, p.b);
}

pixel CJPG::get_pixel(int x, int y) const
{
    pixel tp;
    get_pixel(x, y, tp.r, tp.g, tp.b);
    return tp;
}

bool CJPG::get_pixel(int x, int y, int& r, int& g, int& b) const
{
    if(data == nullptr)
    {
        fprintf(stderr, "has no image content.\n");
        return false;
    }
    r = data[channel * x * width + y * channel + 0];
    g = data[channel * x * width + y * channel + 1];
    b = data[channel * x * width + y * channel + 2];
    return true;
}

bool CJPG::write(const char* filename, int quality)
{
    return stbi_write_jpg(filename, width, height, channel, data, width * 4);
}

bool CJPG::read(const char* filename)
{
    FILE* file = fopen(filename, "rb");
    if (file == nullptr)
    {
        fprintf(stderr, "file not found.\n");
        return false;
    }
    this->data = stbi_load_from_file(file, &width, &height, &channel, 0);
    if (data == nullptr)
    {
        fprintf(stderr, "file is not jpeg format.\n");
        return false;
    }
    return true;
}

int CJPG::get_width() const
{
    return width;
}

int CJPG::get_height() const
{
    return height;
}

int CJPG::get_channel() const
{
    return channel;
}

byte* CJPG::get_data_ptr() const
{
    return data;
}