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
#include "imageSave.h"

class expressionSwap {
public:
    
    ofxFaceTrackerThreaded trackerSource;
    ofxFaceTrackerThreaded trackerDest;
    
    ofFbo sourceFaceFbo, mouthMaskFbo, destinationFaceFbo, cloneMask;//, finalSourceMouth;
    ofShader maskShader;
    ofTessellator tess;
    
    Clone clone;
    
    imageSave imageSaver;
    
    int x,y;
    
    void setup();
    void update(cv::Mat& source, cv::Mat& dest);
    void draw(ofTexture& source, ofTexture& destination);
    
    void keyPressed(int key);
};
#endif /* defined(__modulab_0_1__expressionSwap__) */
