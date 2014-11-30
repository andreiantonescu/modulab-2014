#include "ofApp.h"
using namespace cv;
using namespace ofxCv;

VideoCapture capture;

void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    ofEnableAlphaBlending();
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
    cv::cvtColor(srcTestMat, srcTestMat, CV_BGR2RGB);
//    cv::resize(srcTestMat,srcTestMat,cv::Size(),0.25,0.25,CV_INTER_LINEAR);
    // test with live video instead of still frame
    videoPlayer.loadMovie("/Users/andreiantonescu/Desktop/Untitled.mov");
    videoPlayer.play();

    debugMode = false;
    
    expressionSwapper.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    videoPlayer.update();
	if(cam.isFrameNew()) {
        frame = toCv(cam);
//        cv::cvtColor(frame,frame,CV_BGR2RGB);
        
        videoFrame = toCv(videoPlayer.getPixelsRef());
        cv::resize(videoFrame, videoFrame, cv::Size(),0.75,0.75,CV_INTER_LINEAR);
        
//        expressionSwapper.update(frame, srcTestMat);
        expressionSwapper.update(srcTestMat, frame);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
    
    ofSetColor(255);
    if(!debugMode)
        cam.draw(0, 0);
    
    if(expressionSwapper.trackerSource.getFound() && expressionSwapper.trackerDest.getFound()) {
        if(!debugMode){
            expressionSwapper.trackerSource.draw();
        }
        if(debugMode){
            ofImage temp = cvToOF(srcTestMat);
            expressionSwapper.draw(temp.getTextureReference(), cam.getTextureReference());
//            expressionSwapper.draw(cam.getTextureReference(), temp.getTextureReference());
        }
    }
    
    ofSetColor(0, 0, 255);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), ofPoint(20,20));
    ofSetColor(255, 255, 255);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    expressionSwapper.keyPressed(key);
    if(key == 'd'){
        debugMode = !debugMode;
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
