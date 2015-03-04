#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <unistd.h>
#include <cstdio>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

typedef enum {BLUE = 0, GREEN = 1, RED = 2, ALPHA = 3} channel_color;

struct kernel_s {
  cv::Mat* kernel;

  int row_size() {
    return kernel->rows;
  };

  int col_size() {
    return kernel->cols;
  };

};

const char* get_color_string(int color);
cv::Mat read_image(char* filename);
cv::Mat read_image_gray(char* filename);
void display_image(cv::Mat image, const char* title, cv::MouseCallback on_mouse, void (* key_listener)());
void display_image(cv::Mat image, const char* title, cv::MouseCallback on_mouse);
void display_image(cv::Mat image, const char* title);
float average_distance(cv::Mat image1, cv::Mat image2);
void save_image(char* name, cv::Mat image);
std::vector<cv::Mat> split_channels(cv::Mat image);
cv::Mat merge_channels(std::vector<cv::Mat> channels);
cv::Mat display_channel_in_color(cv::Mat channel_image, int channel, int channels = 3);

cv::Mat apply_kernel(cv::Mat kernel, cv::Mat* image);
cv::Mat convert_float_to_grayscale(cv::Mat float_image);
cv::Mat convert_grayscale_to_float(cv::Mat image);
cv::Mat shift(cv::Mat image) ;
cv::Mat scale(cv::Mat image, int max_scale) ;
cv::Mat clip_values(cv::Mat image);
#endif




























































