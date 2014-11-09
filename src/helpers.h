//
//  helpers.h
//  modulab_0_1
//
//  Created by Andrei Antonescu on 09/11/14.
//
//

#ifndef __modulab_0_1__helpers__
#define __modulab_0_1__helpers__

#include <stdio.h>
#include "genInclude.h"

cv::Mat correctGamma(const cv::Mat& image, const unsigned int& gamma);
cv::Mat initialFramePreproc(const cv::Mat& image);
ofImage cvToOF(const cv::Mat&cvImage);

#endif /* defined(__modulab_0_1__helpers__) */
