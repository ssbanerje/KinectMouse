#include "KinectMouse.h"


//--------------------------------------------------------------
void KinectMouse::setup() {
	ofSetLogLevel(0);
	ofLog(OF_LOG_VERBOSE, "KinectMouse::setup()");
    
    //Setup Application
    debug = true;
    showUI = true;
    mirror = false;
    dispWidth = 1680;
    dispHeight = 1050;
    ofSetFrameRate(30);
    ofBackground(0, 0, 0);
    ofSetWindowShape(800, 600);
    ofSetWindowTitle("Kinect Mouse");
    dispFont.loadFont("Courier New.ttf",14,true,true);
    dispFont.setLineHeight(20.0f);
    soundDetect.loadSound("sound/16582__tedthetrumpet__kettleswitch1.aif");
	soundDetect.setVolume(100);
	soundRelease.loadSound("sound/2674__dmooney__TAPE32.wav");
	soundRelease.setVolume(100);
    
    //Initialize Kinect
    kinectAngle = 0;
    kinect.init();
    kinect.open();
    kinect.setCameraTiltAngle(kinectAngle);
    
    //Setup images
    colorImage.allocate(kinect.width, kinect.height);
    checkGrayImage.allocate(kinect.width, kinect.height);
    grayImage.allocate(kinect.width, kinect.height);
    grayThreshImg.allocate(kinect.width, kinect.height);
    grayThreshImgPrev.allocate(kinect.width, kinect.height);
    
    //Setup detection
    nearThreshold = 5;
    farThreshold = 30;
    detectCount = 0;
    twoHandsCount = 0;
    
    //Setup GUI
    gui.setup();
    gui.config->gridSize.x = 300;
    gui.addTitle("KINECT SETTINGS");
    gui.addSlider("Tilt Angle", kinectAngle, -30, 30);
    gui.addToggle("Mirror Mode", mirror);
    gui.addTitle("DETECT RANGE");
    gui.addSlider("Near Distance", nearThreshold, 5, 20);
    gui.addSlider("Far Distance", farThreshold, 20, 60);
    gui.addTitle("MOUSE CONTROL");
    gui.addSlider("Display Width", dispWidth, 600, 1980);
    gui.addSlider("Display Height", dispHeight, 600, 1980);
    gui.setDefaultKeys(true);
    gui.loadFromXML();
    gui.show();
}

//--------------------------------------------------------------
void KinectMouse::update() {
    kinect.update();
    checkDepthUpdate();
    
    //Update Images
    grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
	grayThreshImg.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
   	grayThreshImg.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
    if(mirror) {
        grayImage.mirror(false,true);
    }
    
    //Do thresholding
    unsigned char *pix = grayImage.getPixels();
	unsigned char *grayThreshPix = grayThreshImg.getPixels();
	int numPixels = grayImage.getWidth() * grayImage.getHeight();
	int maxThreshold = 255 - nearThreshold;
	int minThreshold = 255 - farThreshold;
	int nearestDepth = 0;
	for(int i=0; i<numPixels; i++) {
		if (minThreshold < pix[i] && pix[i] < maxThreshold && pix[i] > nearestDepth) {
			nearestDepth = pix[i];
		}
	}
    for(int i=0; i<numPixels; i++) {
		if(minThreshold<pix[i] && pix[i]<nearestDepth+2 && pix[i]>nearestDepth-10 )  {
			pix[i] = 255;
        } else {
			pix[i] = 0;
		}
	}
    grayImage.flagImageChanged();    
    contourFinder.findContours(grayImage, 1500, (kinect.width*kinect.height)/4, 2, false);
    
    //Detect blobs
    if(showUI) {
        return;
    }
    int detectHands = contourFinder.blobs.size();
    
    if (detectHands == 2) {
        twoHandsCount = min(60, ++twoHandsCount);
    } else {
        twoHandsCount = max(0, --twoHandsCount);
    }
    
    if (detectHands > 0) {
		detectCount = min(50, ++detectCount);
    } else {
		detectCount = max(0, --detectCount);
	}
    
    if (detectingHands) {
		if (detectCount < 10) {
			detectingHands = false;
			for (int j=0; j<hands.size(); j++){
				hands[j]->unregister();
			}
			soundRelease.play();
		}
	} else {
		if (detectCount > 30) {
			detectingHands = true;
			soundDetect.play();
		}
	}
    
    if (detectingTwoHands) {
		if (twoHandsCount < 15) {
			detectingTwoHands = false;
		}
	} else {
		if (twoHandsCount > 30) {
			detectingTwoHands = true;
			CGEventRef keyEv = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)101, true);
			CGEventPost (kCGHIDEventTap, keyEv);
			CGEventRef keyEv2 = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)101, false);
			CGEventPost (kCGHIDEventTap, keyEv2);
		}
	}
    
    if (detectingHands && detectHands==1) {
		for (int i = 0; i < contourFinder.nBlobs; i++) {
			float x = (float)contourFinder.blobs[i].centroid.x;
			float y = (float)contourFinder.blobs[i].centroid.y;
            
			int cornerCount = contourFinder.blobs[i].nPts;
			float centroidX = 0;
			float centroidY = 0;
			float addCount = 0;
			for (int j=0; j<contourFinder.blobs[i].nPts; j+=5) {
				addCount++;
				centroidX += contourFinder.blobs[i].pts[j].x;
				centroidY += contourFinder.blobs[i].pts[j].y;
			}
			centroidX = centroidX/addCount;
			centroidY = centroidY/addCount;
			if (hands.size() == 0) {
				Hand *hand = new Hand(true, dispWidth, dispHeight);
				hand->setIsActive(true);
				hand->update(ofPoint(x, y), cornerCount, ofPoint(x, y));
				hands.push_back(hand);
			}
            else{
				for (int j = 0; j < hands.size(); j++) {
					hands[j]->update(ofPoint(x, y), cornerCount, ofPoint(centroidX, centroidY));
				}
			}
		}
	}
}

