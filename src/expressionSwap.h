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
    
    ofxFaceTrackerThreaded tracker;
    ofxFaceTrackerThreaded trackerSecond;
    
    ofFbo mouthFbo, mouthMaskFbo;
    ofShader maskShader;
    
    void setup();
    void update();
    void draw();
};
#endif /* defined(__modulab_0_1__expressionSwap__) */
