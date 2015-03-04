/* This file contains utility functions that pertain to reading and
 * displaying the images
 */

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cstdio>
#include <iostream>

#include <vector>

#include "image.hpp"

const char* get_color_string(int color) {
  const char* blue = "Blue";
  const char* green = "Green";
  const char* red = "Red";
  const char* alpha = "Alpha";

  switch(color) {
  case BLUE:
    return blue;

  case GREEN:
    return green;

  case RED:
    return red;

  case ALPHA:
    return alpha;
  }

  return "";
}

cv::Mat read_image(char* filename, int type) {
  cv::Mat image = cv::imread(filename, type);

  if(! image.data ) {
    fprintf(stderr, "Error: Could not open the image file: %s.\n", filename);
    exit(1);
  }

  return image;
}

cv::Mat read_image(char* filename) {
  return read_image(filename, CV_LOAD_IMAGE_UNCHANGED);
}

cv::Mat read_image_gray(char* filename) {
  return read_image(filename, CV_LOAD_IMAGE_GRAYSCALE);
}


void display_image(cv::Mat image, const char* title, cv::MouseCallback on_mouse, void (* key_listener)()) {
  namedWindow(title , cv::WINDOW_AUTOSIZE);
  cv::setMouseCallback(title, on_mouse, 0);
  cv::imshow(title, image);

  if(key_listener != NULL) {
    key_listener();
  } else {
    cv::waitKey(0);
  }
}

void save_image(char* name, cv::Mat image) {
  cv::imwrite(name, image);

}
void display_image(cv::Mat image,  const char* title, cv::MouseCallback on_mouse) {
  display_image(image, title, on_mouse, NULL);
}

void display_image(cv::Mat image, const char* title) {
  display_image(image, title, NULL);
}

// Calculate the average distance between two images
float average_distance(cv::Mat image1, cv::Mat image2) {
  int cum_dist = 0;

  for(int r = 0; r < image1.rows; r++ ) {
    for(int c = 0; c < image1.cols; c++) {
      cum_dist += abs(image1.at<uchar>(r, c) - image2.at<uchar>(r, c));
    }
  }

  // * 1.0 to cast to float
  return (cum_dist * 1.0) / (image1.rows * image1.cols);
}


std::vector<cv::Mat> split_channels(cv::Mat image) {
  std::vector<cv::Mat> channels(image.channels());
  cv::split(image, channels);
  return channels;
}

cv::Mat merge_channels(std::vector<cv::Mat> channels) {
  cv::Mat image = cv::Mat(channels[0].rows, channels[0].cols, CV_8UC(channels.size()));
  cv::merge(channels, image);
  return image;
}

/* This function takes a color channel (an 8 bit image) and tranlates
 * it to an RGB channel. i.e it creates a 3 channel image, and offsets
 * the value into the given channel.*/
cv::Mat display_channel_in_color(cv::Mat channel_image, int channel, int channels) {
  std::vector<cv::Mat> color_image(channels);
  cv::Mat black_channel = cv::Mat(channel_image.rows, channel_image.cols, CV_8UC1);

  for(int i = 0; i < channels; i++) {

    if(i == channel) {
      color_image[i] = channel_image;
    } else {
      color_image[i] = black_channel.clone();
    }
  }

  return merge_channels(color_image);
}

float kernel_sum(cv::Mat kernel) {
  float sum = 0;

  for(int r = 0; r < kernel.rows; r++) {
    for(int c = 0; c < kernel.cols; c++) {
      sum += std::abs(kernel.at<float>(r, c));
    }
  }

  return sum;
};


// uchar kernel_pixel_value(cv::Mat kernel, uchar input_pixel) {
//   // Sum of all kernel elements * pixel value
//   int el_sum = 0;

//   for(int r = 0; r < kernel.rows; r++) {
//     for(int c = 0; c < kernel.cols; c++) {
//       el_sum += kernel.at<uchar>(r, c) * input_pixel;
//     }
//   }

//   // printf("%d\n", el_sum / kernel_sum(kernel));
//   return el_sum / kernel_sum(kernel);
// }


