#pragma once

#include "ofxVec3f.h"
#include "Finger.h"

class Hand : public ofxVec3f {
private:
    Finger fingers[5];
public:
    void SetFinger(FingerType finger, int x, int y, int z);
};