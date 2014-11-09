#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTrackerThreaded.h"
#include "ofxIntersection.h"
#include "ofxPS3EyeGrabber.h"
#include "ofxUI.h"

#include "constants.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
        cv::Mat initialFramePreproc(const cv::Mat& image);
        cv::Mat correctGamma(const cv::Mat& img,const unsigned int& gamma );
		
        ofVideoGrabber cam;
        ofVideoGrabber camSecond;
        ofxFaceTrackerThreaded tracker;
        ofxFaceTrackerThreaded trackerSecond;
        cv::Mat frame;
    
        ofVec2f position;
        float scale;
        ofVec3f orientation;
        ofMatrix4x4 rotationMatrix;
    
        ofVec2f positionSecond;
        float scaleSecond;
        ofVec3f orientationSecond;
        ofMatrix4x4 rotationMatrixSecond;
    
        cv::Mat translation, rotation;
        ofMatrix4x4 pose;
    
        bool debugMode;
    
        ofMesh objectMesh;
        ofMesh meanMesh;
    
/////////
/////////
        // intersection test - TO REMOVE
        ofImage srcTest;
        cv::Mat srcTestMat;
    
/////////
/////////
        // intersection test - TO REMOVE
        ofxIntersection intersection;
        ofMesh faceCut;
    
/////////
/////////
        // mouth from test subject
        ofFbo mouthFbo, mouthMaskFbo;
        ofShader maskShader;
    
/////////
/////////
        // PSEye cameras
        ofxPS3EyeGrabber PSEye;
        ofTexture videoTexture;
    
        ofxUICanvas *gui;
        void exit();
        void guiEvent(ofxUIEventArgs &e);
};