// a generic method that applies a kernel to an image
cv::Mat apply_kernel(cv::Mat kernel, cv::Mat* image) {
  cv::Mat output_image = cv::Mat(image->rows, image->cols, CV_32F);

  for(int r = 0; r < image->rows; r++) {
    for(int c = 0; c < image->cols; c++) {
      float el_sum = 0.0;

      for(int k_r = 0; k_r < kernel.rows; k_r++) {
        for(int k_c = 0; k_c < kernel.cols; k_c++) {
          // take the abs value of pixel to take care of edges

          int i_r = r + (k_r - floor(kernel.rows / 2));

          if(i_r < 0) {
            i_r = std::abs(i_r);
          } else if(i_r > image->rows) {
            i_r = r - (k_r - floor(kernel.rows / 2));
          }

          int i_c = c + (k_c - floor(kernel.cols / 2));

          if(i_c < 0) {
            i_c = std::abs(i_c);
          } else if(i_c > image->cols) {
            i_c = c - (k_c - floor(kernel.cols / 2));
          }

          // printf("real(%d, %d) kernel(%d, %d)   image(%d, %d)\n", r, c, k_r, k_c, i_r, i_c);
          el_sum += kernel.at<float>(k_r, k_c) * image->at<uchar>(i_r, i_c);
          // printf("image = %d\n", image->at<uchar>(i_r, i_c));

          // printf("el_sum = %f\n", el_sum);
          // printf("el_sum = %f\n", el_sum);
          // printf("kernel %f\n", kernel.at<float>(k_r, k_c));
          // printf("kernel.at<float>(k_r, k_c) = %f\n", kernel.at<float>(k_r, k_c));
        }
      }

      // printf("el_sum / kernel_sum(kernel): %f/%f = %f\n", el_sum, kernel_sum(kernel), el_sum / kernel_sum(kernel) * 1.0 );

      output_image.at<float>(r, c) =  el_sum / kernel_sum(kernel) * 1.0;
    }
  }

  return output_image;
}

cv::Mat convert_float_to_grayscale(cv::Mat float_image) {
  cv::Mat return_image = cv::Mat(float_image.rows, float_image.cols, CV_8UC1);

  for(int r = 0; r < float_image.rows; r++) {
    for(int c = 0; c < float_image.cols; c++) {
      if(float_image.at<float>(r, c) > 255) {
        return_image.at<uchar>(r, c) = 255;
      } else if(float_image.at<float>(r, c) < 0) {
        return_image.at<uchar>(r, c) = 0;
      } else {
        return_image.at<uchar>(r, c) = uchar(float_image.at<float>(r, c));
      }
    }
  }

  return return_image;
}

cv::Mat convert_grayscale_to_float(cv::Mat image) {
  cv::Mat return_image = cv::Mat(image.rows, image.cols, CV_32F);

  for(int r = 0; r < image.rows; r++) {
    for(int c = 0; c < image.cols; c++) {
      return_image.at<float>(r, c) = image.at<uchar>(r, c);
    }
  }

  return return_image;
}


/* Shift image with negative values so that the minimum value is 0.
 * NB: The original Mat should be of type float*/
cv::Mat shift(cv::Mat image) {
  cv::Mat shifted = cv::Mat(image.rows, image.cols, CV_32F);

  double* min_value = (double*)malloc(sizeof(double*));
  double* max_value = (double*)malloc(sizeof(double*));
  minMaxIdx(image, min_value, max_value);
  float min = float(* min_value);
  // int int_max = int(* max_value);

  for(int r = 0; r < image.rows; r++) {
    for(int c = 0; c < image.cols; c++) {
      float pixel = image.at<float>(r, c) - min;
      shifted.at<float>(r, c) = pixel;
    }
  }

  return shifted;
}


/* Scale the values in the mat so that they cover the entire range */
cv::Mat scale(cv::Mat image, int max_scale) {
  cv::Mat scaled = cv::Mat(image.rows, image.cols, CV_32F);

  double* max_value = (double*)malloc(sizeof(double*));
  minMaxIdx(image, NULL, max_value);
  float max = float(* max_value);

  for(int r = 0; r < image.rows; r++) {
    for(int c = 0; c < image.cols; c++) {
      float pixel =  float(max_scale) * image.at<float>(r, c) / max;
      scaled.at<float>(r, c) = pixel;
    }
  }

  return scaled;
}

cv::Mat clip_values(cv::Mat image) {
  cv::Mat clipped = cv::Mat(image.rows, image.cols, CV_8UC1);

  for(int r = 0; r < image.rows; r++) {
    for(int c = 0; c < image.cols; c++) {
      if(image.at<float>(r, c) < 0) {
        clipped.at<uchar>(r, c) = 0;
      } else {
        clipped.at<uchar>(r, c) = uchar(image.at<float>(r, c));
      }
    }
  }

  return clipped;
}

