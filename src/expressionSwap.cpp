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
    
    destinationFaceFbo.allocate(camWidth, camHeight);
    sourceFaceFbo.allocate(camWidth, camHeight);
    cloneSecondMask.allocate(camWidth, camHeight);
    cloneSecondFbo.allocate(camWidth,camHeight);
	mouthMaskFbo.allocate(camWidth, camHeight);
    cloneMask.allocate(camWidth, camHeight);
    destinationFaceMaskFbo.allocate(camWidth, camHeight);
    
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
//    
//    srcSaved = cv::imread("/Users/andreiantonescu/Downloads/openFrameworks-cpp11/apps/devApps/modulab_0_1/bin/data/savedFaces/tracker1.png");
//    cv::cvtColor(srcSaved, srcSaved, CV_BGR2RGB);
//    cv::Mat srcProcessed = initialFramePreproc(srcSaved);
//    savedTracker.update(srcSaved);
    
}

void expressionSwap::draw(ofTexture& source, ofTexture& destination, cv::Mat& dest){
    
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
//        destination.draw(0, 0); // draw destination
    
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
        ofScale(trackerSource.getScale(), trackerSource.getScale(),trackerSource.getScale());
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
        ofScale(trackerDest.getScale(),trackerDest.getScale());
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
    
//        faceSwapper.updateWithMesh(dest, "/Users/andreiantonescu/Downloads/openFrameworks-cpp11/apps/devApps/modulab_0_1/bin/data/savedFaces/tracker1.png", trackerDest);
//        faceSwapper.draw();
    
//      get destination face fbo
////////////
        destinationFaceFbo.begin();
        ofPushMatrix();
        ofClear(0,0,0);
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofTranslate(ofVec2f(trackerDest.getPosition()));
        ofScale(trackerDest.getScale(),trackerDest.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        destination.bind();
        subMesh.draw();
        ofSetColor(255);
        destination.unbind();
        ofPopMatrix();
        destinationFaceFbo.end();
    
        destinationFaceMaskFbo.begin();
        ofPushMatrix();
        ofClear(0, 0, 0);
        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
        ofTranslate(ofVec2f(trackerDest.getPosition()));
        ofScale(trackerDest.getScale(),trackerDest.getScale());
        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
        subMesh.draw();
        ofPopMatrix();
        destinationFaceMaskFbo.end();

    
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
        ofPopMatrix();
        mouthMaskFbo.end();
    
        cloneMask.begin();
        ofClear(255);
        cloneMask.end();
        clone.setStrength(cloneStrength);
        clone.update(sourceFaceFbo.getTextureReference(), destinationFaceFbo.getTextureReference(), cloneMask.getTextureReference());
    
//        this will be the new destinationFaceFbo
        cloneSecond.setStrength(cloneStrength);
        cloneSecond.update(destinationFaceFbo.getTextureReference(), destinationFaceFbo.getTextureReference(), destinationFaceMaskFbo.getTextureReference());
    
        cloneSecondFbo.begin();
        cloneSecond.draw(0, 0);
        cloneSecondFbo.end();
    
        maskShader.begin();
        maskShader.setUniform1f( "time", ofGetElapsedTimef() );
        maskShader.setUniformTexture( "texture1",  destinationFaceMaskFbo.getTextureReference(), 1);
        cloneSecondFbo.draw(0,0);
        maskShader.end();
    
        classifier.classify(trackerDest);
        if(classifier.getProbability(2)<0.1)
            cout<<classifier.getProbability(2)<<endl;
    
////        draw shader
//        ofPushMatrix();
//        ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
//        ofTranslate(x,y);
//        maskShader.begin();
//        maskShader.setUniform1f( "time", ofGetElapsedTimef() );
//        maskShader.setUniformTexture( "texture1", mouthMaskFbo.getTextureReference(), 1);
//        ofSetColor( 255, 255, 255 );
//        ofPushMatrix();
//        ofTranslate(toDraw);
//        ofxCv::applyMatrix(trackerDest.getRotationMatrix());
////        sourceFaceFbo.draw(-toDraw);
//        clone.draw(-toDraw.x,-toDraw.y);
//        ofPopMatrix();
//        maskShader.end();
//        ofPopMatrix();
    
//        destinationFaceFbo.draw(0, 0);
//    clone.draw(0, 0);
//      cloneSecond.draw(0, 0);
//        mouthMaskFbo.draw(0, 0);

//    sourceFaceFbo.draw(0,0);
//    destinationFaceMaskFbo.draw(0, 0);
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