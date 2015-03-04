#include "colorspace.hpp"

#include <cstdio>

#define PI 3.14159265
#define DEGTORAD(x) ((x) * PI / 180.0)
#define RADTODEG(x) ((x) * 180.0 / PI)

cv::Mat normalize_rgb(cv::Mat image) {
  cv::Mat new_image = cv::Mat(image.rows, image.cols, CV_32FC3);

  for (int r = 0; r < image.rows; r++) {
    for (int c = 0; c < image.cols; c++) {
      new_image.at<cv::Vec3f>(r, c)[0] = image.at<cv::Vec3b>(r, c)[0] / 255.0;
      new_image.at<cv::Vec3f>(r, c)[1] = image.at<cv::Vec3b>(r, c)[1] / 255.0;
      new_image.at<cv::Vec3f>(r, c)[2] = image.at<cv::Vec3b>(r, c)[2] / 255.0;
    }
  }

  return new_image;
}

cv::Mat convert_rbg_to_hsi(cv::Mat rgb_image) {
  cv::Mat hsi_image = cv::Mat(rgb_image.rows, rgb_image.cols , CV_32FC3);

  for (int r = 0; r < rgb_image.rows; r++) {
    for (int c = 0; c < rgb_image.cols; c++) {
      cv::Point3_<uchar>* p = rgb_image.ptr<cv::Point3_<uchar> >(r, c);
      float blue = p->x / 255.0;
      float green = p->y / 255.0;
      float red = p->z / 255.0;

      float hue, saturation, intensity;
      // get the minimum value between red, green and blue
      float min_val = std::min(red, green);
      min_val = std::min(min_val, blue);


      // If we have 0 downstairs saturation = 0
      if(red + green + blue == 0) {
        saturation = 0;
      } else {
        saturation = 1.0 - 3.0 / (red + green + blue) * min_val;
      }

      intensity = 1.0 / 3 * (red + green + blue);

      float numerator = (0.5 * ((red - green) + (red - blue)));
      float theta;

      // if red = green = blue we have 90 degrees
      if(red == green && green == blue) {
        theta = 90;
      } else {
        float denominator = sqrt((pow((red - green), 2) + (red - blue) * (green - blue)));
        theta =  RADTODEG(acosf(numerator / denominator));
      }

      if(blue > green) {
        hue = theta;
      } else {
        hue = 360.0 - theta;
      }

      hsi_image.at<cv::Vec3f>(r, c)[0] = hue / 360.0;
      hsi_image.at<cv::Vec3f>(r, c)[1] = saturation;
      hsi_image.at<cv::Vec3f>(r, c)[2] = intensity;
    }

  }

  return hsi_image;
}

cv::Mat convert_hsi_to_rgb(cv::Mat hsi_image) {
  cv::Mat rgb_image = cv::Mat(hsi_image.rows, hsi_image.cols, CV_8UC3);

  for (int r = 0; r < hsi_image.rows; r++) {
    for (int c = 0; c < hsi_image.cols; c++) {
      float hue = hsi_image.at<cv::Vec3f>(r, c)[0] * 360.0;
      float saturation = hsi_image.at<cv::Vec3f>(r, c)[1];
      float intensity = hsi_image.at<cv::Vec3f>(r, c)[2];

      float red = 0, green = 0, blue = 0;

      if(hue < 120.0) {
        blue = intensity * (1 - saturation);
        red = intensity * (1 + ((saturation * cosf(DEGTORAD(hue))) / (cosf(DEGTORAD(60.0 - hue)))));
        green = 3 * intensity - (red + blue);
      }

      if(hue >= 120.0 && hue < 240.0) {
        hue = hue - 120.0;
        green = intensity * (1 + ((saturation * cosf(DEGTORAD(hue))) / (cosf(DEGTORAD(60.0 - hue)))));
        red = intensity * (1 - saturation);
        blue = 3 * intensity - (green + red);
      }

      if(hue >= 240.0) {
        hue = hue - 240.0;
        blue = intensity * (1 + ((saturation * cosf(DEGTORAD(hue))) / (cosf(DEGTORAD(60.0 - hue)))));
        green = intensity * (1 - saturation);
        red = 3 * intensity - (blue + green);
      }


      if(red > 1) {
        red = 1;
      } else if (red < 0) {
        red = 0;
      }

      if(green > 1) {
        green = 1;
      } else if (green < 0) {
        green = 0;
      }

      if(blue > 1) {
        blue = 1;
      } else if(blue < 0) {
        blue = 0;
      }

      rgb_image.at<cv::Vec3b>(r, c)[0] = uchar(green * 255.0);
      rgb_image.at<cv::Vec3b>(r, c)[1] = uchar(blue * 255.0);
      rgb_image.at<cv::Vec3b>(r, c)[2] = uchar(red * 255.0);
    }
  }

  return rgb_image;
}

