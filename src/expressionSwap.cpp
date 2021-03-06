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
    savedTracker.setup();
    trackerNonSmiling.setup();
    
    destinationFaceFbo.allocate(camWidth, camHeight);
    sourceFaceFbo.allocate(camWidth, camHeight);
    cloneSecondMask.allocate(camWidth, camHeight);
    cloneSecondFbo.allocate(camWidth,camHeight);
	mouthMaskFbo.allocate(camWidth, camHeight);
    cloneMask.allocate(camWidth, camHeight);
    destinationFaceMaskFbo.allocate(camWidth, camHeight);
    nonSmilingFbo.allocate(camWidth,camHeight);
    mouthMaskFboBlurred.allocate(camWidth, camHeight);
    mouthMaskFboToBlur.allocate(camWidth, camHeight);
    destinationBlurred.allocate(camWidth, camHeight);
    finalMouth.allocate(camWidth,camHeight);
    destinationFaceFboForBlur.allocate(camWidth,camHeight);
    skinFbo.allocate(camWidth, camHeight);
    
    gaussianBlur.allocate(camWidth,camHeight);
    gaussianBlur.setPasses(10);
    gaussianBlur.setRadius(20);
    
    clone.setup(camWidth, camHeight);
    cloneSecond.setup(camWidth, camHeight);
    
    maskShader.load( "mask.vert", "mask.frag" );
    
    imageSaver.setup();
    
    ofSetLineWidth(1);
    ofEnableAlphaBlending();
    
    x = 0; y = 4; // to correct
    
    saverTracker = 0;
    lastFound = false;
    savedForTracker = false;
    
    faceSwapper.setup();
    
    classifier.load("Expressions");
    
    skin.loadImage("/Users/andreiantonescu/Desktop/skin2.png");
}

