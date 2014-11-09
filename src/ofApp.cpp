#include "ofApp.h"
using namespace cv;
using namespace ofxCv;

int indexer;

void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
    
    // camera setup -- pseye and standard webcam
    std::vector<ofVideoDevice> camDevices = cam.listDevices();
    std::vector<ofVideoDevice> devices = PSEye.listDevices();
    
    for(std::size_t i = 0; i < devices.size(); ++i){
        std::stringstream ss;
        
        ss << devices[i].id << ": " << devices[i].deviceName;
        
        if(!devices[i].bAvailable)
        {
            ss << " - unavailable ";
        }
        ofLogNotice("ofApp::setup") << ss.str();
	}
    
    PSEye.setDeviceID(0);
	PSEye.setDesiredFrameRate(120);
	PSEye.initGrabber(camWidth, camHeight);
    PSEye.setAutogain(true);
    cam.initGrabber(camWidth, camHeight);

    // test image from drive
    srcTestMat = cv::imread("/Users/andreiantonescu/Desktop/average.jpg");
    srcTestMat = initialFramePreproc(srcTestMat);

    debugMode = false;
    indexer = 0; // check if needed, implement GUI
    
    expressionSwapper.setup();

}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
	if(cam.isFrameNew()) {
        frame = toCv(cam);
        frame = initialFramePreproc(frame);
		tracker.update(frame);
        
        expressionSwapper.update(frame, srcTestMat);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
    ofSetColor(255);
    if(!debugMode)
        cam.draw(0, 0);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), ofPoint(20,20));
    
    // to rename this
    ofPixels srcTestMatPixels;
    toOf(srcTestMat, srcTestMatPixels);
    ofImage srcTestMatOF;
    srcTestMatOF.setFromPixels(srcTestMatPixels);
    
    if(expressionSwapper.trackerSource.getFound() && expressionSwapper.trackerDest.getFound()) {
        if(!debugMode){
            expressionSwapper.trackerSource.draw();
        }
        if(debugMode){
            expressionSwapper.draw(frame, srcTestMatOF);
            
//            draw first face wireframe
            ofPushMatrix();
            ofTranslate(350, 120);
            ofScale(expressionSwapper.trackerSource.getScale(),expressionSwapper.trackerSource.getScale(),1.0);
            applyMatrix(expressionSwapper.trackerSource.getRotationMatrix());
            cam.getTextureReference().bind();
            expressionSwapper.trackerSource.getObjectMesh().draw();
            cam.getTextureReference().unbind();
            expressionSwapper.trackerSource.getObjectMesh().drawWireframe();
            ofPopMatrix();
//            draw second face
            ofPushMatrix();
            ofTranslate(500,0);
            drawMat(srcTestMat, 0, 0);
            ofPopMatrix();
            
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'r') {
		tracker.reset();
	}
    if(key == 'd'){
        debugMode = !debugMode;
    }
    if(key == ']'){
        indexer++;
    }
    if(key == '['){
        indexer--;
    }
    
}

void ofApp::exit(){
    
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

