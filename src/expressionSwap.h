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
#include "faceSwap.h"

class expressionSwap {
public:
    
    ofxFaceTrackerThreaded trackerSource;
    ofxFaceTrackerThreaded trackerDest;
    ofxFaceTrackerThreaded trackerNonSmiling;
    ofxFaceTrackerThreaded savedTracker;
    
    cv::Mat srcSaved;
    
    ofFbo sourceFaceFbo, mouthMaskFbo, destinationFaceFbo, cloneMask, cloneSecondMask,destinationFaceMaskFbo, nonSmilingFbo, mouthMaskFboBlurred;
    ofFbo cloneSecondFbo;
    ofShader maskShader;
    ofTessellator tess;
    
    ofxGaussianBlur gaussianBlur;
    
    ofMesh camMesh;
    
    Clone clone, cloneSecond;
    faceSwap faceSwapper;
    
    imageSave imageSaver;
    int saverTracker; bool lastFound; bool savedForTracker;
    ofTexture destinationNonSmiling;
    int x,y;
    
    ExpressionClassifier classifier;
    
    void setup();
    void update(cv::Mat& source, cv::Mat& dest);
    void draw(ofTexture& source, ofTexture& destination);
    
    void keyPressed(int key);
};
#endif /* defined(__modulab_0_1__expressionSwap__) */
