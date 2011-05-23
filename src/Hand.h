#pragma once

#include "ofxVec3f.h"
#include "ofxCvBlob.h"
#include "Finger.h"

typedef enum {LeftHand, RightHand} HandType;

class Hand : public ofxVec3f {
private:
    Finger fingers[5];
    HandType type;
public:
    void SetFinger(FingerType finger, int x, int y, int z);
    HandType getType() {return type;}
    void setType(HandType h) {type = h;}
    
    static bool isHand(ofxCvBlob *blob) {return true;}
};