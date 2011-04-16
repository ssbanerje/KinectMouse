#include "ofMain.h"
#include "ofAppGlutWindow.h"

#include "KinectMouse.h"
  
//========================================================================
int main(int argc, const char *argv[]){
    ofAppGlutWindow window;
	ofSetupOpenGL(&window,1024,768,OF_WINDOW);
	ofRunApp(new KinectMouse());
}
