#include "ImageOperations.h"
#include "ofMain.h"

static int L0[25]={
    -1,-1,-1,-1,-1,
    0, 0, 0, 0, 0,
    2, 2, 2, 2, 2,
    0, 0, 0, 0, 0,
    -1,-1,-1,-1,-1
};
static int L45[25]={
    0,-1,-1, 0, 2,
    -1,-1, 0, 2, 0,
    -1, 0, 2, 0,-1,
    0, 2, 0,-1,-1,
    2, 0,-1,-1, 0
};
static int L90[25]={
    -1, 0, 2, 0,-1,
    -1, 0, 2, 0,-1,
    -1, 0, 2, 0,-1,
    -1, 0, 2, 0,-1,
    -1, 0, 2, 0,-1
};
static int L135[25]={
    2, 0,-1,-1, 0,
    0, 2, 0,-1,-1,
    -1, 0, 2, 0,-1,
    -1,-1, 0, 2, 0,
    0,-1,-1, 0, 2
};

//--------------------------------------------------------------
void ImageOperations::setOpenCLContextAndInitializeKernels(OpenCL *cl) {
    opencl = cl;
    opencl->loadProgramFromFile("ImageOperations.cl", false);
    depthKernel = opencl->loadKernel("depthThreshold");
    distanceKernel = opencl->loadKernel("distanceTransform");
    templates[0].initBuffer(25*sizeof(int), CL_MEM_READ_ONLY, L0);
    templates[1].initBuffer(25*sizeof(int), CL_MEM_READ_ONLY, L45);
    templates[2].initBuffer(25*sizeof(int), CL_MEM_READ_ONLY, L90);
    templates[3].initBuffer(25*sizeof(int), CL_MEM_READ_ONLY, L135);
    opencl->finish();
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
    distanceKernel->setArg(0, distanceBuffer[0].getCLMem());
    distanceKernel->setArg(1, distanceBuffer[1].getCLMem());
    distanceKernel->setArg(2, templates[0].getCLMem());
    distanceKernel->setArg(3, templates[1].getCLMem());
    distanceKernel->setArg(4, templates[2].getCLMem());
    distanceKernel->setArg(5, templates[3].getCLMem());
    distanceKernel->setArg(6, skTh);
    opencl->finish();
    distanceKernel->run2D(width, height);
    opencl->finish();
    distanceBuffer[1].read(fin, 0, len*sizeof(unsigned char));
    opencl->finish();
    ofSleepMillis(100);
}

//--------------------------------------------------------------
void ImageOperations::setHeightWidth(int w, int h) {
    width = w;
    height = h;
}