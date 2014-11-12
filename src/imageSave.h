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

class imageSave{
public:
    
    ExpressionClassifier classifier;
    
    void setup();
    void update(ofxFaceTrackerThreaded& trackerRef);
    
};

#endif /* defined(__modulab_0_1__imageSave__) */
