#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <map>
#include <utility>
#include <chrono>

int size_of_mosaic = 0;

int main(int argc, char *argv[])
{
  // for record
  int time_cnt = 0;
  double avg_time = 0;
  std::chrono::system_clock::time_point start_time, end_time;

  // 1. load classifier
  std::string cascadeName = "haarcascade_frontalface_alt.xml"; //Haar-like
  cv::CascadeClassifier cascade;
  if(!cascade.load(cascadeName)){
    printf("ERROR: cascadeFile not found\n");
    return -1;
  }

  // 2. initialize VideoCapture
  cv::Mat frame;
  cv::VideoCapture cap;
  cap.open(0);
  cap >> frame;

  // 3. prepare window and trackbar
  cv::namedWindow("result", 1);

  double scale = 4.0;
  cv::Mat gray, smallImg(cv::saturate_cast<int>(frame.rows / scale),
      cv::saturate_cast<int>(frame.cols / scale), CV_8UC1);

  for(;;){

    // 4. capture frame
    cap >> frame;
    //convert to gray scale
    cv::cvtColor(frame, gray,CV_BGR2GRAY);

    // 5. scale-down the image
    cv::resize(gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR);
    cv::equalizeHist(smallImg, smallImg);

    // 6. detect face using Haar-classifier
    std::vector<cv::Rect> faces;
    ///multi-scale face searching
    // image, size, scale, num, flag, smallest rect
    // and time record
    start_time = std::chrono::system_clock::now();
    cascade.detectMultiScale(smallImg, faces,
                    1.1,
                    10,
                    CV_HAAR_SCALE_IMAGE,
                    cv::Size(30, 30));
    end_time = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
    avg_time = (avg_time * time_cnt + elapsed) / (time_cnt + 1);
    time_cnt++;

    // 7. mosaic(pixelate) face-region
    for(int i = 0; i < faces.size(); i++){
      cv::Mat dst = frame(cv::Rect(faces[i].x * scale, faces[i].y * scale, faces[i].width * scale, faces[i].height * scale));
      cv::blur(dst, dst, cv::Size(30, 30));
    }

    // 8. show mosaiced image to window
    cv::imshow("result", frame);

    int key = cv::waitKey(10);
    if(key == 'q' || key == 'Q')
        break;

  }
  printf("avg time:%.lf\n", avg_time);
 return 0;
}
