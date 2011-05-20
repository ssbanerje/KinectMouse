#include "ImageOperations.h"

//--------------------------------------------------------------
void ImageOperations::setOpenCLContextAndInitializeKernels(OpenCL *cl) {
    opencl = cl;
    opencl->loadProgramFromFile("ImageOperations.cl", false);
    depthKernel = opencl->loadKernel("depthThreshold");
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
    size_t SIZE[1] = {len};
    depthKernel->run(1, SIZE);
    opencl->finish();
    depthBuffer.read(pixels, 0, len*sizeof(unsigned char));
    opencl->finish();
}

//--------------------------------------------------------------
void ImageOperations::distanceTransform() {
    
}