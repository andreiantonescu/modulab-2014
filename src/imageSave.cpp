//
//  imageSave.cpp
//  modulab_0_1
//
//  Created by Andrei Antonescu on 12/11/14.
//
//

#include "imageSave.h"
#include "threadedImageSaver.h"

void imageSave::setup(){
    classifier.load("Expressions");
}

bool imageSave::update(ofxFaceTrackerThreaded& trackerRef, ofPixels& pixels, int tracker){
    
    if(trackerRef.getFound()){
        classifier.classify(trackerRef);
        if(classifier.getProbability(2)<0.4){
            image.setFromPixels(pixels);
            image.saveThreaded("savedFaces/tracker" + ofToString(tracker) + ".png");
            cout<<"saved for "<<tracker<<endl;
            return true;
        }
    }
    return false;
}