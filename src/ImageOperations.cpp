#include "ImageOperations.h"
#include "ofMain.h"

//--------------------------------------------------------------
void ImageOperations::setOpenCLContextAndInitializeKernels(OpenCL *cl) {
    opencl = cl;
    opencl->loadProgramFromFile("ImageOperations.cl", false);
    depthKernel = opencl->loadKernel("depthThreshold");
    distanceKernel = opencl->loadKernel("distanceTransform");
}

//--------------------------------------------------------------
void ImageOperations::depthThreshold(int near, int far) {
    const unsigned int len = width*height;
    unsigned char *pixels = thresholdImage->getPixels();
    depthBuffer.initBuffer(len*sizeof(unsigned char), CL_MEM_READ_WRITE, pixels);
    depthKernel->setArg(0, depthBuffer.getCLMem());
    depthKernel->setArg(1, near);
    depthKernel->setArg(2, far);
    opencl->finish();
    depthKernel->run1D(len);
    opencl->finish();
    depthBuffer.read(pixels, 0, len*sizeof(unsigned char));
    opencl->finish();
}

//--------------------------------------------------------------
void ImageOperations::distanceTransform(int skTh) {
    const unsigned int len = width*height;
    unsigned char *ori = thresholdImage->getPixels();
    unsigned char *fin = distanceImage->getPixels();
    distanceBuffer[0].initBuffer(len*sizeof(unsigned char), CL_MEM_READ_ONLY, ori);
    distanceBuffer[1].initBuffer(len*sizeof(unsigned char), CL_MEM_READ_WRITE);
    opencl->finish();
    distanceKernel->setArg(0, distanceBuffer[0].getCLMem());
    distanceKernel->setArg(1, distanceBuffer[1].getCLMem());
    opencl->finish();
    distanceKernel->run2D(width, height);
    opencl->finish();
    distanceBuffer[1].read(fin, 0, len*sizeof(unsigned char));
    opencl->finish();
}

//--------------------------------------------------------------
void ImageOperations::setHeightWidth(int w, int h) {
    width = w;
    height = h;
}