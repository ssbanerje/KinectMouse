#include "KinectMouse.h"


//--------------------------------------------------------------
void KinectMouse::setup() {
	ofSetLogLevel(OF_LOG_ERROR);
    ofSetDataPathRoot("../Resources/");
    ofSetWindowTitle("Kinect Mouse");
    ofSetFrameRate(50);
    ofSetWindowPosition(300, 300);
    
    //Initialize Kinect
    kinectAngle = 0;
    kinect.init();
    kinect.setVerbose(false);
    kinect.open();
    kinect.setCameraTiltAngle(kinectAngle);
    nearThreshold = 245;
    farThreshold = 220;
    skeletonThreshold = 6;
    mirror = false;
    dispWidth = 1680;
    dispHeight = 1050;
    depthImage.allocate(kinect.width, kinect.height);
    colorImage.allocate(kinect.width, kinect.height);
    thresholdImage.allocate(kinect.width, kinect.height);
    distanceTransformImage.allocate(kinect.width, kinect.height);
    
    //Initialize ImageOperations
    opencl.setup(CL_DEVICE_TYPE_GPU, 2);
    imgOps.setHeightWidth(kinect.width, kinect.height);
    imgOps.setOpenCLContextAndInitializeKernels(&opencl);
    imgOps.setOriginalImages(&colorImage, &depthImage);
    imgOps.setThresholdImage(&thresholdImage);
    imgOps.setDistanceTImage(&distanceTransformImage);
    
    //Setup GUI
    showUI = true;
    ofBackground(50, 50, 50);
    ofSetColor(255, 255, 255);
    gui.setup();
    gui.config->gridSize.x = 200;
    gui.addTitle("Screen Dimensions");
    gui.addSlider("Display Width", dispWidth, 600, 1980);
    gui.addSlider("Display Height", dispHeight, 600, 1980);
    gui.addTitle("Kinect Settings");
    gui.addSlider("Tilt Angle", kinectAngle, -30, 30);
    gui.addToggle("Mirror", mirror);
    gui.addTitle("Depth Threshold");
    gui.addSlider("Near Distance", nearThreshold, 230, 255);
    gui.addSlider("Far Distance", farThreshold, 0, 255);
    gui.addSlider("Skeleton Threshold", skeletonThreshold, 0, 15);
    gui.setDefaultKeys(true);
    gui.loadFromXML();
    gui.show();
    dispFont.loadFont("Courier New.ttf",14,true,true);
}

//--------------------------------------------------------------
void KinectMouse::update() {
    kinect.setCameraTiltAngle(kinectAngle);
    kinect.update();
    if(kinect.isFrameNew()) {
        colorImage.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
        depthImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        thresholdImage.setFromPixels(depthImage.getPixels(), kinect.width, kinect.height);
        imgOps.depthThreshold(nearThreshold, farThreshold);
        if(!showUI) {
            distanceTransformImage.setFromPixels(depthImage.getPixels(), kinect.width, kinect.height);
            imgOps.distanceTransform(skeletonThreshold);
            distanceTransformImage.flagImageChanged();
        }
        colorImage.flagImageChanged();
        depthImage.flagImageChanged();
        thresholdImage.flagImageChanged();
    }
}

//--------------------------------------------------------------
void KinectMouse::draw(){
    //Mouse not in use
    if(showUI){
        ofPushMatrix();
        ofTranslate(400, 100);
        colorImage.draw(0, 0, 400, 300);
        depthImage.draw(-200, 350, 400, 300);
        thresholdImage.draw(210, 350, 400, 300);
        ofPopMatrix();
		gui.draw();
        dispFont.drawString("Press Space Key to start.", 20, ofGetHeight()-60);
	}
    //Mouse is in use
    else{
        ofPushMatrix();
        ofTranslate(115, 10);
        colorImage.draw(0, 0, 200, 150);
        depthImage.draw(210, 0, 200, 150);
        thresholdImage.draw(420, 0, 200, 150);
        ofPopMatrix();
        distanceTransformImage.draw(250, 220, 400, 300);
        dispFont.drawString("Press Space Key to stop.", 20, ofGetHeight()-60);
        ofDrawBitmapString("Frame Rate : "+ofToString(ofGetFrameRate()), 20, ofGetHeight()-30);
	}
}

//--------------------------------------------------------------
void KinectMouse::exit() {
    kinect.setCameraTiltAngle(0);
	kinect.close();
    ofLog(OF_LOG_NOTICE, "KinectMouse::exit()");
}

//--------------------------------------------------------------
void KinectMouse::keyPressed(int key){
    ofLog(OF_LOG_VERBOSE, ofToString(key));
	switch (key)
	{
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
		case '<':		
		case ',':		
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
		case '-':		
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
        case 'r':
			kinect.close();
			kinect.open();
			kinect.setCameraTiltAngle(kinectAngle);
            break;
		case ' ':
			showUI = !showUI;
			if (showUI)
				ofSetWindowShape(1024, 768);
			else
				ofSetWindowShape(800, 600);
			break;			
		case OF_KEY_UP:
			kinectAngle++;
			if(kinectAngle>30) kinectAngle=30;
			kinect.setCameraTiltAngle(kinectAngle);
			break;
		case OF_KEY_DOWN:
			kinectAngle--;
			if(kinectAngle<-30) kinectAngle=-30;
			kinect.setCameraTiltAngle(kinectAngle);
			break;
	}
}

//--------------------------------------------------------------
void KinectMouse::keyReleased(int key){}

//--------------------------------------------------------------
void KinectMouse::mouseMoved(int x, int y ){}

//--------------------------------------------------------------
void KinectMouse::mouseDragged(int x, int y, int button){}

//--------------------------------------------------------------
void KinectMouse::mousePressed(int x, int y, int button){}

//--------------------------------------------------------------
void KinectMouse::mouseReleased(int x, int y, int button){}

//--------------------------------------------------------------
void KinectMouse::windowResized(int w, int h){}
