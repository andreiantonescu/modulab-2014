//
//  expressionSwap.h
//  modulab_0_1
//
//  Created by Andrei Antonescu on 09/11/14.
//
//

#ifndef __modulab_0_1__expressionSwap__
#define __modulab_0_1__expressionSwap__

#include <iostream>
#include "genInclude.h"

class expressionSwap {
public:
    
    ofxFaceTrackerThreaded trackerSource;
    ofxFaceTrackerThreaded trackerDest;
    
    ofFbo mouthFbo, mouthMaskFbo;
    ofShader maskShader;
    
    void setup();
    void update(cv::Mat& source, cv::Mat& dest);
    void draw(cv::Mat& frame, ofImage& destImage);
    
    void keyPressed(int key);
};
#endif /* defined(__modulab_0_1__expressionSwap__) */
