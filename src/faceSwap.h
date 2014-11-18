//
//  faceSwapper.h
//  modulab_0_1
//
//  Created by Andrei Antonescu on 09/11/14.
//
//

#ifndef __modulab_0_1__faceSwapper__
#define __modulab_0_1__faceSwapper__

#include <iostream>
#include "genInclude.h"

class faceSwap{
public:
    ofxFaceTrackerThreaded camTracker;
    ofxFaceTrackerThreaded srcTracker;
    
    ofImage src;
    std::vector<ofVec2f> srcPoints;
    
    bool cloneReady;
    Clone clone;
    ofFbo srcFbo, maskFbo;
    
    void setup(string directory);
    void update(cv::Mat& video, cv::Mat& faceSource,ofVideoGrabber &cam);
    void draw();
};

#endif /* defined(__modulab_0_1__faceSwapper__) */
