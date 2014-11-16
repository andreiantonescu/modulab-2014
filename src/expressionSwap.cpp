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

cv::Mat colorConvert(cv::Mat& source_img, cv::Mat& target_img){
    Mat source_img_cie,
    target_img_cie;
    
    cvtColor(source_img, source_img_cie, CV_BGR2Lab );
    cvtColor(target_img, target_img_cie, CV_BGR2Lab );
    
    
    /* Split into individual l a b channels */
    vector<Mat> source_channels,
    target_channels;
    
    split( source_img_cie, source_channels );
    split( target_img_cie, target_channels );
    
    /* For each of the l, a, b, channel ... */
    for( int i = 0; i < 3; i++ ) {
        /* ... find the mean and standard deviations */
        /* ... for source image ... */
        Mat temp_mean, temp_stddev;
        meanStdDev(source_channels[i], temp_mean, temp_stddev);
        double source_mean     = temp_mean.at<double>(0);
        double source_stddev   = temp_stddev.at<double>(0);
        
        /* ... and for target image */
        meanStdDev(target_channels[i], temp_mean, temp_stddev);
        double target_mean     = temp_mean.at<double>(0);
        double target_stddev   = temp_stddev.at<double>(0);
        
        /* Fit the color distribution from target LAB to our source LAB */
        target_channels[i].convertTo( target_channels[i], CV_64FC1 );
        target_channels[i] -= target_mean;
        target_channels[i] *= (target_stddev / source_stddev);
        target_channels[i] += source_mean;
        target_channels[i].convertTo( target_channels[i], CV_8UC1 );
    }
    
    
    /* Merge the lab channels back into a single BGR image */
    Mat output_img;
    merge(target_channels, output_img);
    cvtColor(output_img, output_img, CV_Lab2BGR );
    
    
    /* Append all the images together so that it looks like a triptych */
    int max_rows = MAX(source_img.rows, target_img.rows);
    Mat append( max_rows, source_img.cols + target_img.cols + output_img.cols, CV_8UC3, Scalar(0, 0, 0) );
    source_img.copyTo( Mat(append, cv::Rect( Point2i(0, 0),                                 source_img.size() )) );
    target_img.copyTo( Mat(append, cv::Rect( Point2i(source_img.cols, 0),                   target_img.size() )) );
    output_img.copyTo( Mat(append, cv::Rect( Point2i(source_img.cols + target_img.cols, 0), output_img.size() )) );
    
    return append;
}




ofPixels matToPixels (cv::Mat& mat){
    ofPixels pixels;
    ofxCv::toOf(mat, pixels);
    return pixels;
}

void expressionSwap::setup(){
	trackerSource.setup();
    trackerDest.setup();
    
    mouthFbo.allocate(ofGetWidth(), ofGetHeight());
	mouthMaskFbo.allocate( ofGetWidth(), ofGetHeight() );
    maskShader.load( "mask.vert", "mask.frag" );
    
    imageSaver.setup();
    
    ofSetLineWidth(1);
    ofEnableAlphaBlending();
    
    x = 0; y=0;
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
        destImage.draw(0, 0);
    
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
    
//    mouth fbo
        mouthFbo.begin();
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
        mouthFbo.end();
    
        //            draw second face
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
    
//    mask fbo
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
    
//        ofPixels inter;
//        mouthFbo.readToPixels(inter);
//        cv::Mat test,test2;
//        test2 = imread("/Users/andreiantonescu/Desktop/average.jpg");
//        cv::cvtColor(frame,test,CV_BGR2RGB);
//        imshow("asdasd", colorConvert(test2,test));
    
//    draw shader
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
        mouthFbo.draw(-toDraw);
        ofPopMatrix();
        maskShader.end();
        ofPopMatrix();

//            draw original and source
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