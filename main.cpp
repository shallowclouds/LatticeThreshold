#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "CJPG/CJPG.h"
using namespace std;

double get_percent(CJPG& pic)
{
    int w = pic.get_width(), h = pic.get_height();
    int total_pixel = w * h;
    int gray_count[256];
    double gray_smooth[6][256];
    int** bitmap = new int*[h];
    for (int i = 0; i < h; i++)
        bitmap[i] = new int[w];
    memset(gray_smooth, 0, sizeof(gray_smooth));
    memset(gray_count, 0, sizeof(gray_count));
    int min_gray = 255, max_gray = 0;
    int sum = 0;
    int minR = 255;
    int minG = 255;
    int minB = 255;
    int maxR = 0;
    int maxG = 0;
    int maxB = 0;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            pixel pix = pic.get_pixel(i, j);
            maxR = max(maxR, pix.r);
            maxG = max(maxG, pix.g);
            maxB = max(maxB, pix.b);
            minR = min(minR, pix.r);
            minG = min(minG, pix.g);
            minB = min(minB, pix.b);
            // 加权平均
//            int gray = (int)((double) pix.r * 0.3 + (double) pix.g * 0.59 + (double) pix.b * 0.11);
            int gray = (int)(((double) pix.r) * 0.299 + ((double) pix.g) * 0.587 + ((double) pix.b) * 0.144);
            // 直接平均
//            int gray = (int)((((double) pix.r) + ((double) pix.g) + ((double) pix.b)) / 3.0);
            // 最大值
//            int gray = max(max(pix.r, pix.g), pix.b);
            gray_count[gray]++;
            gray_smooth[0][gray] = (double) gray_count[gray];
            sum += gray;
            min_gray = min(min_gray, gray);
            max_gray = max(max_gray, gray);
            bitmap[i][j] = gray;
        }
    }

    int min_dot = 0;
    for (int i = 0; i < 255; i++)
    {
        min_gray = i;
        min_dot += gray_count[i];
        if (min_dot > total_pixel / 800)
            break;
    }

    int max_dot = 0;
    for (int i = 255; i > 0; i--)
    {
        max_gray = i;
        max_dot += gray_count[i];
        if (max_dot > total_pixel / 800)
            break;
    }
    for (int i = 0; i < 256; i++)
    {
        if(i <= min_gray || i >= max_gray)
            gray_smooth[0][i] = 0;
    }
    for (int i = 1; i < 4; i++)
    {
        for (int j = 2; j < 254; j++)
        {
            gray_smooth[i][j] = (((
                    (gray_smooth[i - 1][j - 2] + gray_smooth[i - 1][j - 1]) + gray_smooth[i - 1][j]) +
                            gray_smooth[i - 1][j + 1]) + gray_smooth[i - 1][j + 2]) / 5.0;
        }
    }
    for (int i = 1; i < 256; i++)
    {
        gray_smooth[4][i] = gray_smooth[3][i] - gray_smooth[3][i - 1];
    }
    bool is_rise = true;
    for (int i = min_gray; i <= max_gray; i++)
    {
        if (is_rise && gray_smooth[4][i] >= 0.0
        && gray_smooth[4][i + 1] < 0.0
        && gray_smooth[4][i + 2] < 0.0
        && gray_smooth[4][i + 3] < 0)
        {
            gray_smooth[5][i] = 1.0;
            is_rise = false;
        }
        if(!is_rise && gray_smooth[4][i] < 0.0
        && gray_smooth[4][i + 1] >= 0.0
        && gray_smooth[4][i + 2] >= 0.0
        && gray_smooth[4][i + 3] >= 0.0)
        {
            gray_smooth[5][i] = -1.0;
            is_rise = true;
        }
    }
    if (!is_rise && gray_smooth[5][max_gray - 1] == 0.0
    && gray_smooth[5][max_gray] == 0.0)
    {
        gray_smooth[5][max_gray] = -1.0;
    }
    int gray_mode = 0;
    int gray_high = 0;
    int gray_low = 0;
    int gray_wave[10][2];
    memset(gray_wave, 0, sizeof(gray_wave));
    int gray_wave_row = 0;
    int fst_max_idx = min_gray;
    for (int i = min_gray; i <= max_gray; i++)
    {
        if (gray_smooth[5][i] > 0.0)
        {
            gray_mode++;
            gray_high++;
            if (gray_wave_row == 0)
            {
                fst_max_idx = i;
            }
            gray_wave[gray_wave_row][0] = i;
        }
        if (gray_smooth[5][i] < 0.0)
        {
            gray_low++;
            gray_wave[gray_wave_row][1] = i;
            gray_wave_row++;
        }
    }
    int lst_max_idx = max_gray;
    for (int i = max_gray; i >= min_gray; i--)
    {
        if (gray_smooth[5][i] > 0.0)
        {
            lst_max_idx = i;
            if (max_gray - lst_max_idx > 10)
            {
                break;
            }
        }
    }
    if(lst_max_idx - min_gray < 10)
    {
        gray_mode = 10;
    }
    int mid_sip = (max_gray + min_gray) / 2;
    int sum_mid = sum / ((w * h) / 4);
    int mid_val = 0;
    switch (gray_mode)
    {
        case 0:
            mid_val = mid_sip;
            break;

        case 1:
            if (lst_max_idx - fst_max_idx < 20)
            {
                if(fst_max_idx - mid_sip <= 0)
                {
                    mid_val = fst_max_idx + ((max_gray - fst_max_idx) / 2);
                    break;
                }
                mid_val = min_gray + ((fst_max_idx - min_gray) / 2);
                break;
            }
            mid_val = mid_sip;
            break;

        case 2:
            mid_val = gray_wave[0][1];
            break;

        case 3:
            mid_val = gray_wave[1][0];
            break;

        case 4:
            mid_val = gray_wave[1][1];
            break;

        default:
            mid_val = (fst_max_idx + lst_max_idx) / 2;
            break;
    }
    int black_pixel = 0;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            int gray = 0;
            if (bitmap[i][j] >= mid_val)
                gray = 255;
            else
                black_pixel++;
            pic.set_pixel(i, j, gray, gray, gray);
        }
    }
    pic.write("bi.jpg");
    for (int i = 0; i < h; i++)
        delete[] bitmap[i];
    delete[] bitmap;
    printf("black: %d total: %d threshold: %d\n", black_pixel, total_pixel, mid_val);
