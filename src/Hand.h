#ifndef _HAND_APP_INC
#define _HAND_APP_INC

#include "ofMain.h"

#define MOUSE_CLICK_FRAME 8
#define HAND_MODE_NORMAL 0
#define HAND_MODE_MOVE 1
#define HAND_MODE_CLICK 2
#define HAND_MODE_DRAG 3
#define POSITION_HISTORY_SIZE 4
#define GESTURE_BUFFER 20

class Hand {
public:
    Hand(bool isPrimary, int dispWidth, int dispHeight);
    ~Hand();
    
    void update(ofPoint currentPoint, int cornerCount, ofPoint currentCentroid); //Update current mouse status
    ofPoint getCurrentPos(ofPoint currentPoint); //Get mean of positions on history vector
    void setPos(ofPoint currentPoint); //Set position for mouse
    void unregister(); //This Hand object no longer controls mouse
    ofPoint getPos();
    bool getIsPrimary();
    bool dead;
    
private:
    bool isActive;
    bool isPrimary;
    
    //Mouse functions
    void MouseClick();
    void MouseDown();
    void MouseUp();
    void MouseDrag();
    void MouseMove();
    
    CGPoint calcMousePosition(); //Get position of primary hand in image and map to screen coordinates
    
    //Mouse position
    ofPoint currentPos;
    ofPoint currentTmpPos;
    ofPoint clickedPos;
    vector<ofPoint> posHistory;
    vector<ofPoint> adjustPosHistory;
    
    //Click
    int handMode;
    int mouseDownCount;
    int toNormalModeCount;
    int gestureCount;
    int currentCornerNo;
    vector<int> cornerCountHistory;
    bool checkClick(int cornerCount); //Check if click has happened
    bool checkSpeedMove(); //Check and interpret move speed for movement
    
    //Referecences
    Hand *primaryHand;
    int dispHeight;
    int dispWidth;
    float previousFrameCount;
    bool isSuspendEvent;
    
    //Sounds
    ofSoundPlayer soundClick;
};



#endif