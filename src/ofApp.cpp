#include "ofApp.h"
using namespace cv;
using namespace ofxCv;

// correct gamma helper
cv::Mat ofApp::correctGamma(const cv::Mat& image, const unsigned int& gamma ){
    double inverse_gamma = 1.0 / gamma;
    
    cv::Mat lut_matrix(1, 256, CV_8UC1 );
    uchar * ptr = lut_matrix.ptr();
    for( int i = 0; i < 256; i++ )
        ptr[i] = (int)( pow( (double) i / 255.0, inverse_gamma ) * 255.0 );
    
    cv::Mat result;
    cv::LUT( image, lut_matrix, result );
    
    return result;
}

// initial frame preproc helper
cv::Mat ofApp::initialFramePreproc(const cv::Mat& image){
    
    cv::Mat gamma_corrected = correctGamma(image, initialPreprocGamma);
    cv::Mat lab_image;
    cv::cvtColor(gamma_corrected, lab_image, CV_BGR2HSV);
    
    // Extract the L channel
    std::vector<cv::Mat> lab_planes(3);
    cv::split(lab_image, lab_planes);  // now we have the L image in lab_planes[0]
    
    // apply the CLAHE algorithm to the L channel
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE();
    clahe->setClipLimit(initialPreprocClahe);
    cv::Mat dst;
    clahe->apply(lab_planes[2], dst);
//    cv::medianBlur(dst, dst, initialPreprocBlur);
    
    return dst;
}
int indexer;
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
	ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
    
    std::vector<ofVideoDevice> camDevices = cam.listDevices();
    cout<<camDevices.size()<<endl;
    
    // get device list
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
    
    // tracker setup.
    tracker.setIterations(5);
    tracker.setClamp(3.0);
    tracker.setTolerance(.01);
    tracker.setAttempts(1);
	tracker.setup();
    trackerSecond.setup();

    srcTestMat = cv::imread("/Users/andreiantonescu/Desktop/average.jpg");
    srcTestMat = initialFramePreproc(srcTestMat);

    debugMode = false;
    indexer = 0;
    
    mouthFbo.allocate( ofGetWidth(), ofGetHeight() );
	mouthMaskFbo.allocate( ofGetWidth(), ofGetHeight() );
    
    maskShader.load( "mask.vert", "mask.frag" );
    
    
    ofEnableAlphaBlending();
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
	if(cam.isFrameNew()) {
        frame = toCv(cam);
        frame = initialFramePreproc(frame);
		tracker.update(frame);
		position = tracker.getPosition();
		scale = tracker.getScale();
		orientation = tracker.getOrientation();
		rotationMatrix = tracker.getRotationMatrix();
        
        objectMesh = tracker.getObjectMesh();
		meanMesh = tracker.getMeanObjectMesh();
	}
    
//    trackerSecond.update(srcTestMat);
//    positionSecond = trackerSecond.getPosition();
//    scaleSecond = trackerSecond.getScale();
//    orientationSecond = trackerSecond.getOrientation();
//    rotationMatrixSecond = trackerSecond.getRotationMatrix();

