#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

int main(int argc, char** argv){
  std::string file_dir = "/mnt/c/Users/James/Desktop/imgs";
  std::string file_prefix = "left";
  std::string file_suffix = ".jpg";
  int file_num = 0;
  char file_name[file_dir.length()+file_prefix.length()+4+file_suffix.length()];
  sprintf(file_name, (file_dir+file_prefix+std::string("%04d")+file_suffix).c_str(),file_num++);
  while(access(file_name,F_OK)){
    cv::Mat img = cv::imread(file_name,CV_LOAD_IMAGE_COLOR);//Probably BGR
    



    sprintf(file_name, (file_dir+file_prefix+std::string("%04d")+file_suffix).c_str(),file_num++);
  }

}








