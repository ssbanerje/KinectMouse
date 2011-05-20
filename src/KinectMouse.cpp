#include "KinectMouse.h"


//--------------------------------------------------------------
void KinectMouse::setup() {
	ofSetLogLevel(0);
    ofSetDataPathRoot("../Resources/");
    
    //Initialize Kinect
    kinectAngle = 0;
    kinect.init();
    kinect.open();
    kinect.setCameraTiltAngle(kinectAngle);
    nearThreshold = 5;
    farThreshold = 30;
    mirror = false;
    dispWidth = 1680;
    dispHeight = 1050;
    depthImage.allocate(kinect.width, kinect.height);
    colorImage.allocate(kinect.width, kinect.height);
    
    //Initialize OpenCL
    opencl.setup(CL_DEVICE_TYPE_GPU, 2);
    imgOps.setOpenCLContextAndInitializeKernels(&opencl);
    
    //Setup GUI
    showUI = true;
    gui.setup();
    gui.config->gridSize.x = 200;
    gui.addTitle("Screen Dimensions");
    gui.addSlider("Display Width", dispWidth, 600, 1980);
    gui.addSlider("Display Height", dispHeight, 600, 1980);
    gui.addTitle("Kinect Settings");
    gui.addSlider("Tilt Angle", kinectAngle, -30, 30);
    gui.addToggle("Mirror", mirror);
    gui.addTitle("Depth Threshold");
    gui.addSlider("Near Distance", nearThreshold, 5, 20);
    gui.addSlider("Far Distance", farThreshold, 20, 60);
    gui.setDefaultKeys(true);
    gui.loadFromXML();
    gui.show();
}

//--------------------------------------------------------------
void KinectMouse::update() {
}

//--------------------------------------------------------------
void KinectMouse::draw(){
    //Draw initial menu with settings
    if(showUI){
		gui.draw();
        dispFont.drawString("Press Space Key to start.", 20, ofGetHeight()-60);
	}
    //Draw grayImage when the KinectMouse is in use
    else{
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
				ofSetWindowShape(800, 600);
			else
				ofSetWindowShape(400, 300);
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
