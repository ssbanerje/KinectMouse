#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxSimpleGuiToo.h"
#include "MSAOpenCL.h"
#include "ImageOperations.h"

using namespace std;
using namespace MSA;

class KinectMouse : public ofBaseApp{    
public:
    void setup();
    void update();
    void draw();
    void exit();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);

private:
    //Kinect
    ofxKinect           kinect;
    int                 kinectAngle;
    int                 nearThreshold;
    int                 farThreshold;
    int                 skeletonThreshold;
    ofxCvGrayscaleImage depthImage;
    ofxCvColorImage     colorImage;
    ofxCvGrayscaleImage thresholdImage;
    ofxCvGrayscaleImage distanceTransformImage;
    OpenCL              opencl;
    
    //Application
    bool                mirror;
    bool                showUI;
    int                 dispWidth;
    int                 dispHeight;
    ImageOperations     imgOps;
    ofTrueTypeFont      dispFont;
};
