//
//  helpers.cpp
//  modulab_0_1
//
//  Created by Andrei Antonescu on 09/11/14.
//
//

#include "helpers.h"

cv::Mat correctGamma(const cv::Mat& image, const unsigned int& gamma ){
    double inverse_gamma = 1.0 / gamma;
    cv::Mat lut_matrix(1, 256, CV_8UC1 );
    uchar * ptr = lut_matrix.ptr();
    for( int i = 0; i < 256; i++ )
        ptr[i] = (int)( pow( (double) i / 255.0, inverse_gamma) * 255.0);
    cv::Mat result;
    cv::LUT(image, lut_matrix, result);
    return result;
}

cv::Mat initialFramePreproc(const cv::Mat& image){
    
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

ofImage cvToOF(const cv::Mat&cvImage){
    ofPixels somePixels;
    ofxCv::toOf(cvImage, somePixels);
    ofImage image;
    image.setFromPixels(somePixels);
    
    return image;
}