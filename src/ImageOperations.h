#pragma once

#include "ofxOpenCv.h"
#include "MSAOpenCL.h"

using namespace MSA;

class ImageOperations {
private:
    OpenCL              *opencl;
    ofxCvColorImage     *colorImage;
    ofxCvGrayscaleImage *depthImage;
    ofxCvGrayscaleImage *thresholdImage;
    ofxCvGrayscaleImage *distanceImage;
    
public:
    void setOpenCLContextAndInitializeKernels(OpenCL *ocl);
    void setOriginalImages(ofxCvColorImage *cI, ofxCvGrayscaleImage *gI) {colorImage = cI; depthImage = gI;}
    void setThresholdImage(ofxCvGrayscaleImage *tI) {thresholdImage = tI;}
    void setDistanceTImage(ofxCvGrayscaleImage *dI) {distanceImage = dI;}
    void depthThreshold(int near, int far);
    void distanceTransform();
};