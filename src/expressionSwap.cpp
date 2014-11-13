//
//  expressionSwap.cpp
//  modulab_0_1
//
//  Created by Andrei Antonescu on 09/11/14.
//
//

#include "expressionSwap.h"

ofPixels matToPixels (cv::Mat& mat){
    ofPixels pixels;
    ofxCv::toOf(mat, pixels);
    return pixels;
}

void expressionSwap::setup(){
//    trackerSource.setIterations(5);
//    trackerSource.setClamp(3.0);
//    trackerSource.setTolerance(.01);
//    trackerSource.setAttempts(1);
	trackerSource.setup();
    trackerDest.setup();
    
    mouthFbo.allocate( ofGetWidth(), ofGetHeight() );
	mouthMaskFbo.allocate( ofGetWidth(), ofGetHeight() );
    maskShader.load( "mask.vert", "mask.frag" );
    
    imageSaver.setup();
    
    ofSetLineWidth(1);
    ofEnableAlphaBlending();
}

void expressionSwap::update(cv::Mat& source, cv::Mat& dest){
    ofPixels pixels = matToPixels(source);
//    imageSaver.update(trackerSource, pixels);
    
    cv::Mat sourcePreProc = initialFramePreproc(source);
    cv::Mat destPreProc = initialFramePreproc(dest);
    
    trackerSource.update(sourcePreProc);
    trackerDest.update(destPreProc);
}

void expressionSwap::draw(cv::Mat& frame, ofImage& destImage){
    
        destImage.draw(0, 0);
    
//            mouth source face drawing
////////////
        mouthFbo.begin();
        ofPushMatrix();
        ofTranslate(ofVec2f(trackerSource.getPosition().x,trackerSource.getPosition().y));
        ofScale(trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale());
        ofxCv::applyMatrix(trackerSource.getRotationMatrix());
        ofxCv::drawMat(frame, -trackerSource.getPosition().x, -trackerSource.getPosition().y);
        ofPopMatrix();
        mouthFbo.end();
        
//            get source face mouth mask
////////////
        mouthMaskFbo.begin();
        ofBackground( 0, 0, 0 );
        ofPushMatrix();
        ofTranslate(ofVec2f(trackerSource.getPosition().x,trackerSource.getPosition().y));
        ofScale(trackerDest.getScale(),trackerDest.getScale(),1.0);
        ofxCv::applyMatrix(trackerSource.getRotationMatrix());
        ofPath path;
        for( int i = 0; i < trackerSource.getObjectFeature(ofxFaceTrackerThreaded::INNER_MOUTH).size(); i++) {
            if(i == 0) {
                path.newSubPath();
                path.moveTo(trackerSource.getObjectFeature(ofxFaceTrackerThreaded::INNER_MOUTH).getVertices()[i] );
            } else {
                path.lineTo( trackerSource.getObjectFeature(ofxFaceTrackerThreaded::INNER_MOUTH).getVertices()[i] );
            }
        }
        path.getTessellation().draw();
        ofSetColor(255);
        ofPopMatrix();
        mouthMaskFbo.end();
        
//            draw mouth using the shaders // behind destination face
////////////
        ofPushMatrix();
        ofTranslate(trackerDest.getPosition().x,trackerDest.getPosition().y);
        maskShader.begin();
        maskShader.setUniform1f( "time", ofGetElapsedTimef() );
        maskShader.setUniformTexture( "texture1", mouthMaskFbo.getTextureReference(), 1);
        ofSetColor( 255, 255, 255 );
        ofxCv::applyMatrix(trackerSource.getRotationMatrix().getInverse());
        mouthFbo.draw(-trackerSource.getPosition().x, -trackerSource.getPosition().y);
        maskShader.end();
        ofPopMatrix();
        
//            draw second face
////////////
//        ofPushMatrix();
//        ofTranslate(ofVec2f(trackerDest.getPosition()));
//        ofScale(trackerDest.getScale(),trackerDest.getScale());
//        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
//        
//        destImage.getTextureReference().bind();
//        ofMesh subMesh;
//        subMesh = trackerDest.getObjectMesh();
//        for(int i=27; i<36; i++){
//            subMesh.setVertex(i, trackerSource.getObjectMesh().getVertex(i));
//        }
//        for(int i=48; i<66; i++){
//            subMesh.setVertex(i, trackerSource.getObjectMesh().getVertex(i));
//        }
////        cout<<subMesh.getVertex(50).x<<" plm "<<subMesh.getVertex(10).y<<endl;
////        ofCircle(subMesh.getVertex(50).x, subMesh.getVertex(10).y, 1);
//        subMesh.draw();
//
//        ofSetColor(255);
//        destImage.getTextureReference().unbind();
//        ofPopMatrix();
    
//            draw original and source
////////////
        cv::Mat frameResized;
        cv::resize(frame,frameResized,cv::Size(),0.25,0.25,CV_INTER_LINEAR);
        ofxCv::drawMat(frameResized, 0, 0);
        ofImage destResized;
        destImage.resize(destImage.width/4, destImage.height/4);
        destImage.draw(0, frameResized.rows);
}

void expressionSwap::keyPressed(int key){
    if(key == 'r'){
        trackerSource.reset();
    }
}