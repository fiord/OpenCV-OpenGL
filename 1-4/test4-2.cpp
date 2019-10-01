#include <opencv2/opencv.hpp>
#include <cstring>
#include <stdio.h>

int main(int argc, char *argv[])
{
  printf("usage: ./test3 [-b bg_img] (device_num|movie_file)\n");
  double B_PARAM = 1.0 / 50.0;
  double T_PARAM = 1.0 / 200.0;
  double Zeta = 10.0;

  cv::VideoCapture cap;
  cv::Mat frame, prev_frame;
  cv::Mat dst_img, msk_img, tmp_img, diff_img;
  cv::Mat bg_img, inv_msk_img, bg_msked;

  // 1. initialize VideoCapture
  bool cap_opened = false;
  for(int i = 1; i < argc; i++) {
    printf("argv[%d] = %s\n", i, argv[i]);
    if (strcmp(argv[i], "-b") == 0 && argc > i + 1) {
      i++;
      printf("start to load bg file: %s\n", argv[i]);
      bg_img = cv::imread(argv[i]);
      printf("background image is loaded\n");
    }
    else {
      char *in_name = argv[i];
      if('0' <= in_name[0] && in_name[0] <= '9' && in_name[1] == '\0'){
        cap.open(in_name[0] - '0');  // open with device id
      }else{
        cap.open(in_name);  // open with filename
      }
      cap_opened = true;
      printf("capture device is loaded\n");
    }
  }
  if (!cap_opened)  cap.open(0);
  if(!cap.isOpened()){
    printf("Cannot open the video.\n");
    exit(0);
  }

  // 2. prepare window for showing images
  cv::namedWindow("Input", 1);
  cv::namedWindow("FG", 1);
  cv::namedWindow("mask", 1);

  // 3. calculate initial value of background
  cap >> frame;

  cv::Size s = frame.size();
  prev_frame = frame.clone();
  dst_img.create(s, CV_8UC3);
  tmp_img.create(s, CV_8UC3);
  bg_msked.create(s, CV_8UC3);
  msk_img.create(s, CV_8UC1);
  inv_msk_img.create(s, CV_8UC1);

  frame.convertTo(prev_frame, prev_frame.type());

  bool loop_flag = true;
  while(loop_flag){
    cap >> frame;
    if (frame.empty()) {
      break;
    }

    cv::absdiff(frame, prev_frame, tmp_img);
    cv::cvtColor(tmp_img, diff_img, CV_BGR2GRAY);
    cv::threshold(diff_img, diff_img, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    cv::morphologyEx(diff_img, diff_img, cv::MORPH_OPEN, cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5)));

    // 基本的な考察:左の方の動いていると検出された点はより右側にも動いている点がある(つまりその間を塗りつぶす)
    // ただし，以下の実装では対象となるオブジェクトが凸で1つのみであることが前提になっている
    for (int i = 0; i < s.height; i++) {
      unsigned char *ptr = diff_img.ptr<unsigned char>(i);
      for (int j = 0; j < s.width; j++) {
        if (*(ptr + j) && j + 1 < s.width) {
          // 右側に白があるか確認
          if (*(ptr + j + 1)) continue;
          else {
            int end = j + 2;
            while(end < s.width && !*(ptr + end)) end++;
            if (end < s.width) {
              // 間を埋める
              while(j < end) {
                *(ptr + j) = 255;
                j++;
              }
            }
          }
        }
      }
    }
    // 縦にも
    for (int i = 0; i < s.width; i++) {
      for (int j = 0; j < s.height; j++) {
        if (diff_img.ptr<unsigned char>(j)[i]) {
          if (j + 1 < s.height && diff_img.ptr<unsigned char>(j+1)[i])  continue;
          else {
            int end = j + 2;
            while(end < s.height && !diff_img.ptr<unsigned char>(end)[i])  end++;
            if (end < s.height) {
              while(j < end) {
                *(diff_img.ptr<unsigned char>(j++) + i) = 255;
              }
            }
          }
        }
      }
    }

    diff_img.copyTo(msk_img);
    dst_img =  cv::Scalar(0);
    frame.copyTo(dst_img, msk_img);

    if (!bg_img.empty()) {
      for (int i = 0; i < s.height; i++) {
        unsigned char *ptr = dst_img.ptr<unsigned char>(i);
        unsigned char *ptr2 = bg_img.ptr<unsigned char>(i);
        for (int j = 0; j < s.width; j++) {
          if (msk_img.at<unsigned char>(i, j)) {
            continue;
          }
          else {
            for (int k = 0; k < 3; k++) {
              *(ptr + 3 * j + k) = *(ptr2 + 3 * j + k);
            }
          }
        }
      }
    }

    cv::imshow("Input", frame);
    cv::imshow("mask", msk_img);
    cv::imshow("FG", dst_img);

    prev_frame = frame.clone();

    char key =cv::waitKey(10);
    if(key == 27){
      loop_flag = false;
    }
  }
  return 0;
}
