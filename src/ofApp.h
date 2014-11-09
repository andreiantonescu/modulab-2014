#pragma once

#include "ofMain.h"
#include "genInclude.h"

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
		
        ofVideoGrabber cam; //
        ofVideoGrabber camSecond; //
        cv::Mat frame;//
    
        ofxFaceTrackerThreaded tracker;//
        ofxFaceTrackerThreaded trackerSecond;//
    
        bool debugMode;
    
        ofImage srcTest;
        cv::Mat srcTestMat;
    
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
