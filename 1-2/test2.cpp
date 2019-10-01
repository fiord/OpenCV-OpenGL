#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <cmath>
#include <algorithm>

#define PAINT_MODE 0
#define AREA_MODE 1
int current_mode = PAINT_MODE;

cv::Mat inpaint_mask, inpainted;
cv::Mat original_image, whiteLined_image, select_base_image;

cv::Point base, base2;

void initBasePos() {
  base = cv::Point(-1, -1);
  base2 = cv::Point(-1, -1);
}

void myMouseEventHandler(int event, int x , int y , int flags, void *){
  if(whiteLined_image.empty()){
    return;
  }

  if (current_mode == PAINT_MODE) {
    static bool isBrushDown = false;
    static cv::Point prevPt;
    cv::Point pt(x, y);

    bool isLButtonPressedBeforeEvent = (bool)(flags & CV_EVENT_FLAG_LBUTTON);
    if(isLButtonPressedBeforeEvent && isBrushDown){
      cv::line(inpaint_mask, prevPt, pt, cv::Scalar(255), 5, 8, 0);
      cv::line(whiteLined_image, prevPt, pt, cv::Scalar::all(255), 5, 8, 0);
      cv::imshow("image", whiteLined_image);
    }

    // The XOR below means, isLButtonPressedAfterEvent
    // is usualy equal to isLButtonPressedBeforeEvent,
    // but not equal if the event is mouse down or up.
    bool isLButtonPressedAfterEvent = isLButtonPressedBeforeEvent
      ^ ((event == CV_EVENT_LBUTTONDOWN) || (event == CV_EVENT_LBUTTONUP));
    if(isLButtonPressedAfterEvent){
      prevPt = pt;
      isBrushDown = true;
    }else{
      isBrushDown = false;
    }
  }
  else if (current_mode == AREA_MODE) {
    static bool isMouseDown = (base != cv::Point(-1, -1));
    cv::Point pt(x, y);

    bool isLButtonPressedBeforeEvent = (bool)(flags & CV_EVENT_FLAG_LBUTTON);
    if (isMouseDown && isLButtonPressedBeforeEvent) {
      whiteLined_image = select_base_image.clone();
      base2 = pt;
      cv::line(whiteLined_image, base, cv::Point(base.x, y), CV_RGB(255, 0, 0));
      cv::line(whiteLined_image, cv::Point(base.x, y), pt, CV_RGB(255, 0, 0));
      cv::line(whiteLined_image, pt, cv::Point(x, base.y), CV_RGB(255, 0, 0));
      cv::line(whiteLined_image, cv::Point(x, base.y), base, CV_RGB(255, 0, 0));
      cv::imshow("image", whiteLined_image);
    }

    bool isLButtonPressedAfterEvent = isLButtonPressedBeforeEvent
      ^ ((event == CV_EVENT_LBUTTONDOWN) || (event == CV_EVENT_LBUTTONUP));
    if (isLButtonPressedAfterEvent) {
      if (!isMouseDown) {
        base = base2 = cv::Point(-1, -1);
      }
      if (base == cv::Point(-1, -1))  base = pt;
      isMouseDown = true;
    }
    else {
      isMouseDown = false;
    }
  }
}

int main(int argc, char *argv[]){
  initBasePos();

  // 1. read image file
  char *filename = (argc >= 2) ? argv[1] : (char *)"fruits.jpg";
  original_image = cv::imread(filename);
  if(original_image.empty()){
    printf("ERROR: image not found!\n");
    return 0;
  }

  //print hot keys
  printf( "Hot keys: \n"
      "\tESC - quit the program\n"
      "\ts - change to area select mode\n"
      "\tb - bitwise not in selected area(only in area select mode and area have been already selected\n"
      "\ti or ENTER - run inpainting algorithm\n"
      "\t\t(before running it, paint something on the image)\n");

  // 2. prepare window
  cv::namedWindow("image",1);

  // 3. prepare Mat objects for processing-mask and processed-image
  whiteLined_image = original_image.clone();
  inpainted = original_image.clone();
  inpaint_mask.create(original_image.size(), CV_8UC1);

  inpaint_mask = cv::Scalar(0);
  inpainted = cv::Scalar(0);

  // 4. show image to window for generating mask
  cv::imshow("image", whiteLined_image);

  // 5. set callback function for mouse operations
  cv::setMouseCallback("image", myMouseEventHandler, 0);

  bool loop_flag = true;
  while(loop_flag){

    // 6. wait for key input
    int c = cv::waitKey(0);

    // 7. process according to input
    switch(c){
      case 27://ESC
      case 'q':
        loop_flag = false;
        break;

      case 'r':
        inpaint_mask = cv::Scalar(0);
        original_image.copyTo(whiteLined_image);
        cv::imshow("image", whiteLined_image);
        break;

      case 's':
        // area select
        if (current_mode == PAINT_MODE) {
          current_mode = AREA_MODE;
          select_base_image = whiteLined_image.clone();
        }
        else if (current_mode == AREA_MODE) {
          current_mode = PAINT_MODE;
          whiteLined_image = select_base_image.clone();
          initBasePos();
        }
        cv::imshow("image", whiteLined_image);
        break;

      case 'b':
        // bitwise not
        if (current_mode == AREA_MODE && base != cv::Point(-1, -1) && base2 != cv::Point(-1, -1)) {
          printf("%d, %d\n", std::min(base.x, base2.x), std::max(base.x, base2.x));
          for (int i = std::min(base.y, base2.y); i <= std::max(base.y, base2.y); i++) {
            unsigned char *ptr1 = select_base_image.ptr<unsigned char>(i);
            unsigned char *ptr2 = inpaint_mask.ptr<unsigned char>(i);
            for (int j = std::min(base.x, base2.x); j <= std::max(base.x, base2.x); j++) {
              for (int k = 0; k < 3; k++) {
                unsigned char val = *(ptr1+3*j+k);
                *(ptr1 + 3 * j + k) = ~val;
                *(ptr2 + 3 * j + k) = ~val;
              }
            }
          }
          cv::imshow("image", select_base_image);
        }
        else {
          printf("error\n");
        }
        break;

      case 'i':
      case 10://ENTER
        cv::namedWindow("inpainted image", 1);
        cv::inpaint(whiteLined_image, inpaint_mask, inpainted, 3.0, cv::INPAINT_TELEA);
        cv::imshow("inpainted image", inpainted);
        break;
    }
  }
  return 0;
}
