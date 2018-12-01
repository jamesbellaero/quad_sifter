//Some code borrowed from here: https://stackoverflow.com/questions/27533203/how-do-i-use-sift-in-opencv-3-0-with-c
#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <string>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <vector>

int main(int argc, char** argv){
  std::string file_dir = "/mnt/c/Users/James/Desktop/imgs";
  std::string file_prefix = "left";
  std::string file_suffix = ".jpg";
  int file_num = 0;
  char file_name[file_dir.length()+file_prefix.length()+4+file_suffix.length()];
  sprintf(file_name, (file_dir+file_prefix+std::string("%04d")+file_suffix).c_str(),file_num++);
  
  cv::Mat descriptorHist;
  std::vector<int> matchCountHist;
  cv::Ptr<cv::Feature2D> sifter = xfeatures2d::SIFT::create();
  bool first;
  cv::Scalar blueLower(100,100,100);
  cv::Scalar blueUpper(140,255,255);

  while(access(file_name,F_OK)){
    const std::string img_path = std::string(file_name);
    cv::Mat img = cv::imread(img_path);//Probably BGR
    /*DO IMAGE PROCESSING HERE*/
    cv::Mat imgHsv, imgGray, maskB;
    cv::cvtColor(img,imgHsv,cv::COLOR_BGR2HSV);
    cv::cvtColor(img,imgGray,cv::COLOR_BGR2GRAY);
    //Find region of image that's blue and get a bitmask
    cv::inRange(imgHsv,blueLower,blueUpper,maskB);

    //Get max contour for blue area
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(maskB,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);
    double maxArea = 0;
    int contIdx;
    for(int i=0;i<contours.size();i++){
      if(cv::contourArea(contours[i])>maxArea){
        contIdx;
        maxArea=cv::contourArea(contours[i]);
      }
    }
    //Get bitmask for everything inside the contour
    cv::Mat maskCont(maskB.rows,maskB.cols,CV_8UC1);
    cv::Scalar color(255);
    cv::drawContours(maskCont,contours,contIdx,color,CV_FILLED,cv::LineTypes::FILLED);

    //keep only the tsuff inside the contour that isn't blue
    cv::Mat mask(maskB.rows,maskB.cols,CV_8UC1);
    maskCont.copyTo(mask,255-maskB);

    cv::Mat imgGrayCont(imgGray.rows,imgGray.cols,CV_8UC1);
    imgGray.copyTo(imgGrayCont,mask);
    
    //-- Step 1: Detect the keypoints:
    std::vector<cv::KeyPoint> keypoints;    
    sifter->detect( img, keypoints, mask);

    //-- Step 2: Calculate descriptors (feature vectors)    
    cv::Mat descriptors;    
    sifter->compute( img, keypoints, descriptors );
    if(first){
      for(int i=0;i++;i<descriptors.size()){
        matchCountHist.push_back(0);
        descriptorHist.push_back(descriptors.at(i));
      }
      first = false;
      continue;
    }
    //-- Step 3: Matching descriptor vectors using BFMatcher :
    cv::BFMatcher matcher;
    std::vector< cv::DMatch > matches;
    matcher.match( descriptorHist, descriptors, matches );
    // Add on new descriptors
    int matched[descriptors.size()];
    for(int i=0;i<matches.size();i++){
      matchCountHist[matches[i].trainIdx]++;//or queryIdx or imgIdx
      matched[matches[i].queryIdx] = 1;
    }
    for(int i=0;i<descriptors.size();i++){
      if(matched[i]==0){
        matchCountHist.push_back(0);
        descriptorHist.push_back(descriptors.at(i));
      }
    }


    sprintf(file_name, (file_dir+file_prefix+std::string("%04d")+file_suffix).c_str(),file_num++);
  }
  for(int i=0;i<matchCountHist.size();i++){
    std::cout<<matchCountHist[i]<<"\t";
  }
  std::cout<<std::endl;

}








