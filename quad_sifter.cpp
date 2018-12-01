#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <math.h>

int main(int argc, char** argv){
  string file_dir = "/mnt/c/Users/James/Desktop/imgs";
  string file_prefix = "left";
  string file_suffix = ".jpg"
  int numPics = 
  for(int count = 0;count<numPics;count++){
    string file_num = 
    string file_name = file_dir+file_prefix+file_num+file_suffix;
    cv::Mat img = cv::imread(file_name,CV_LOAD_IMAGE_COLOR);//Probably BGR
    
  }

}








