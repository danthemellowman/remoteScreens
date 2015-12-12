#include "ofApp.h"
#include "ofxAndroidVideoGrabber.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofDisableArbTex();
	ofxAccelerometer.setup();
	ofSetVerticalSync(false);
	ofBackground(0,0,0);
	ofSetLogLevel(OF_LOG_NOTICE);
	ofSetOrientation(OF_ORIENTATION_DEFAULT);
	preCameraID = cameraID = 0;
	camWidth = 640;
	camHeight = 480;
	grabber.setDeviceID(cameraID);
	grabber.setup(camWidth, camHeight);

	ofx::HTTP::SimpleIPVideoServerSettings settings;

	// Many other settings are available.
	settings.setPort(7890);

	// The default maximum number of client connections is 5.
	settings.ipVideoRouteSettings.setMaxClientConnections(2);

	// Apply the settings.
	server.setup(settings);

	// Start the server.
	server.start();

	pix = new ofPixels();
	pix->allocate(camWidth, camHeight, 3);
	image = new ofImage();
	image->allocate(camWidth, camHeight, OF_IMAGE_COLOR);

	sender.setup("REMOTEVIEWER.WV.CC.CMU.EDU", 7777);
//	sender.setup("10.0.0.5", 7777);
	//	receiver.setup(7776);
	drawConnections = false;

	bPtouch = false;
	bTouch = false;

	fader.setLength(0.5, 0.5);
	faderDelay.setLength(1.55, 0);

}



//--------------------------------------------------------------
void ofApp::update(){
	grabber.update();
	if(grabber.isFrameNew()){
		if(cameraChange){
			cameraChange = false;
		}
		ofSetColor(255, 255, 255);
		image->setFromPixels(grabber.getPixels());
		server.send(image->getPixels());
	}
	if(cameraID != preCameraID){
		cameraChange = true;

		preCameraID = cameraID;
		if(fader.getActive()){
			fader.stop();
		}
		fader.start();
	}


	mFade = fader.get();
	mFadeDelay = faderDelay.get();
	if(fader.getActive() && mFade == 1.0 && !faderDelay.getActive()){
		faderDelay.start();
		grabber.close();
		grabber.setDeviceID(cameraID);
		grabber.setup(camWidth, camHeight);

	}
	if(faderDelay.getActive() && mFadeDelay == 1.0){
		fader.stop();
		faderDelay.stop();
	}


	accel = ofxAccelerometer.getForce();
	messages[0] = "g(x) = " + ofToString(accel.x,2);
	messages[1] = "g(y) = " + ofToString(accel.y,2);
	messages[2] = "g(z) = " + ofToString(accel.z,2);
	normAccel = accel.getNormalized();

	ofxOscMessage movements;
	movements.setAddress("/movement");
	movements.addStringArg("4");
	movements.addFloatArg(normAccel.x);
	movements.addFloatArg(normAccel.y);
	movements.addFloatArg(normAccel.z);
	sender.sendMessage(movements, false);

//	if(bPtouch != bTouch){
		ofxOscMessage recording;
		recording.setAddress("/record");
		recording.addStringArg("4");
		recording.addBoolArg(bTouch);

		sender.sendMessage(recording, false);
//		bPtouch = bTouch;
//	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0, 0, 0);
	float width = ofGetHeight();
	float height = width*image->getHeight()/image->getWidth();


	//	if(!cameraChange){
	ofPushMatrix();
	{
		ofTranslate(ofGetWidth()/2.0, ofGetHeight()/2.0);
		ofPushMatrix();
		{

			if(!fader.getActive()){
				ofSetColor(255, 255, 255, 255);
			}else{
				ofSetColor(255*(1-mFade), 255*(1-mFade), 255*(1-mFade), 255*(1-mFade));
			}
			ofRotate(90);
			grabber.draw(-ofGetHeight()/2, -ofGetWidth()/2, width, height);
		}
		ofPopMatrix();
	}
	ofPopMatrix();
	//	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){ 

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id){
	bPtouch = bTouch;
	bTouch = true;
}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id){
	bPtouch = bTouch;
	bTouch = false;
}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(int x, int y, int id){
	if(cameraID == 0){
		cameraID = 1;
	}else{
		cameraID = 0;
	}
}

//--------------------------------------------------------------
void ofApp::touchCancelled(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::swipe(ofxAndroidSwipeDir swipeDir, int id){

}

//--------------------------------------------------------------
void ofApp::pause(){
	server.stop();
}

//--------------------------------------------------------------
void ofApp::stop(){
	server.stop();
}

//--------------------------------------------------------------
void ofApp::resume(){
	ofx::HTTP::SimpleIPVideoServerSettings settings;

	// Many other settings are available.
	settings.setPort(7890);

	// The default maximum number of client connections is 5.
	settings.ipVideoRouteSettings.setMaxClientConnections(6);

	// Apply the settings.
	server.setup(settings);

	// Start the server.
	server.start();
}

//--------------------------------------------------------------
void ofApp::reloadTextures(){
	pix = new ofPixels();
	pix->allocate(camWidth, camHeight, 3);
	image = new ofImage();
	image->allocate(camWidth, camHeight, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
bool ofApp::backPressed(){
	return false;
}

//--------------------------------------------------------------
void ofApp::okPressed(){

}

//--------------------------------------------------------------
void ofApp::cancelPressed(){

}
