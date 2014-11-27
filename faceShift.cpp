//
//  faceShift.cpp
//  modulab_0_1
//
//  Created by Andrei Antonescu on 25/11/14.
//
//

#include "faceShift.h"

void faceShift::setup(){
    srcTracker.setup();
}

void faceShift::update(){
    cv::Mat src = cv::imread("/Users/andreiantonescu/Desktop/average.jpg");
    imshow("test",src);
    srcTracker.update(src);
    cout<<srcTracker.getFound();
}