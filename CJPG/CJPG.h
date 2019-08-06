//
// Created by Yorling on 2019/8/1.
//

#ifndef BIPIC_CJPG_H
#define BIPIC_CJPG_H
#include <iostream>
#include <cstdio>
using namespace std;

typedef unsigned char byte;

struct pixel
{
    int r, g, b, a;
    pixel(int tr = 0, int tg = 0, int tb = 0, int ta = 0)
    :r(tr), g(tg), b(tb), a(ta)
    {

    }
};

class CJPG{

    byte* data;
    int width, height, channel;

public:
    CJPG(const char* filename);

    CJPG(const CJPG& rhs);

    CJPG(byte* data_t, int h, int w, int c);

    ~CJPG();

    bool set_pixel(int x, int y, int r, int g, int b);

    bool set_pixel(int x, int y, pixel p);

    pixel get_pixel(int x, int y) const;

    bool get_pixel(int x, int y, int& r, int& g, int& b) const;

    bool write(const char* filename, int quality = 4);

    bool read(const char* filename);

    int get_width() const;

    int get_height() const;

    int get_channel() const;

    byte* get_data_ptr() const;

};


#endif //BIPIC_CJPG_H