// Calculate the average value for each of the 3 channels.
//
//I tried to use a template with this function, but it proved to be
// tricky. To fix it I assume a float image. For RGB images you can
// use the normalize_rgb function to get it into float before using
// this function.
float*  average(cv::Mat_<cv::Vec3f> image) {
  float cum_red = 0, cum_green = 0, cum_blue = 0;

  for(int r = 0; r < image.rows; r++ ) {
    for(int c = 0; c < image.cols; c++) {
      cum_blue += image.at<cv::Vec3f>(r, c)[0];
      cum_green += image.at<cv::Vec3f>(r, c)[1];
      cum_red += image.at<cv::Vec3f>(r, c)[2];
    }
  }

  float* return_array = (float*)malloc(3 * sizeof(float*));
  return_array[0] = cum_blue / (image.rows * image.cols);
  return_array[1] = cum_green / (image.rows * image.cols);
  return_array[2] = cum_red / (image.rows * image.cols);
  return return_array;
}

// Generic function that calculates the distance between 2 pixels of the same type
template <class T> float dist(T pixel1, T pixel2) {
  /* TODO: To make this code more generic we can try to find a way to
   * figure out the number of channels for the image. This proves to
   * be pretty difficult to do because the pixels are already of template
   * type, and not a simple array. For now we assume we use 3 channels
   */
  float x1 = float(pixel1[0]);
  float x2 = float(pixel2[0]);
  float y1 = float(pixel1[1]);
  float y2 = float(pixel2[1]);
  float z1 = float(pixel1[2]);
  float z2 = float(pixel2[2]);

  return sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2) + pow((z1 - z2), 2));
}

std::vector<float> rgb_dists(cv::Mat image1, cv::Mat image2) {
  std::vector<float> vec;

  // We can use the size of any image of the 2 images since we already
  // made sure they are of the same size
  for(int r = 0; r < image1.rows; r++ ) {
    for(int c = 0; c < image1.cols; c++) {
      vec.push_back(dist(image1.at<cv::Vec3f>(r, c), image2.at<cv::Vec3f>(r, c)));
    }
  }

  return vec;
}


std::vector<float> hsi_dists(cv::Mat image1, cv::Mat image2) {
  std::vector<float> vec;
  /* In order to calculate the distances we need to get the cartesian
   * coordinates in the color space for each pixel.
   *
   * If we look at the top view, we can get the x and y coordinates
   * from H and S by doing some trigenomitry. I gives us the z value
   * This gives us a bit of a weird system with (x,y) = (0,0) in the
   * center of the top view and z = 0 at the bottom, but since both
   * pixels are in this system we can use it to calculate the distance
   */

  float h1, s1, i1, h2, s2, i2;
  float x1, y1, z1, x2, y2, z2;

  for(int r = 0; r < image1.rows; r++ ) {
    for(int c = 0; c < image1.cols; c++) {
      // First seperate the HSI components
      h1 = image1.at<cv::Vec3f>(r, c)[0];
      h2 = image2.at<cv::Vec3f>(r, c)[0];
      s1 = image1.at<cv::Vec3f>(r, c)[1];
      s2 = image2.at<cv::Vec3f>(r, c)[1];
      i1 = image1.at<cv::Vec3f>(r, c)[2];
      i2 = image2.at<cv::Vec3f>(r, c)[2];

      x1 = s1 * cos(DEGTORAD(h1));
      x2 = s2 * cos(DEGTORAD(h2));
      y1 = s1 * sin(DEGTORAD(h1));
      y2 = s2 * sin(DEGTORAD(h2));
      z1 = i1;
      z2 = i2;

      cv::Vec3f pixel1 (x1, y1, z1);
      cv::Vec3f pixel2 (x2, y2, z2);
      vec.push_back(dist(pixel1, pixel2));
    }
  }

  return vec;

}

/* generic function to give the average of a vector of values */
float avg_dist(std::vector<float> dists) {
  float sum = 0;
  size_t pixel_count = dists.size();

  for(size_t i = 0; i < dists.size(); i++ ) {
    // printf("dist: %f\n", dists.at(i));
    sum += dists.at(i);
  }

  return (sum / pixel_count);
}

/* generic function to calculate the standard devation of a vector of
 * values*/
float std_dev(std::vector<float> dists) {
  float avg = avg_dist(dists);
  float sum = 0;
  size_t pixel_count = dists.size();

  for(size_t i = 0; i < pixel_count; i++ ) {
    sum += pow(dists.at(i) - avg, 2);
  }

  float variance = sum / pixel_count;
  float std_dev = sqrt(variance);
  return std_dev;
}


// float hsi_avg_dist(cv::Mat image1, cv::Mat image2) {
//   float sum = 0;

//   for(int r = 0; r < image1.rows; r++ ) {
//     for(int c = 0; c < image1.cols; c++) {
//       cv::Vec3f p1 = calc_hsi_coord();
//       cv::Vec3f p2 = calc_hsi_coord();

//       sum += dist(p1, p2);
//     }
//   }
// }

// float hsi_std_dev(cv::Mat image1, cv::Mat image2) {

// }

//   int pixels = image.rows * image.cols;
//   float variance = rgb_sum_of_square_dif(image) / pixels;
//   return sqrt(variance);
// }

// float rgb_std_dev(cv::Mat image) {
//   int pixels = image.rows * image.cols;
//   float variance = rgb_sum_of_square_dif(image) / pixels;
//   return sqrt(variance);
// }
