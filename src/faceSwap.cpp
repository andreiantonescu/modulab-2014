//
//  faceSwapper.cpp
//  modulab_0_1
//
//  Created by Andrei Antonescu on 09/11/14.
//
//

#include "faceSwap.h"

void faceSwap::setup(string directory){
    
    cloneReady = false;
    clone.setup(camWidth, camHeight);
    ofFbo::Settings settings;
    settings.width = camWidth; settings.height = camHeight;
    
    maskFbo.allocate(settings);
    srcFbo.allocate(settings);
    
    camTracker.setup();
    srcTracker.setup();
    srcTracker.setIterations(25);
    srcTracker.setAttempts(4);
}

void faceSwap::update(cv::Mat& video, cv::Mat& faceSource, ofVideoGrabber& cam){
    camTracker.update(video);
    
    srcTracker.update(faceSource);
    srcPoints = srcTracker.getImagePoints();
    
    cloneReady = camTracker.getFound();
    
    if(cloneReady){
        ofMesh camMesh = camTracker.getImageMesh();
        camMesh.clearTexCoords();
        camMesh.addTexCoords(srcPoints);
        
        maskFbo.begin();
        ofClear(0, 255);
        camMesh.draw();
        maskFbo.end();
        
        srcFbo.begin();
        ofClear(0, 255);
        src.bind();
        camMesh.draw();
        src.unbind();
        srcFbo.end();
        
        clone.setStrength(16);
        clone.update(srcFbo.getTextureReference(), cam.getTextureReference(), maskFbo.getTextureReference());
    }
}

void faceSwap::draw(){
    
}