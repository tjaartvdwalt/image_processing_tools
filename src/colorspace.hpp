#ifndef COLORSPACE_HPP
#define COLORSPACE_HPP

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

cv::Mat convert_rbg_to_hsi(cv::Mat rgb_image);
cv::Mat convert_hsi_to_rgb(cv::Mat hsi_image);

cv::Mat normalize_rgb(cv::Mat image);
float* average(cv::Mat_<cv::Vec3f> image);
std::vector<float> rgb_dists(cv::Mat image1, cv::Mat image2);
std::vector<float> hsi_dists(cv::Mat image1, cv::Mat image2);
float avg_dist(std::vector<float> dists);
float std_dev(std::vector<float> dists);
#endif


