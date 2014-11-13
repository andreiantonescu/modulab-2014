//
//  threadedImageSaver.h
//  modulab_0_1
//
//  Created by Andrei Antonescu on 12/11/14.
//
//

#ifndef __modulab_0_1__threadedImageSaver__
#define __modulab_0_1__threadedImageSaver__

#include <iostream>
#include "threadedImageSaver.h"
#include "genInclude.h"
#include "ofMain.h"
#include "ofThread.h"

class threadedImageSaver : public ofThread, public ofImage{
public:
    
    string fileName;
    void threadedFunction();
    void saveThreaded(string fileName);
};

#endif /* defined(__modulab_0_1__threadedImageSaver__) */
