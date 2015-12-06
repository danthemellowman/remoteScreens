#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    ofGLFWWindowSettings settings;
    settings.numSamples = 4;
    settings.width = 1920;
    settings.height = 1080;
    shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);
    guiWindow->setVerticalSync(true);
    
    shared_ptr<ofApp> mainApp(new ofApp);

    
    ofRunApp(guiWindow, mainApp);
    ofRunMainLoop();

}
