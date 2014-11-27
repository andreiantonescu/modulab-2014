//
//  faceSwapper.cpp
//  modulab_0_1
//
//  Created by Andrei Antonescu on 09/11/14.
//
//

#include "faceSwap.h"

void faceSwap::setup(){
    
    cloneReady = false;
    clone.setup(camWidth, camHeight);
    ofFbo::Settings settings;
    settings.width = camWidth; settings.height = camHeight;
    
    maskFbo.allocate(settings);
    srcFbo.allocate(settings);
    
    camTracker.setup();
    srcTracker.setup();
}

// two versions of this, one with video
void faceSwap::updateWithMesh(cv::Mat& destFace, string face, ofxFaceTrackerThreaded& destTracker){ // faceSource ?!
    
    src = cv::imread(face);
    cv::cvtColor(src, src, CV_BGR2RGB);
    cv::Mat srcProcessed = initialFramePreproc(src);
    
    srcTracker.update(src);
    if(srcTracker.getFound()){
        srcPoints = srcTracker.getImagePoints();}
    
    //
    
    cv::Mat camDest = initialFramePreproc(destFace);
    camTracker.update(camDest);
    
    cloneReady = camTracker.getFound();
    if(cloneReady) {
        
        ofMesh camMesh = destTracker.getObjectMesh();
        camMesh.clearTexCoords();
        camMesh.addTexCoords(srcPoints);
        
        for(int i=27; i<36; i++){
            camMesh.setVertex(i, srcTracker.getObjectMesh().getVertex(i));
        }
        for(int i=48; i<66; i++){
            camMesh.setVertex(i, srcTracker.getObjectMesh().getVertex(i));
        }
        
        maskFbo.begin();
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofClear(0, 255);
        ofTranslate(destTracker.getPosition());
        ofScale(destTracker.getScale(), destTracker.getScale(),destTracker.getScale());
        ofxCv::applyMatrix(destTracker.getRotationMatrix());
        camMesh.draw();
        ofPopMatrix();
        maskFbo.end();
        
        ofPixels tempSource;
        ofxCv::toOf(src, tempSource);
        ofImage sourceFaceImage;
        sourceFaceImage.setFromPixels(tempSource);
        
        srcFbo.begin();
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofClear(0, 255);
        ofTranslate(destTracker.getPosition());
        ofScale(destTracker.getScale(), destTracker.getScale(),destTracker.getScale());
        ofxCv::applyMatrix(destTracker.getRotationMatrix());
        sourceFaceImage.bind();
        camMesh.draw();
        sourceFaceImage.unbind();
        ofPopMatrix();
        srcFbo.end();
        
        clone.setStrength(24);
        
        ofPixels tempDest;
        ofxCv::toOf(destFace, tempDest);
        ofImage destFaceImage;
        destFaceImage.setFromPixels(tempDest);
        
        clone.update(srcFbo.getTextureReference(), destFaceImage.getTextureReference(), maskFbo.getTextureReference());
    }
}

void faceSwap::draw(){
    if(cloneReady)
        clone.draw(0, 0);
//    maskFbo.draw(0,0);
}