//    player.update();
//    if(player.isFrameNew()){
//        srcTestMat = toCv(player);
//        srcTestMat = initialFramePreproc(srcTestMat);
//        trackerSecond.update(srcTestMat);
//    }
    trackerSecond.update(srcTestMat);
    
    PSEye.update();
    
	if (PSEye.isFrameNew())
    {
//		videoTexture.loadData(PSEye.getPixelsRef());
//        frame = toCv(PSEye.getPixelsRef());
//        frame = initialFramePreproc(frame);
//        trackerSecond.update(frame);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofSetColor(255);
//    ofBackground(125,125,255);
    if(debugMode){
//
    }
    else
        cam.draw(0, 0);
    
    ofDrawBitmapString(ofToString(ofGetFrameRate()), ofPoint(20,20));
    ofMesh toSet;
    if(tracker.getFound() && trackerSecond.getFound()) {
//        trackerSecond.draw();
		ofSetLineWidth(1);
        if(!debugMode){
            tracker.draw();
        }
        if(debugMode){
            
            mouthFbo.begin();
            ofPushMatrix();
            ofTranslate(ofVec2f(tracker.getPosition().x,tracker.getPosition().y));
            ofScale(trackerSecond.getScale()/tracker.getScale(),trackerSecond.getScale()/tracker.getScale());
            ofxCv::drawMat(frame, -tracker.getPosition().x, -tracker.getPosition().y);
            ofPopMatrix();
            mouthFbo.end();
            
            // get first face mouth mask fbo
            mouthMaskFbo.begin();
            ofBackground( 0, 0, 0 );
            ofPushMatrix();
            ofTranslate(ofVec2f(tracker.getPosition().x,tracker.getPosition().y));
            ofScale(trackerSecond.getScale(),trackerSecond.getScale(),1.0);
            applyMatrix(rotationMatrixSecond);
            ofPath path;
            for( int i = 0; i < tracker.getObjectFeature(ofxFaceTrackerThreaded::INNER_MOUTH).size(); i++) {
                if(i == 0) {
                    path.newSubPath();
                    path.moveTo(tracker.getObjectFeature(ofxFaceTrackerThreaded::INNER_MOUTH).getVertices()[i] );
                } else {
                    path.lineTo( tracker.getObjectFeature(ofxFaceTrackerThreaded::INNER_MOUTH).getVertices()[i] );
                }
            }
            ofCircle(tracker.getObjectFeature(ofxFaceTrackerThreaded::INNER_MOUTH).getCentroid2D(), 13);
//            path.getTessellation().draw();
            ofSetColor(255);
            ofPopMatrix();
            mouthMaskFbo.end();
            
            ofSetupScreenOrtho(640, 480, OF_ORIENTATION_UNKNOWN, true, -1000, 1000);
            
////////////
            // draw mouth;
            ofPushMatrix();
            ofTranslate(120-tracker.getPosition().x, 120-tracker.getPosition().y);
            maskShader.begin();
            maskShader.setUniform1f( "time", ofGetElapsedTimef() );
            maskShader.setUniformTexture( "texture1", mouthMaskFbo.getTextureReference(), 1 );
            ofSetColor( 255, 255, 255 );
            
            ofTranslate(ofVec2f(tracker.getPosition().x,tracker.getPosition().y));
            applyMatrix(rotationMatrix.getInverse());
            mouthFbo.draw( -tracker.getPosition().x, -tracker.getPosition().y );
            
            maskShader.end();
            ofPopMatrix();
            
////////////
            /// draw second face
            ofPushMatrix();
            ofTranslate(ofVec2f(120,120));
            ofScale(trackerSecond.getScale(),trackerSecond.getScale());
            applyMatrix(rotationMatrixSecond);
            
            ofPixels letsee;
            toOf(srcTestMat, letsee);
            ofImage final;
            final.setFromPixels(letsee);
            final.getTextureReference().bind();
            
//            videoTexture.bind();
            ofMesh subMesh;
            subMesh = trackerSecond.getObjectMesh();
            for(int i=17; i<66; i++){ //17 / 48 /  66
                subMesh.setVertex(i, tracker.getObjectMesh().getVertex(i));
            }
            subMesh.draw();
            
            for(int i=17; i<66; i++){
                ofSetColor(255,255,0);
//                ofCircle(subMesh.getVertex(i).x, subMesh.getVertex(i).y, 0.5);
                ofSetColor(0,0,255);
                ofSetColor(255);
            }
            ofSetColor(255);
//            videoTexture.unbind();
            final.getTextureReference().unbind();
            ofPopMatrix();
         
////////////
            // draw first face wireframe
            ofPushMatrix();
            ofTranslate(350, 120);
            ofScale(tracker.getScale(),tracker.getScale(),1.0);
            applyMatrix(rotationMatrix);
            ofDrawAxis(scale);
            cam.getTextureReference().bind();
            tracker.getObjectMesh().draw();
            cam.getTextureReference().unbind();
            tracker.getObjectMesh().drawWireframe();
            ofPopMatrix();
            
////////////
            // draw second face
            ofPushMatrix();
            ofTranslate(500,0);
            drawMat(srcTestMat, 0, 0);
            ofPopMatrix();
            
        }
    }
    
    // test ps eye
//    videoTexture.draw(0, 0);
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


// intersection test - doesn't work ?
//faceCut.clear();
//IntersectionData id;
//
//for(int i=0;i<tracker.getObjectMesh().getUniqueFaces().size();i++){
//    IsTriangle triangle;
//    ofMeshFace face=tracker.getObjectMesh().getUniqueFaces().at(i);
//    triangle.set(face);
//    
//    for(int j=0;j<trackerSecond.getObjectMesh().getUniqueFaces().size();j++){
//        IsTriangle triangleSecond;
//        ofMeshFace faceSecond=trackerSecond.getObjectMesh().getUniqueFaces().at(j);
//        triangleSecond.set(faceSecond);
//        
//        id = intersection.TriangleTriangleIntersection(triangle, triangleSecond);
//        if(id.isIntersection){
//            faceCut.addVertex(id.pos);
//            faceCut.addVertex(id.pos+id.dir);
//        }
//    }
//}
//
//ofTranslate(ofGetWidth()-200, 500);
//faceCut.draw();
//cout<<tracker.getObjectMesh().getUniqueFaces().size()<<endl;