//--------------------------------------------------------------
void KinectMouse::draw(){
    ofSetColor(255, 255, 255);
    
    //Draw initial menu with settings
    if(showUI){
        kinect.drawDepth(400, 0, 400, 300);
		gui.draw();
        dispFont.drawString("Press Space Key to start.", 20, ofGetHeight()-60);
		ofPushMatrix();
		ofTranslate(400, 300, 0);
		glScalef(0.6, 0.6, 1.0f); 
        for (int i = 0; i < contourFinder.nBlobs; i++){
            ofPushMatrix();
            contourFinder.blobs[i].draw(0,0);
			ofSetColor(255, 0, 0);
            ofFill();
            ofEllipse(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y, 4, 4);
			float centroidX = 0;
			float centroidY = 0;
			float addCount = 0;
			for (int j = 0; j < contourFinder.blobs[i].nPts; j+=5){
				addCount++;
				centroidX += contourFinder.blobs[i].pts[j].x;
				centroidY += contourFinder.blobs[i].pts[j].y;
			}
			centroidX = centroidX/addCount;
			centroidY = centroidY/addCount;
			ofCircle(centroidX, centroidY, 10);
		    ofPopMatrix();
        }
		ofPopMatrix();
	}
    //Draw grayImage when the KinectMouse is in use
    else{
		grayImage.draw(0, 0, 400, 300);
	}
	ofSetColor(255, 255, 255);
    ofNoFill();
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
			if (showUI) {
				ofSetWindowShape(800, 600);
			} else {
				ofSetWindowShape(400, 300);
				kinect.setCameraTiltAngle(kinectAngle);
			}
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

//--------------------------------------------------------------
void KinectMouse::checkDepthUpdate(){
    if(ofGetFrameNum() % 150 == 0){
        ofLog(OF_LOG_VERBOSE, "KinectMouse::checkDepthUpdate()");
        unsigned char *nextDepth = kinect.getDepthPixels();
        if (ofGetFrameNum() != 150){
			unsigned char *currentDepthPixels = checkGrayImage.getPixels();
			int pixNum = kinect.width * kinect.height;
            for (int i=0; i<pixNum; i++) {
                if (nextDepth[i] != currentDepthPixels[i]) {
                    break;
				}
				if (i > pixNum/2) {
					ofLog(OF_LOG_ERROR, "Reset Kinect");
					kinect.close();
					kinect.open();
					kinect.setCameraTiltAngle(kinectAngle);
					break;
				}
			}                  
		}
        checkGrayImage.setFromPixels(nextDepth, kinect.width, kinect.height);
    }
}