#ifndef _KINECT_MOUSE_APP_INC
#define _KINECT_MOUSE_APP_INC

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxSimpleGuiToo.h"
#include "Hand.h"

#include <iostream>
#include <string.h>
#include <vector>

class KinectMouse : public ofBaseApp{    
public:
    //ofBaseApp
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
    ofxKinect kinect;
    int kinectAngle;
    void checkDepthUpdate();
    
    //Images
    ofxCvColorImage colorImage;
    ofxCvGrayscaleImage checkGrayImage;
    ofxCvGrayscaleImage grayImage;
    ofxCvGrayscaleImage grayThreshImg;
    ofxCvGrayscaleImage grayThreshImgPrev;
    int nearThreshold;
    int farThreshold;
    
    //Application
    bool debug;
    bool showUI;
    bool mirror;
    int dispHeight;
    int dispWidth;
    ofSoundPlayer soundDetect;
    ofSoundPlayer soundRelease;
    ofTrueTypeFont dispFont;
    
    //Detection Algorithm
    int detectCount;
    int twoHandsCount;
    bool detectingHands;
    bool twoHandsDetected;
    ofxCvContourFinder contourFinder;
    vector<Hand *> hands;    
};

#endif
