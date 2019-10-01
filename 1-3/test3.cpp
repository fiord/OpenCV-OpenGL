#include <opencv2/opencv.hpp>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#define IN_VIDEO_FILE "sample_video_input.avi"
#define OUT_VIDEO_FILE "sample_video_output.avi"

int main(int argc, char *argv[]){

  // 1. prepare VideoCapture Object
  cv::VideoCapture cap;
  std::string input_index;
  if(argc > 1){ // capture from video file
    input_index = argv[1];
    cap.open(input_index);
  }else{ // capture from camera
    cap.open(0);
  }

  // 2. prepare VideoWriter Object
  cv::Mat frame, copy_frame;
  int rec_mode= 0;

  cv::namedWindow("video", 1);
  cv::VideoWriter output_video;
  output_video.open(OUT_VIDEO_FILE, CV_FOURCC('M','J','P','G'), 30, cv::Size(640, 480));
  /* using "MJPG" as the video codec */

  if(!cap.isOpened() || !output_video.isOpened()){
    printf("no input video\n");
    return 0;
  }
  else
  {
    bool loop_flag = true;
    while(loop_flag){

      // 3. capture frame from VideoCapture
      cap >> frame;
      if(frame.empty()){
        break;
      }

      // 4. save frame
      if(rec_mode){
        output_video << frame;
        frame.copyTo(copy_frame);
        cv::Size s = frame.size();
        cv::rectangle(copy_frame, cv::Point(0,0),
          cv::Point(s.width-1, s.height-1), cv::Scalar(0,0,255),4,8,0);
        cv::imshow("video",copy_frame);
      }
      else{
        cv::imshow("video",frame);
      }

      // 5. process according to input key
	  int k = cv::waitKey(33);
      switch(k){
      case 'q':
      case 'Q':
        loop_flag = false;
        break;
      case 'p':
        if(rec_mode == 0) {
          printf("play mode:\n"
              "q or Q: stop playing\n");
          system("cp " OUT_VIDEO_FILE " " IN_VIDEO_FILE);
          cv::VideoCapture play_cap(IN_VIDEO_FILE);
          if (!play_cap.isOpened()) {
            printf("error: failed to open video file\n");
          }
          else {
            while(1) {
              cv::Mat play_frame;
              play_cap >> play_frame;
              if (play_frame.empty()) {
                break;
              }
              // print frame
              cv::Size s = frame.size();
              cv::rectangle(play_frame, cv::Point(0, 0),
                  cv::Point(s.width-1, s.height-1), cv::Scalar(255, 0, 0), 4, 8, 0);
              cv::imshow("video", play_frame);
              
              char play_k = cv::waitKey(33);
              if (play_k == 'q' || play_k == 'Q') {
                break;
              }
            }
          }
        }
        break;
      case 'r':
        if(rec_mode ==0){
          rec_mode = 1;
        }else{
          rec_mode = 0;
        }
        break;
      }
    }
  }
  return 0;
}
