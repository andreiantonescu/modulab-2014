//
//  faceTracker.h
//  modulab_0_1
//
//  Created by Andrei Antonescu on 09/11/14.
//
//

#ifndef __modulab_0_1__faceTracker__
#define __modulab_0_1__faceTracker__

#include <iostream>

#include "ofApp.h"

class faceTracker{
public:
    
//    preprocessing 
    cv::Mat initialFramePreproc(const cv::Mat& image);
    cv::Mat correctGamma(const cv::Mat& img,const unsigned int& gamma );
    
//    tracker
    ofxFaceTrackerThreaded tracker;
    
//    tracker details
    ofVec2f position;
    float scale;
    ofVec3f orientation;
    ofMatrix4x4 rotationMatrix;
    
    void setup();
    void update(cv::Mat& frame);
};

#endif /* defined(__modulab_0_1__faceTracker__) */
