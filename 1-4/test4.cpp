#include <opencv2/opencv.hpp>
#include <cstring>
#include <stdio.h>

int main(int argc, char *argv[])
{
  printf("usage: ./test3 [-b bg_img] (device_num|movie_file)\n");
  int INIT_TIME = 50;
  double B_PARAM = 1.0 / 50.0;
  double T_PARAM = 1.0 / 200.0;
  double Zeta = 10.0;

  cv::VideoCapture cap;
  cv::Mat frame;
  cv::Mat avg_img, sgm_img;
  cv::Mat lower_img, upper_img, tmp_img;
  cv::Mat dst_img, msk_img;
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
  printf("OK\n");

  // 2. prepare window for showing images
  cv::namedWindow("Input", 1);
  cv::namedWindow("FG", 1);
  cv::namedWindow("mask", 1);

  // 3. calculate initial value of background
  cap >> frame;

  cv::Size s = frame.size();

  avg_img.create(s, CV_32FC3);
  sgm_img.create(s, CV_32FC3);
  lower_img.create(s, CV_32FC3);
  upper_img.create(s, CV_32FC3);
  tmp_img.create(s, CV_32FC3);

  dst_img.create(s, CV_8UC3);
  msk_img.create(s, CV_8UC1);

  inv_msk_img.create(s, CV_8UC1);
  bg_msked.create(s, CV_8UC1);

  printf("Background statistics initialization start\n");

  avg_img = cv::Scalar(0, 0, 0);

  for( int i = 0; i < INIT_TIME; i++){
    cap >> frame;
    cv::Mat tmp;
    frame.convertTo(tmp, avg_img.type());
    cv::accumulate(tmp, avg_img);
  }

  avg_img.convertTo(avg_img, -1, 1.0 / INIT_TIME);
  sgm_img = cv::Scalar(0, 0, 0);

  for( int i = 0; i < INIT_TIME; i++){
    cap >> frame;
    frame.convertTo(tmp_img, avg_img.type());
    cv::subtract(tmp_img, avg_img, tmp_img);
    cv::pow(tmp_img, 2.0, tmp_img);
    tmp_img.convertTo(tmp_img, -1, 2.0);
    cv::sqrt(tmp_img, tmp_img);
    cv::accumulate(tmp_img, sgm_img);
  }

  sgm_img.convertTo(sgm_img, -1, 1.0 / INIT_TIME);

  printf("Background statistics initialization finish\n");

  bool loop_flag = true;
  while(loop_flag){
    cap >> frame;
    if (frame.empty()) {
      break;
    }

    frame.convertTo(tmp_img, tmp_img.type());

    // 4. check whether pixels are background or not
    cv::subtract(avg_img, sgm_img, lower_img);
    cv::subtract(lower_img, Zeta, lower_img);
    cv::add(avg_img, sgm_img, upper_img);
    cv::add(upper_img, Zeta, upper_img);
    cv::inRange(tmp_img,lower_img, upper_img,msk_img);

    // 5. recalculate
    cv::subtract(tmp_img, avg_img, tmp_img);
    cv::pow(tmp_img, 2.0, tmp_img);
    tmp_img.convertTo(tmp_img, -1, 2.0);
    cv::sqrt(tmp_img, tmp_img);
    
    // 6. renew avg_img and sgm_img
    cv::accumulateWeighted(frame, avg_img, B_PARAM, msk_img);
    cv::accumulateWeighted(tmp_img, sgm_img, B_PARAM, msk_img);

    cv::bitwise_not(msk_img, msk_img);
    cv::accumulateWeighted(tmp_img, sgm_img, T_PARAM, msk_img);

    dst_img = cv::Scalar(0);
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

    char key =cv::waitKey(10);
    if(key == 27){
      loop_flag = false;
    }
  }
  return 0;
}
