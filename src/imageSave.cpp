//
//  imageSave.cpp
//  modulab_0_1
//
//  Created by Andrei Antonescu on 12/11/14.
//
//

#include "imageSave.h"

void imageSave::setup(){
    classifier.load("Expressions");
}

void imageSave::update(ofxFaceTrackerThreaded& trackerRef){
    
    if(trackerRef.getFound()){
        classifier.classify(trackerRef);
    }
    for(int i = 0; i < classifier.size(); i++){
        if(classifier.getProbability(0)<0.6 && classifier.getProbability(2)<0.2){
            
        }
    }
}