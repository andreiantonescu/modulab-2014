#pragma once

#include "ofMain.h"
#include "genInclude.h"
#include "expressionSwap.h"
#include "faceSwap.h"

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
		
        ofVideoGrabber cam;
        cv::Mat frame;
        bool debugMode;
    
        cv::Mat srcTestMat;

        ofxPS3EyeGrabber PSEye;
        ofTexture videoTexture;
    
        ofxUICanvas *gui;
        void exit();
        void guiEvent(ofxUIEventArgs &e);
    
        expressionSwap expressionSwapper;
        faceSwap faceSwapper;
    
        bool isNewFace;
};
