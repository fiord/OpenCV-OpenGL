#include <opencv2/opencv.hpp>
#include <cmath>
#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>

#define FLAG 1 // (0: direct access / 1: built-in function)

char *preset_file = "fruits.jpg";

// gray scale
void convertColorToGray(cv::Mat &input, cv::Mat &processed);
// blur
void convertBlur(cv::Mat &input, cv::Mat &processed);
// edge
void convertEdge(cv::Mat &input, cv::Mat &processed);

int main(int argc, char *argv[])
{
  char *input_file;
  // 1. prepare Mat objects for input-image and output-image
  cv::Mat input, gray, processed;

  if(argc == 2){
    input_file = argv[1];
  }
  else{
    input_file = preset_file;
  }

  // 2. read an image from the specified file
  input = cv::imread(input_file, 1);
  if(input.empty()){
    fprintf(stderr, "cannot open %s\n", input_file);
    exit(0);
  }
  
  convertColorToGray(input, gray);

  std::string command;
  std::cout << "Press convert number!" << std::endl;
  std::cout << "1: gray scale" << std::endl;
  std::cout << "2: blur(grayed)" << std::endl;
  std::cout << "3: edge(grayed)" << std::endl;
  std::cin >> command;

  if (command == "1") {
    processed = gray;
  }
  else if (command == "2") {
    convertBlur(gray, processed);
  }
  else if (command == "3") {
    convertEdge(gray, processed);
  }

  // 5. create windows
  cv::namedWindow("original image", 1);
  cv::namedWindow("processed image", 1);

  // 6. show images
  cv::imshow("original image", input);
  cv::imshow("processed image", processed);

  // 7. wait key input
  cv::waitKey(0);

  // 8. save the processed result
  cv::imwrite("processed.jpg", processed);

  return 0;
}


void convertColorToGray(cv::Mat &input, cv::Mat &processed)
{
#if FLAG // use built-in function

  //4. convert color to gray
  cv::Mat temp;
  std::vector<cv::Mat> planes;
  cv::cvtColor(input, temp, CV_BGR2YCrCb);
  cv::split(temp, planes);
  processed = planes[0];

#else

  // 3. create Mat for output-image
  cv::Size s = input.size();
  processed.create(s, CV_8UC1);

  for(int j = 0; j < s.height; j++){
    uchar *ptr1, *ptr2;
    ptr1 = input.ptr<uchar>(j);
    ptr2 = processed.ptr<uchar>(j);

    //4. convert color to gray
    for(int i = 0; i < s.width; i++){
      double y = 0.114 * ((double)ptr1[0]) + 0.587 * ((double)ptr1[1]) + 0.299 * ((double)ptr1[2]);

      if(y > 255) y = 255;
      if(y < 0) y = 0;

      *ptr2 = (uchar)y;
      ptr1 += 3;
      ptr2++;
    }
  }
#endif
}

void convertBlur(cv::Mat &input, cv::Mat &processed) {
#if FLAG // use built-in function
  cv::Mat tmp;
  std::vector<cv::Mat> planes;
  cv::GaussianBlur(input, tmp, cv::Size(15,15), 7);
  cv::split(tmp, planes);
  processed = planes[0];
#else

  cv::Size s = input.size();
  processed.create(s, CV_8UC1);

  for (int i = 0 ; i < s.height; i++) {
    unsigned char *ptr = processed.ptr<unsigned char>(i);
    for (int j = 0; j < s.width; j++) {
      int cnt = 0;
      int val = 0;
      for (int y = std::max(0, i - 5); y < std::min(s.height, i + 6); y++) {
        for (int x = std::max(0, j - 5); x < std::min(s.width, j + 6); x++) {
          cnt++;
          val += input.at<unsigned char>(y, x);
        }
      }
      val /= cnt;
      *ptr = (unsigned char)val;
      ptr++;
    }
  }

#endif
}

void convertEdge(cv::Mat &input, cv::Mat &processed) {
#if FLAG // use built-in function
  cv::Mat tmp_x, tmp_y, tmp, tmp_edged;
  std::vector<cv::Mat> planes;
  cv::Sobel(input, tmp_x, CV_64F, 1, 0, 3);
  cv::Sobel(input, tmp_y, CV_64F, 0, 1, 3);
  tmp = abs(tmp_x) + abs(tmp_y);
  cv::convertScaleAbs(tmp, tmp, 0.5, 0);
  cv::threshold(tmp, tmp_edged, 0,255, cv::THRESH_BINARY|cv::THRESH_OTSU);
  cv::split(tmp_edged, planes);
  processed = planes[0];

#else

  cv::Size s = input.size();
  cv::Mat tmp, differ;
  convertBlur(input, tmp);
  differ.create(s, CV_8UC1);
  
  for (int i = 0; i < s.height; i++) {
    unsigned char *ptr = differ.ptr<unsigned char>(i);
    for (int j = 0; j < s.width; j++) {
      int cnt = 0;
      int val = 0;
      for (int y = std::max(0, i - 1); y < std::min(s.height, i + 2); y++) {
        for (int x = std::max(0, j - 1); x < std::min(s.width, j + 2); x++) {
          if (y == i && x == j) continue;
          cnt++;
          val += tmp.at<unsigned char>(y, x);
        }
      }
      *ptr = (unsigned char)(cnt * tmp.at<unsigned char>(i, j) - val);
      ptr++;
    }
  }

  processed = differ;
#endif
}
