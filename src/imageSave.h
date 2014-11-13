//
//  imageSave.h
//  modulab_0_1
//
//  Created by Andrei Antonescu on 12/11/14.
//
//

#ifndef __modulab_0_1__imageSave__
#define __modulab_0_1__imageSave__

#include <iostream>
#include "genInclude.h"
#include "threadedImageSaver.h"

class imageSave{
public:
    
    ExpressionClassifier classifier;
    threadedImageSaver image;
    ofFbo fbo;
    
    int numFaces;
    
    void setup();
    void update(ofxFaceTrackerThreaded& trackerRef, ofPixels& pixels);
    
};

#endif /* defined(__modulab_0_1__imageSave__) */
