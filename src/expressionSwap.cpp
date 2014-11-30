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
void expressionSwap::draw(ofTexture& source, ofTexture& destination, cv::Mat& dest){
    
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        destination.draw(0, 0); // draw destination
    
        // get tracker of last non smiling frame
        // then use that to get the correct texture on
    
        ofTexture destinationNonSmiling;
        classifier.classify(trackerDest);
        if(classifier.getProbability(2)<0.001 && classifier.getProbability(3)<0.001) {
            ofPixels temporaryPixels;
            destination.readToPixels(temporaryPixels);
            testing.setFromPixels(temporaryPixels);
  
            temporaryNonSmilingMat = toCv(testing.getPixelsRef());
            temporaryNonSmilingMat = initialFramePreproc(temporaryNonSmilingMat);
            trackerNonSmiling.update(temporaryNonSmilingMat);
        }
    
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
    
//      get source face fbo
        sourceFaceFbo.begin();
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofClear(0, 0, 0);
        ofTranslate(toDraw);
        ofScale(trackerSource.getScale(),trackerSource.getScale(),trackerSource.getScale());
        ofScale(trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale());
        source.bind();
        face.draw();
        source.unbind();
        ofPopMatrix();
        sourceFaceFbo.end();
    
//      draw dest face - put the saved face from pics here
////////////
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofTranslate(ofVec2f(trackerDest.getPosition()));
        ofScale(trackerDest.getScale(),trackerDest.getScale(),trackerDest.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        destination.bind();
        ofMesh subMesh;
        subMesh = trackerDest.getObjectMesh();
        for(int i=27; i<36; i++){
            subMesh.setVertex(i, trackerSource.getObjectMesh().getVertex(i));
        }
        for(int i=48; i<66; i++){
            subMesh.setVertex(i, trackerSource.getObjectMesh().getVertex(i));
        }
//        subMesh.draw();
        ofSetColor(255);
        destination.unbind();
        ofPopMatrix();

//      get destination face fbo
////////////
        destinationFaceFbo.begin();
        ofPushMatrix();
        ofClear(0,0,0);
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

        subMesh.clearTexCoords();
        subMesh.addTexCoords(trackerNonSmiling.getImagePoints());
        ofPushMatrix();
        ofTranslate(ofVec2f(trackerDest.getPosition()));
        ofScale(trackerDest.getScale(),trackerDest.getScale(),trackerDest.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        testing.bind();
        subMesh.draw();
        testing.unbind();
        ofPopMatrix();
    
//        nonSmilingFbo.begin();
//        ofPushMatrix();
//        ofClear(0,0,0);
//        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
////        ofTranslate(ofVec2f(trackerNonSmiling.getPosition()));
//        ofScale(trackerNonSmiling.getScale(),trackerNonSmiling.getScale(),trackerNonSmiling.getScale());
////        ofxCv::applyMatrix(trackerNonSmiling.getRotationMatrix());
//        testing.bind();
//        trackerNonSmiling.getObjectMesh().draw();
//        ofSetColor(255);
//        testing.unbind();
//        ofPopMatrix();
//        nonSmilingFbo.end();
    
//    get mouth mask fbo from source face
        mouthMaskFbo.begin();
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofClear(0, 0, 0);
        ofTranslate(toDraw);
        ofScale(trackerSource.getScale(), trackerSource.getScale(),trackerSource.getScale());
        ofScale(trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale(),trackerDest.getScale()/trackerSource.getScale());
    
        innerMouthMesh.draw();
        outerMouthMesh.draw();
        ofRect(outerMouthMesh.getCentroid()-ofPoint(10,5),20,15);

        ofPopMatrix();
        mouthMaskFbo.end();
    
        gaussianBlur.clear();
        gaussianBlur.setTexture(mouthMaskFbo.getTextureReference());
        gaussianBlur.update();
    
        mouthMaskFboBlurred.begin();
        ofPushMatrix();
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofClear(0, 0, 0);
        gaussianBlur.draw();
        ofPopMatrix();
        mouthMaskFboBlurred.end();
    
        cloneMask.begin();
        ofClear(255);
        cloneMask.end();
        clone.setStrength(cloneStrength);
        clone.update(sourceFaceFbo.getTextureReference(), destinationFaceFbo.getTextureReference(), cloneMask.getTextureReference());
    
        destinationFaceFbo.draw(0, 0);
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
        clone.draw(-toDraw.x,-toDraw.y);
        ofPopMatrix();
        maskShader.end();
        ofPopMatrix();
    
//        trackerDest.draw();
//        trackerDest.getImageMesh().drawWireframe();
    
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