void expressionSwap::update(cv::Mat& source, cv::Mat& dest){
    
    // image saving for destination tracker workflow
//    if(!trackerDest.getFound() && lastFound == true){
//        saverTracker++;
//        lastFound = false;
//        savedForTracker = false;
//    }
//    if(trackerDest.getFound() && savedForTracker == false){
//        lastFound = true;
//        ofPixels pixels = matToPixels(dest);
//        if(imageSaver.update(trackerDest, pixels, saverTracker)){
//            savedForTracker = true;
//        }
//    }
    
    cv::Mat sourcePreProc = initialFramePreproc(source);
    cv::Mat destPreProc = initialFramePreproc(dest);
    
    trackerSource.update(sourcePreProc);
    trackerDest.update(destPreProc);
    
}
ofImage testing;
cv::Mat temporaryNonSmilingMat;
void expressionSwap::draw(ofTexture& source, ofTexture& destination){
    
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
//        destination.draw(0, 0); // draw destination
    
        // get tracker of last non smiling frame
        // then use that to get the correct texture on
    
//        ofTexture destinationNonSmiling;
//        classifier.classify(trackerDest);
//        if(classifier.getProbability(2)<0.001 && classifier.getProbability(3)<0.001) {
//            ofPixels temporaryPixels;
//            destination.readToPixels(temporaryPixels);
//            testing.setFromPixels(temporaryPixels);
//  
//            temporaryNonSmilingMat = toCv(testing.getPixelsRef());
//            temporaryNonSmilingMat = initialFramePreproc(temporaryNonSmilingMat);
//            trackerNonSmiling.update(temporaryNonSmilingMat);
////        }
    
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
    
//    fill inner mouth dest
        ofMesh faceDest = trackerDest.getObjectMesh();
        int verticesDest[8]={48,60,61,62,54,63,64,65}; // inner mouth vertices
        for(int i=0; i<8; i++)
            for(int j=0; j<8; j++)
                for(int z=0; z<8; z++)
                    faceDest.addTriangle(verticesDest[i], verticesDest[j], verticesDest[z]);
    
        ofVec2f toDraw = trackerDest.getPosition(); // final place to put the new mouth
    
        ofMesh camMesh = trackerDest.getImageMesh();
        camMesh.clearTexCoords();
        camMesh.addTexCoords(trackerSource.getImagePoints());
        
//      get source face fbo
        sourceFaceFbo.begin();
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofClear(255);
        ofTranslate(toDraw);
        ofScale(trackerSource.getScale(),trackerSource.getScale(),trackerSource.getScale());
        ofScale(trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        source.bind();
        face.draw();
        source.unbind();
        ofPopMatrix();
        sourceFaceFbo.end();
    
        ofMesh subMesh;
        subMesh = trackerDest.getObjectMesh();
        for(int i=27; i<36; i++){
            subMesh.setVertex(i, trackerSource.getObjectMesh().getVertex(i));
        }
        for(int i=48; i<66; i++){
            subMesh.setVertex(i, trackerSource.getObjectMesh().getVertex(i));
        }
    
        for(int i=27; i<36; i++){
           faceDest.setVertex(i, trackerSource.getObjectMesh().getVertex(i));
        }
        for(int i=48; i<66; i++){
           faceDest.setVertex(i, trackerSource.getObjectMesh().getVertex(i));
        }
    
    
    
        //    get mouth mask fbo from source face
        mouthMaskFbo.begin();
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofClear(0, 0, 0);
        ofTranslate(toDraw);
        ofScale(trackerSource.getScale(), trackerSource.getScale(),trackerSource.getScale());
        ofScale(trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        innerMouthMesh.draw();
        outerMouthMesh.draw();
        ofPopMatrix();
        mouthMaskFbo.end();
        
        //    define mask area for blurring
        mouthMaskFboToBlur.begin();
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofClear(0, 0, 0);
        ofTranslate(toDraw);
        ofScale(trackerSource.getScale(), trackerSource.getScale(),trackerSource.getScale());
        ofScale(trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        ofRect(outerMouthMesh.getCentroid()-ofPoint(7,7),14,14);
//        ofCircle(-10,5,5);
//        ofCircle(10,5,5);
        ofPopMatrix();
        mouthMaskFboToBlur.end();
        
        gaussianBlur.clear();
        gaussianBlur.setTexture(mouthMaskFboToBlur.getTextureReference());
        gaussianBlur.update();
        
        mouthMaskFboBlurred.begin();
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofClear(255);
        gaussianBlur.draw();
        ofPopMatrix();
        mouthMaskFboBlurred.end();

//      get destination face fbo
////////////
        destinationFaceFbo.begin();
        ofPushMatrix();
        ofClear(255);
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofTranslate(ofVec2f(trackerDest.getPosition()));
        ofScale(trackerDest.getScale(),trackerDest.getScale(),trackerDest.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        destination.bind();
        subMesh.draw();
        ofSetColor(255);
        destination.unbind();
        ofPopMatrix();
        destinationFaceFbo.end();
    
        skinFbo.begin();
        ofPushMatrix();
        ofClear(255);
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofTranslate(ofVec2f(trackerDest.getPosition()));
        ofScale(trackerDest.getScale(),trackerDest.getScale(),trackerDest.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        skin.bind();
        faceDest.draw();
        ofSetColor(255);
        skin.unbind();
        ofPopMatrix();
        skinFbo.end();
    
        destinationFaceFboForBlur.begin();
        ofPushMatrix();
        ofClear(0,0,0);
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofTranslate(ofVec2f(trackerDest.getPosition()));
        ofScale(trackerDest.getScale(),trackerDest.getScale(),trackerDest.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        subMesh.draw();
        ofSetColor(255);
        ofPopMatrix();
        destinationFaceFboForBlur.end();
    
        cloneMask.begin();
        ofClear(255);
        cloneMask.end();
    
        //    draw outer mouth with color transfer
        clone.setStrength(cloneStrength);
        clone.update(sourceFaceFbo.getTextureReference(), destinationFaceFbo.getTextureReference(), cloneMask.getTextureReference());
        finalMouth.begin();
        ofClear(0, 0, 0);
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        maskShader.begin();
        maskShader.setUniform1f( "time", ofGetElapsedTimef() );
        maskShader.setUniformTexture( "texture1", mouthMaskFbo.getTextureReference(), 1);
        ofSetColor( 255, 255, 255 );
        clone.draw(0, 0);
        maskShader.end();
        ofPopMatrix();
        finalMouth.end();
    
        //redraw destinationFaceFbo.
        destinationFaceFbo.begin();
//        ofClear(0, 0, 0);
        destinationFaceFbo.draw(0, 0);
        finalMouth.draw(0, 0);
        destinationFaceFbo.end();
    
        // get blurred face
        clone.setStrength(30);
        clone.update(skinFbo.getTextureReference(), destinationFaceFbo.getTextureReference(), cloneMask.getTextureReference());
        
        destinationBlurred.begin();
        ofClear(0, 0, 0);
        destination.draw(0, 0);
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        maskShader.begin();
        maskShader.setUniform1f( "time", ofGetElapsedTimef() );
        maskShader.setUniformTexture( "texture1", mouthMaskFboBlurred.getTextureReference(), 1);
        ofSetColor( 255, 255, 255 );
        clone.draw(0, 0);
        maskShader.end();
        ofPopMatrix();
        destinationBlurred.end();

        destinationFaceFbo.draw(0,0);
//        destinationBlurred.draw(0, 0);
    
//        clone.draw(0, 0);
    
        destinationFaceFbo.begin();
        ofPushMatrix();
//        ofClear(0,0,0);
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofTranslate(ofVec2f(trackerDest.getPosition()));
        ofScale(trackerDest.getScale(),trackerDest.getScale(),trackerDest.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        destinationBlurred.getTextureReference().bind();
        subMesh.draw();
        ofSetColor(255);
        destinationBlurred.getTextureReference().unbind();
        ofPopMatrix();
        destinationFaceFbo.end();

        destinationFaceFbo.draw(0,0);
        finalMouth.draw(0,0);
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