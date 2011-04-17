#include "Hand.h"

int mouseDownCount = 0;

//--------------------------------------------------------------

Hand::Hand(bool isPrimary, int dispWidth, int dispHeight) {	
	this.isPrimary = isPrimary;
	isActive = false;
	this.dispHeight = dispHeight;
	this.dispWidth = dispWidth;
	mouseDownCount = 0;
	gestureCount = 0;
	toNormalModeCount = 0;
	handMode = HAND_MODE_NORMAL;
	previousFrameCount = ofGetFrameNum();
	isSuspendEvent = false;
	
	mouseDownCount = 0;
	
	soundClick.loadSound("sound/16582__tedthetrumpet__kettleswitch1.aif");
	soundClick.setVolume(100);
}

//--------------------------------------------------------------
Hand::~Hand() {}

//--------------------------------------------------------------
void Hand::update(ofPoint pos, int cornerCount, ofPoint currentCentroid) {
    if (ofGetFrameNum() - previousFrameCount > 15) {
        isSuspendEvent = true;
            gestureCount = 10;
	} else {
        isSuspendEvent = false;
        gestureCount = max(0, --gestureCount);
    }
    previousFrameCount = ofGetFrameNum();
    currentTmpPos = getCurrentPos(pos);
    if (handMode == HAND_MODE_NORMAL) {         
        if (!checkClick(cornerCount)) {
            setPos(currentTmpPos);
            MouseMove();
            checkSpeedMove();
        }
    } else if (handMode == HAND_MODE_MOVE) {
        setPos(currentTmpPos);
        MouseMove();
        checkSpeedMove();
    } else if (handMode == HAND_MODE_DRAG) {
        if (!checkClick(cornerCount)) {
            setPos(currentTmpPos);
            MouseDrag();
        }
    } else if (handMode == HAND_MODE_CLICK) {           
        checkClick(cornerCount);
    }
}

//--------------------------------------------------------------
bool Hand::checkMoveSpeed() {
    if (posHistory.size() < POSITION_HISTORY_SIZE) {
		false;
    }	
    float x = 0;
    float y = 0;
    for (int j = 0; j < posHistory.size()-1; j++) {
        x += posHistory.at(j).x - posHistory.at(j+1).x;
        y += posHistory.at(j).y - posHistory.at(j+1).y;
    }
    if (gestureCount == 0) {
        bool suspend = false;
        if (x > 40) {
            //SWIPE LEFT
            suspend = true;
            CGEventRef keyEv = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)123, true);
            CGEventPost (kCGHIDEventTap, keyEv);
        } else if (x < -40) {
            //SWIPE RIGHT
            suspend = true;
            CGEventRef keyEv = CGEventCreateKeyboardEvent(NULL, (CGKeyCode)124, true);
            CGEventPost (kCGHIDEventTap, keyEv);
        } else if (y > 30) {
            //SCROLL UP
            CGPoint pt = calcMousePosition();
            CGEventRef wheelUpEv = CGEventCreateScrollWheelEvent (NULL,kCGScrollEventUnitPixel,1,60);
            CGEventPost (kCGHIDEventTap, wheelUpEv);
        } else if (y < -30){
            //SCROLL DOWN
            CGPoint pt = calcMousePosition();
            CGEventRef wheelUpEv = CGEventCreateScrollWheelEvent (NULL,kCGScrollEventUnitPixel,1,-60);
            CGEventPost (kCGHIDEventTap, wheelUpEv);
        }
        if (suspend) {
            gestureCount = GESTURE_BUFFER;
        }
    }	
    if (abs(x) + abs(y) > 12) {
        handMode = HAND_MODE_MOVE;
        toNormalModeCount = 0;
        return true;
    }
    if (handMode == HAND_MODE_MOVE) {
        toNormalModeCount++;
        if (toNormalModeCount > 10) {
            handMode = HAND_MODE_NORMAL;
            return false;
        }
        return true;
    }
    handMode = HAND_MODE_NORMAL;
    return false;
}

