//
//  expressionSwap.cpp
//  modulab_0_1
//
//  Created by Andrei Antonescu on 09/11/14.
//
//


///

#include "expressionSwap.h"
using namespace cv;
using namespace ofxCv;

ofPixels matToPixels (cv::Mat& mat){
    ofPixels pixels;
    ofxCv::toOf(mat, pixels);
    return pixels;
}

void expressionSwap::setup(){
	trackerSource.setup();
    trackerDest.setup();
    
    destinationFaceFbo.allocate(camWidth, camHeight);
    sourceFaceFbo.allocate(camWidth, camHeight);
	mouthMaskFbo.allocate(camWidth, camHeight);
    cloneMask.allocate(camWidth, camHeight);
    
    clone.setup(camWidth, camHeight);
    
    maskShader.load( "mask.vert", "mask.frag" );
    
    imageSaver.setup();
    
    ofSetLineWidth(1);
    ofEnableAlphaBlending();
    
    x = 0; y = 0;
}

void expressionSwap::update(cv::Mat& source, cv::Mat& dest){
    ofPixels pixels = matToPixels(source);
//    imageSaver.update(trackerSource, pixels);
    
    cv::Mat sourcePreProc = initialFramePreproc(source);
    cv::Mat destPreProc = initialFramePreproc(dest);
    
    trackerSource.update(sourcePreProc);
    trackerDest.update(destPreProc);
}

void expressionSwap::draw(cv::Mat& frame, ofImage& destImage, ofVideoGrabber& cam){
    
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        destImage.draw(0, 0); // draw destination
    
//    get inner and outer mouth meshes
        ofPolyline innerMouth = trackerSource.getObjectFeature(ofxFaceTrackerThreaded::INNER_MOUTH);
        ofMesh innerMouthMesh;
        ofPolyline outerMouth = trackerSource.getObjectFeature(ofxFaceTrackerThreaded::OUTER_MOUTH);
        ofMesh outerMouthMesh;
        tess.tessellateToMesh(innerMouth,OF_POLY_WINDING_POSITIVE, innerMouthMesh);
        tess.tessellateToMesh(outerMouth,OF_POLY_WINDING_POSITIVE, outerMouthMesh);
//    fill inner mouth
        ofMesh face = trackerSource.getObjectMesh();
        int vertices[8]={48,60,61,62,54,63,64,65}; // inner mouth vertices
        for(int i=0; i<8; i++)
            for(int j=0; j<8; j++)
                for(int z=0; z<8; z++)
                    face.addTriangle(vertices[i], vertices[j], vertices[z]);
    
        ofVec2f toDraw = trackerDest.getPosition(); // final place to put the new mouth
    
//      get source face fbo - contains mouth
        sourceFaceFbo.begin();
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofClear(0, 0, 0);
        ofTranslate(toDraw);
        ofScale(trackerSource.getScale(), trackerSource.getScale(),trackerSource.getScale());
        ofScale(trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale());
        cam.getTextureReference().bind();
        face.draw();
        cam.getTextureReference().unbind();
        ofPopMatrix();
        sourceFaceFbo.end();
    
//      draw destination face
////////////
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofTranslate(ofVec2f(trackerDest.getPosition()));
        ofScale(trackerDest.getScale(),trackerDest.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        
        destImage.getTextureReference().bind();
        ofMesh subMesh;
        subMesh = trackerDest.getObjectMesh();
        for(int i=27; i<36; i++){
            subMesh.setVertex(i, trackerSource.getObjectMesh().getVertex(i));
        }
        for(int i=48; i<66; i++){
            subMesh.setVertex(i, trackerSource.getObjectMesh().getVertex(i));
        }
        subMesh.draw();
        
        ofSetColor(255);
        destImage.getTextureReference().unbind();
        ofPopMatrix();
    
//      get destination face fbo
////////////
        destinationFaceFbo.begin();
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofTranslate(ofVec2f(trackerDest.getPosition()));
        ofScale(trackerDest.getScale(),trackerDest.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        destImage.getTextureReference().bind();
        subMesh.draw();
        ofSetColor(255);
        destImage.getTextureReference().unbind();
        ofPopMatrix();
        destinationFaceFbo.end();
    
//    get mouth mask fbo for source face
        mouthMaskFbo.begin();
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofClear(0, 0, 0);
        ofTranslate(toDraw);
        ofScale(trackerSource.getScale(), trackerSource.getScale(),trackerSource.getScale());
        ofScale(trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale());
        innerMouthMesh.draw();
        outerMouthMesh.draw();
        ofPopMatrix();
        mouthMaskFbo.end();
    
        cloneMask.begin();
        ofClear(255);
        cloneMask.end();
        clone.setStrength(24);
        clone.update(sourceFaceFbo.getTextureReference(), destinationFaceFbo.getTextureReference(), cloneMask.getTextureReference());
    
//        draw shader
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofTranslate(x,y);
        maskShader.begin();
        maskShader.setUniform1f( "time", ofGetElapsedTimef() );
        maskShader.setUniformTexture( "texture1", mouthMaskFbo.getTextureReference(), 1);
        ofSetColor( 255, 255, 255 );
        ofPushMatrix();
        ofTranslate(toDraw);
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
//        sourceFaceFbo.draw(-toDraw);
        clone.draw(-toDraw.x,-toDraw.y);
        ofPopMatrix();
        maskShader.end();
        ofPopMatrix();

//        draw original and source
////////////
        cv::Mat frameResized;
        cv::resize(frame,frameResized,cv::Size(),0.25,0.25,CV_INTER_LINEAR);
        ofxCv::drawMat(frameResized, 350, 0);
        ofImage destResized;
        destImage.resize(destImage.width/4, destImage.height/4);
        destImage.draw(350, frameResized.rows);
}

void expressionSwap::keyPressed(int key){
    if(key == 'r'){
        trackerSource.reset();
    }
    if(key == 'a')
        y++;
    if(key == 's')
        y--;
}