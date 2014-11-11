//
//  expressionSwap.cpp
//  modulab_0_1
//
//  Created by Andrei Antonescu on 09/11/14.
//
//

#include "expressionSwap.h"

void expressionSwap::setup(){
    trackerSource.setIterations(5);
    trackerSource.setClamp(3.0);
    trackerSource.setTolerance(.01);
    trackerSource.setAttempts(1);
	trackerSource.setup();
    trackerDest.setup();
    
    mouthFbo.allocate( ofGetWidth(), ofGetHeight() );
	mouthMaskFbo.allocate( ofGetWidth(), ofGetHeight() );
    
    maskShader.load( "mask.vert", "mask.frag" );
    
    ofSetLineWidth(1);
    
    ofEnableAlphaBlending();
}

void expressionSwap::update(cv::Mat& source, cv::Mat& dest){
    trackerSource.update(source);
    trackerDest.update(dest);
}

void expressionSwap::draw(cv::Mat& frame, ofImage& destImage){
    
        ofPushMatrix();
        ofTranslate(-trackerDest.getPosition().x+120, -trackerDest.getPosition().y+120);
        ofPixels somepix = destImage.getPixelsRef();
        cv::Mat destImageCV = ofxCv::toCv(somepix);
        ofxCv::drawMat(destImageCV, 0, 0);
        ofPopMatrix();
    
//            mouth source face drawing
////////////
        mouthFbo.begin();
        ofPushMatrix();
        ofTranslate(ofVec2f(trackerSource.getPosition().x,trackerSource.getPosition().y));
        ofScale(trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
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
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        ofCircle(trackerSource.getObjectFeature(ofxFaceTrackerThreaded::INNER_MOUTH).getCentroid2D(), mouthCircleMesh);
        ofSetColor(255);
        ofPopMatrix();
        mouthMaskFbo.end();
        
//            draw mouth using the shaders // behind destination face
////////////
        ofPushMatrix();
        ofTranslate(120-trackerSource.getPosition().x, 120-trackerSource.getPosition().y);
        maskShader.begin();
        maskShader.setUniform1f( "time", ofGetElapsedTimef() );
        maskShader.setUniformTexture( "texture1", mouthMaskFbo.getTextureReference(), 1);
        ofSetColor( 255, 255, 255 );
        ofTranslate(ofVec2f(trackerSource.getPosition().x,trackerSource.getPosition().y));
        ofxCv::applyMatrix(trackerSource.getRotationMatrix().getInverse());
        mouthFbo.draw(-trackerSource.getPosition().x, -trackerSource.getPosition().y);
        maskShader.end();
        ofPopMatrix();
        
//            draw second face
////////////
        ofPushMatrix();
        ofTranslate(ofVec2f(120,120));
        ofScale(trackerDest.getScale(),trackerDest.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        
        destImage.getTextureReference().bind();
        ofMesh subMesh;
        subMesh = trackerDest.getObjectMesh();
        for(int i=17; i<36; i++){
            subMesh.setVertex(i, trackerSource.getObjectMesh().getVertex(i));
        }
        for(int i=48; i<66; i++){
            subMesh.setVertex(i, trackerSource.getObjectMesh().getVertex(i));
        }
        subMesh.draw();

        ofSetColor(255);
        destImage.getTextureReference().unbind();
        ofPopMatrix();
}

void expressionSwap::keyPressed(int key){
    if(key == 'r'){
        trackerSource.reset();
    }
}