//--------------------------------------------------------------
bool Hand::checkClick(int cornerCount) {
    cornerCountHistory.pushBack(cornerCount);
    if (cornerCountHistory.size() > 6) {
        cornerCountHistory.erase(cornerCountHistory.begin());
    } else {
        return false;
    }
    int oldCornerNums = 0;
    int cornerNums = 0;
    for (int i=0; i<cornerCountHistory.size(); i++) {
        if (i < 4) {
            oldCornerNums += cornerCountHistory[i];
        } else {
            cornerNums += cornerCountHistory[i];
        }
    }
    oldCornerNums = oldCornerNums/4;
    cornerNums = cornerNums/2;
    if (handMode == HAND_MODE_NORMAL && cornerNums + 150 < oldCornerNums) {
        currentCornerNums = cornerNums;
        handMode = HAND_MODE_CLICK;
        mouseDownCount = 0;
        return true;
    }
    if (cornerNums > currentCornerNums + 150) {
        if (handMode == HAND_MODE_DRAG) {
            MouseUp();
            soundClick.play();
            handMode = HAND_MODE_NORMAL;
            return true;
        } else if (handMode == HAND_MODE_CLICK) {
            MouseClick();
            soundClick.play();
            handMode = HAND_MODE_NORMAL;
            return true;
        }
    }
    if (handMode == HAND_MODE_CLICK) {
        mouseDownCount++;
        if (mouseDownCount > MOUSE_CLICK_FRAME) {
            handMode = HAND_MODE_DRAG;
            MouseDown();
            soundClick.play();
            mouseDownCount = 0;
        }
    }
    return false;
}

//--------------------------------------------------------------
CGPoint Hand::calcCurrentPosition() {
    float x = currentPos.x;
    float y = currentPos.y;
    
    x = max(0.0, x*100.0/640.0-15.0f);
    x = min(x, 70.0f)/70*100;
    y = max(0.0, y*100.0/480.0-10.0f);
    y = min(y, 70.0f)/70*100;
	

    CGPoint pt;
    pt.x = x/100*displayWidth;
    pt.y = y/100*displayHeight;
    return pt;
}

//--------------------------------------------------------------
void Hand::unregister() {
    if (handMode == HAND_MODE_DRAG) {
        MouseUp();
    }
    isActive = false;
    handMode = HAND_MODE_NORMAL;
}

//--------------------------------------------------------------
ofPoint Hand::getCurrentPos(ofPoint newPos) {
	if (posHistory.size() == 0) {
		return newPos;
	}
	float x = newPos.x;
	float y = newPos.y;
	for (int j = 0; j < posHistory.size(); j++) {
		x += posHistory.at(j).x;
		y += posHistory.at(j).y;
	}
	x = x/(posHistory.size()+1);
	y = y/(posHistory.size()+1);
	return ofPoint(x, y);	
}

//--------------------------------------------------------------
void Hand::setPos(ofPoint pos) {
    currentPos = pos;
    posHistory.push_back(pos);
    if (posHistory.size() > POSITION_HISTORY_SIZE) {
        posHistory.erase(posHistory.begin());
    }	
}

//--------------------------------------------------------------
void Hand::setIsActive(bool active) {
    isActive = active;
}

//--------------------------------------------------------------
ofPoint Hand::getPos() {
    return currentPos;
}

//--------------------------------------------------------------
bool Hand::getIsPrimary() {
    return isPrimary;
}

//--------------------------------------------------------------
void Hand::MouseMove() {
    CGPoint pt = calcMousePosition();
    CGEventRef mouseMoveEv = CGEventCreateMouseEvent(NULL, kCGEventMouseMoved, pt, kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, mouseMoveEv);
}

//--------------------------------------------------------------
void Hand::MouseDown() {
    CGPoint pt = calcMousePosition();
    CGEventRef mouseDownEv = CGEventCreateMouseEvent(NULL,kCGEventLeftMouseDown,pt,kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, mouseDownEv);
}

//--------------------------------------------------------------
void Hand::MouseUp() {
    CGPoint pt = calcMousePosition();
    CGEventRef mouseUpEv = CGEventCreateMouseEvent(NULL,kCGEventLeftMouseUp,pt,kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, mouseUpEv );				
}

//--------------------------------------------------------------
void Hand::MouseDrag() {
    CGPoint pt = calcMousePosition();
    CGEventRef mouseDragEv = CGEventCreateMouseEvent(NULL,kCGEventLeftMouseDragged,pt,kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, mouseDragEv );					
}

//--------------------------------------------------------------
void Hand::MouseClick() {
    fireMouseDown();
    fireMouseUp();
}
