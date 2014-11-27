//
//  faceShift.h
//  modulab_0_1
//
//  Created by Andrei Antonescu on 25/11/14.
//
//

#ifndef __modulab_0_1__faceShift__
#define __modulab_0_1__faceShift__

#include <iostream>
#include "genInclude.h"

class faceShift {
public:
    ofxFaceTrackerThreaded camTracker;
    ofxFaceTrackerThreaded srcTracker;
    
    void setup();
    void update();
};

#endif /* defined(__modulab_0_1__faceShift__) */
