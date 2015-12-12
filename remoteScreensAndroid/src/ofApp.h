#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"
#include "ofxHTTP.h"
#include "ofxOsc.h"
#include "ofxAccelerometer.h"
#include "ofxTiming.h"
class ofApp : public ofxAndroidApp{

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void windowResized(int w, int h);

	void touchDown(int x, int y, int id);
	void touchMoved(int x, int y, int id);
	void touchUp(int x, int y, int id);
	void touchDoubleTap(int x, int y, int id);
	void touchCancelled(int x, int y, int id);
	void swipe(ofxAndroidSwipeDir swipeDir, int id);

	void pause();
	void stop();
	void resume();
	void reloadTextures();

	bool backPressed();
	void okPressed();
	void cancelPressed();


	ofVideoGrabber grabber;
	int one_second_time;
	int camera_fps;
	int frames_one_sec;
	ofPixels* pix;
//	vector<ofImage*> cameraImages;
	ofImage* image;
	ofx::HTTP::SimpleIPVideoServer server;
	ofxOscSender sender;
	ofxOscReceiver receiver;
	int cameraID;
	int preCameraID;
	bool drawConnections;
	int camWidth, camHeight;

	bool cameraChange;


	ofTrueTypeFont font;
	ofVec3f accel, normAccel;
	string messages[3];

	vector<ofRectangle> rects;

	bool bTouch;
	bool bPtouch;
	bool facing;
	int orientation;

	float mFade;
	FadeTimer fader;

	float mFadeDelay;
	FadeTimer faderDelay;

};