//    printf("[ ");
//    for(int i = 0; i < 256; i++)
//        printf("%d, ", gray_count[i]);
//    printf("]\n");
    const int tt = 912384;
    int tmp_sum = 0;
    for(int i = 0; i < 256; i++)
    {
        tmp_sum += gray_count[i];
        if(tmp_sum >= tt){
            printf("great_thresold: %d\n", i);
            break;
        }
    }
    return (double)black_pixel / (double) total_pixel;
}

double get_percent_part(CJPG& pic)
{
    int w = pic.get_width(), h = pic.get_height();
//    int total_pixel = w * h;
    int gray_count[256];
    double gray_smooth[6][256];
    int** bitmap = new int*[h];
    for (int i = 0; i < h; i++)
        bitmap[i] = new int[w];
    int box_width = (w * 2) / 5;
    int box_height = h / 2;
    int total_pixel = box_width * box_height;
    int left = (w * 3) / 10;
    int right = left + box_width;
    int top = h / 4;
    int bottom = top + box_height;
    memset(gray_smooth, 0, sizeof(gray_smooth));
    memset(gray_count, 0, sizeof(gray_count));
    int min_gray = 255, max_gray = 0;
    int sum = 0;
    int minR = 255;
    int minG = 255;
    int minB = 255;
    int maxR = 0;
    int maxG = 0;
    int maxB = 0;

    for (int i = top; i < bottom; i++)
    {
        for (int j = left; j < right; j++)
        {
            pixel pix = pic.get_pixel(i, j);
            maxR = max(maxR, pix.r);
            maxG = max(maxG, pix.g);
            maxB = max(maxB, pix.b);
            minR = min(minR, pix.r);
            minG = min(minG, pix.g);
            minB = min(minB, pix.b);
            // 加权平均
//            int gray = (int)((double) pix.r * 0.3 + (double) pix.g * 0.59 + (double) pix.b * 0.11);
            int gray = (int)(((double) pix.r) * 0.299 + ((double) pix.g) * 0.587 + ((double) pix.b) * 0.144);
            // 直接平均
//            int gray = (int)((((double) pix.r) + ((double) pix.g) + ((double) pix.b)) / 3.0);
            // 最大值
//            int gray = max(max(pix.r, pix.g), pix.b);
            gray_count[gray]++;
            gray_smooth[0][gray] = (double) gray_count[gray];
            sum += gray;
            min_gray = min(min_gray, gray);
            max_gray = max(max_gray, gray);
            bitmap[i][j] = gray;
        }
    }

    int min_dot = 0;
    for (int i = 0; i < 255; i++)
    {
        min_gray = i;
        min_dot += gray_count[i];
        if (min_dot > total_pixel / 800)
            break;
    }

    int max_dot = 0;
    for (int i = 255; i > 0; i--)
    {
        max_gray = i;
        max_dot += gray_count[i];
        if (max_dot > total_pixel / 800)
            break;
    }
    for (int i = 0; i < 256; i++)
    {
        if(i <= min_gray || i >= max_gray)
            gray_smooth[0][i] = 0;
    }
    for (int i = 1; i < 4; i++)
    {
        for (int j = 2; j < 254; j++)
        {
            gray_smooth[i][j] = ((((gray_smooth[i - 1][j - 2] + gray_smooth[i - 1][j - 1]) + gray_smooth[i - 1][j]) +
                                  gray_smooth[i - 1][j + 1]) + gray_smooth[i - 1][j + 2]) / 5.0;
        }
    }
    for (int i = 1; i < 256; i++)
    {
        gray_smooth[4][i] = gray_smooth[3][i] - gray_smooth[3][i - 1];
    }
    bool is_rise = true;
    for (int i = min_gray; i <= max_gray; i++)
    {
        if (is_rise && gray_smooth[4][i] >= 0.0
            && gray_smooth[4][i + 1] < 0.0
            && gray_smooth[4][i + 2] < 0.0
            && gray_smooth[4][i + 3] < 0)
        {
            gray_smooth[5][i] = 1.0;
            is_rise = false;
        }
        if(!is_rise && gray_smooth[4][i] < 0.0
           && gray_smooth[4][i + 1] >= 0.0
           && gray_smooth[4][i + 2] >= 0.0
           && gray_smooth[4][i + 3] >= 0.0)
        {
            gray_smooth[5][i] = -1.0;
            is_rise = true;
        }
    }
    if (!is_rise && gray_smooth[5][max_gray - 1] == 0.0
        && gray_smooth[5][max_gray] == 0.0)
    {
        gray_smooth[5][max_gray] = -1.0;
    }
    int gray_mode = 0;
    int gray_high = 0;
    int gray_low = 0;
    int gray_wave[10][2];
    memset(gray_wave, 0, sizeof(gray_wave));
    int gray_wave_row = 0;
    int fst_max_idx = min_gray;
    for (int i = min_gray; i <= max_gray; i++)
    {
        if (gray_smooth[5][i] > 0.0)
        {
            gray_mode++;
            gray_high++;
            if (gray_wave_row == 0)
            {
                fst_max_idx = i;
            }
            gray_wave[gray_wave_row][0] = i;
        }
        if (gray_smooth[5][i] < 0.0)
        {
            gray_low++;
            gray_wave[gray_wave_row][1] = i;
            gray_wave_row++;
        }
    }
    int lst_max_idx = max_gray;
    for (int i = max_gray; i >= min_gray; i--)
    {
        if (gray_smooth[5][i] > 0.0)
        {
            lst_max_idx = i;
            if (max_gray - lst_max_idx > 10)
            {
                break;
            }
        }
    }
    if(lst_max_idx - min_gray < 10)
    {
        gray_mode = 10;
    }
    int mid_sip = (max_gray + min_gray) / 2;
    int sum_mid = sum / ((w * h) / 4);
    int mid_val = 0;
    switch (gray_mode)
    {
        case 0:
            mid_val = mid_sip;
            break;

        case 1:
            if (lst_max_idx - fst_max_idx < 20)
            {
                if(fst_max_idx - mid_sip <= 0)
                {
                    mid_val = fst_max_idx + ((max_gray - fst_max_idx) / 2);
                    break;
                }
                mid_val = min_gray + ((fst_max_idx - min_gray) / 2);
                break;
            }
            mid_val = mid_sip;
            break;

        case 2:
            mid_val = gray_wave[0][1];
            break;

        case 3:
            mid_val = gray_wave[1][0];
            break;

        case 4:
            mid_val = gray_wave[1][1];
            break;

        default:
            mid_val = (fst_max_idx + lst_max_idx) / 2;
            break;
    }
    int black_pixel = 0;
    for (int i = top; i < bottom; i++)
    {
        for (int j = left; j < right; j++)
        {
            int gray = 0;
            if (bitmap[i][j] >= mid_val)
                gray = 255;
            else
                black_pixel++;
            if(gray == 0)
                pic.set_pixel(i, j, gray, gray, gray);
        }
    }
    pic.write("bi.jpg");
    for (int i = 0; i < h; i++)
        delete[] bitmap[i];
    delete[] bitmap;
    printf("%d\n", mid_val);
    return (double)black_pixel / (double) total_pixel;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "please input file name.\n");
        return 0;
    }
    CJPG pic(argv[1]);
    printf("%.6lf\n", get_percent_part(pic));
    return 0;
}
