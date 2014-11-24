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
    
    void setup();
    bool update(ofxFaceTrackerThreaded& trackerRef, ofPixels& pixels, int tracker);
    
};

#endif /* defined(__modulab_0_1__imageSave__) */
