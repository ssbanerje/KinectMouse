#pragma once

#include "ofxVec3f.h"

typedef enum {THUMB, INDEX, MIDDLE, RING, SMALL} FingerType;

class Finger : public ofxVec3f {
private:
    FingerType type;
public:
    void setType(FingerType t) {type = t;}
    FingerType getType() {return type;}
};