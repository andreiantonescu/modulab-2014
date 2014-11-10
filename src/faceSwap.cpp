//
//  faceSwapper.cpp
//  modulab_0_1
//
//  Created by Andrei Antonescu on 09/11/14.
//
//

#include "faceSwap.h"

void faceSwap::loadFace(string face){
    src.loadImage(face);
    if(src.getWidth() > 0) {
        cv::Mat test;
        test = ofxCv::toCv(src);
        cv::medianBlur(test,test, 11);
        srcTracker.update(test);
        srcPoints = srcTracker.getImagePoints();
        //        cout<<"face: "<<currentFace<<endl;
        for(int i=0; i<srcTracker.getImagePoints().size();i++){
            cout<<srcTracker.getImagePoints().at(i)<<endl;}
    }
}

void faceSwap::setup(){
    
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
    
    //just gifs?
    faces.allowExt("jpg");
    faces.allowExt("png");
    faces.listDir("faces");
    cout<<"Faces dir size: "<<faces.size()<<endl;
    for(int i=0; i<faces.size(); i++)
        cout<<faces.getPath(i)<<endl;
    
    currentFace = 0;
    if(faces.size()!=0){
        loadFace(faces.getPath(currentFace));
    }
}