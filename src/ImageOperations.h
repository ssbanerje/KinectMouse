#pragma once

#include "ofxOpenCv.h"
#include "MSAOpenCL.h"

using namespace MSA;

class ImageOperations {
private:
    OpenCL              *opencl;
    ofxCvColorImage     *colorImage;
    ofxCvGrayscaleImage *depthImage;
public:
    void setOpenCLContextAndInitializeKernels(OpenCL *ocl);
    void setImages(ofxCvColorImage *cI, ofxCvGrayscaleImage *gI) {colorImage = cI; depthImage = gI;}
    void depthThreshold(int near, int far);
    void distanceTransform();
};