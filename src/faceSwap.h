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
    
    cv::Mat src;
    std::vector<ofVec2f> srcPoints;
    
    bool cloneReady;
    Clone clone;
    ofFbo srcFbo, maskFbo;
    
    void setup(); // to change, should not receive face path - just for testing
    void updateWithMesh(ofTexture& destFace, ofImage& image, ofxFaceTrackerThreaded& destTracker);
    void draw();
};

#endif /* defined(__modulab_0_1__faceSwapper__) */
