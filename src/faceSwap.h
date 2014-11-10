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
#include "Clone.h"

class faceSwap{
public:
    ofxFaceTrackerThreaded camTracker;
    
    ofxFaceTrackerThreaded srcTracker;
    ofImage src;
    std::vector<ofVec2f> srcPoints;
    
    bool cloneReady;
    Clone clone;
    ofFbo srcFbo, maskFbo;
    
    ofDirectory faces;
    int currentFace;
    
    void loadFace(string face);
    
    void setup();
    void update();
    void draw();
};

#endif /* defined(__modulab_0_1__faceSwapper__) */
