//Some code borrowed from here: https://stackoverflow.com/questions/27533203/how-do-i-use-sift-in-opencv-3-0-with-c
#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>

int main(int argc, char** argv){
  const std::string file_dir = "/mnt/c/Users/James/Desktop/imgs/";
  const std::string file_prefix = "left";
  const std::string file_suffix = ".jpg";
  const std::string file_regex = "%04d";
  int file_num = 0;
  char file_name[file_dir.length()+file_prefix.length()+4+file_suffix.length()];
  sprintf(file_name, (file_dir+file_prefix+file_regex+file_suffix).c_str(),file_num++);
  
  cv::Mat descriptorHist;
  std::vector<int> matchCountHist;
  cv::Ptr<cv::Feature2D> sifter = cv::xfeatures2d::SIFT::create();
  bool first = true;
  cv::Scalar blueLower(90,90,90);
  cv::Scalar blueUpper(150,255,255);
  
  while(access(file_name,R_OK)==0){
    const std::string img_path = std::string(file_name);
    std::cout<<file_name<<std::endl;
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
    if(contours.size()==0){
      sprintf(file_name, (file_dir+file_prefix+std::string("%04d")+file_suffix).c_str(),file_num++);
      
      continue;
    }
    double maxArea = 0;
    int contIdx = 0;
    std::cout<<contours.size()<<" contours"<<std::endl;
    for(int i=0;i<contours.size();i++){
      if(cv::contourArea(contours[i])>maxArea){
        contIdx = i;
        maxArea=cv::contourArea(contours[i]);
      }
    }
    std::cout<<maxArea<<" max area"<<std::endl;
    if(maxArea<100){
      sprintf(file_name, (file_dir+file_prefix+std::string("%04d")+file_suffix).c_str(),file_num++);
      continue;
    }
    
    //Get bitmask for everything inside the contour
    cv::Mat maskCont(maskB.rows,maskB.cols,CV_8UC1);
    cv::Scalar color(255);
    cv::drawContours(maskCont,contours,contIdx,color,CV_FILLED);

    //keep only the tsuff inside the contour that isn't blue
    cv::Mat maskTemp(maskB.rows,maskB.cols,CV_8UC1);
    maskCont.copyTo(maskTemp,255-maskB);
    cv::Mat mask(maskB.size(),CV_8UC1);
    cv::inRange(maskTemp,254,255,mask); //For some reason, they weren't all zero or 255

    cv::Mat imgGrayCont(imgGray.rows,imgGray.cols,CV_8UC1);
    imgGray.copyTo(imgGrayCont,mask);

    // imshow("imgGrayCont",imgGrayCont);
    // cv::waitKey(0);
    
    //-- Step 1: Detect the keypoints:
    std::vector<cv::KeyPoint> keypoints;    
    sifter->detect( imgGrayCont, keypoints, mask);
    //-- Step 2: Calculate descriptors (feature vectors)    
    cv::Mat descriptors;    
    sifter->compute( imgGrayCont, keypoints, descriptors );
    if(first){
      descriptorHist.create(descriptors.size(),descriptors.type());
      for(int i=0;i<descriptors.rows;i++){
        matchCountHist.push_back(1);
        descriptors.row(i).copyTo(descriptorHist.row(i));
      }
      first = false;
      sprintf(file_name, (file_dir+file_prefix+std::string("%04d")+file_suffix).c_str(),file_num++);
      continue;
    }
    //-- Step 3: Matching descriptor vectors using BFMatcher :
    cv::BFMatcher matcher;
    std::vector< cv::DMatch > matches;
    matcher.match( descriptors, descriptorHist, matches );
    // Add on new descriptors
    int matched[descriptors.rows];
    for(int i=0;i<descriptors.rows;i++){
      matched[i] = 0;
    }

    for(int i=0;i<matches.size();i++){
      matchCountHist[matches[i].trainIdx]++;//or queryIdx or imgIdx
      matched[matches[i].queryIdx] = 1;
    }
    volatile int unmatched = descriptors.rows;
    for(int i=0;i<descriptors.rows;i++){
      unmatched -= matched[i];
    }
    

    int oldSize = descriptorHist.rows;
    if(unmatched>0){
      descriptorHist.resize(descriptorHist.rows+unmatched,descriptorHist.cols);
      int copied = 0;
      for(int i=0;i<descriptors.rows;i++){
        if(matched[i]==0){
          matchCountHist.push_back(0);
          descriptors.row(i).copyTo(descriptorHist.row(oldSize + copied++));
        }
      }
    }
    sprintf(file_name, "/mnt/c/Users/James/Desktop/imgs/left%04d.jpg",file_num++);
  }

  std::ofstream countLogger, descriptorLogger;
  countLogger.open("/mnt/c/Users/James/Desktop/imgs/counts.txt",std::ofstream::out);
  descriptorLogger.open("/mnt/c/Users/James/Desktop/imgs/descriptors.txt",std::ofstream::out);
  for(int i=0;i<matchCountHist.size();i++){
    std::cout<<matchCountHist[i]<<"\t";
    countLogger<<matchCountHist[i];
    
    for(int j=0;j<descriptorHist.cols;j++){
      descriptorLogger<<descriptorHist.at<float>(i,j);
      if(j<descriptorHist.cols-1){
        descriptorLogger<<",";
      }
    }

    if(i<matchCountHist.size()-1){
      countLogger<<",";
      descriptorLogger<<std::endl;
    }
  }
  countLogger.close();
  descriptorLogger.close();
  std::cout<<std::endl;
  
}








