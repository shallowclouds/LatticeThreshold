#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "CJPG/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "CJPG/stb_image.h"
#include <iostream>
using namespace std;

int main()
{
    cv::VideoCapture capture(1);
    if(!capture.isOpened())
    {
        cout << "error" << endl;
        return -1;
    }
    cv::Mat frame;
    capture >> frame;
    auto data = new uchar[frame.cols * frame.rows * 3];
    cout << frame.rows << " " << frame.cols << endl;
    for(int i = 0; i < frame.rows; i++)
    {
        for(int j = 0; j < frame.cols; j++)
        {
            auto tp = frame.at<cv::Vec3b>(i, j);
            data[i * frame.cols * 3 + j * 3 + 0] = tp[2];
            data[i * frame.cols * 3 + j * 3 + 1] = tp[1];
            data[i * frame.cols * 3 + j * 3 + 2] = tp[0];
        }
    }
    stbi_write_jpg("open_cv.jpg", frame.cols, frame.rows, frame.channels(), data, 100);
    delete[] data;
    return 0;
}
