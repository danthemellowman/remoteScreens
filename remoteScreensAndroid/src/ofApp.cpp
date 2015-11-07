#include "ofApp.h"
#include "ofxAndroidVideoGrabber.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofDisableArbTex();
	ofxAccelerometer.setup();
	ofBackground(255,255,255);
	ofSetLogLevel(OF_LOG_NOTICE);
	ofSetOrientation(OF_ORIENTATION_DEFAULT);
	preCameraID = cameraID = 0;
	camWidth = 720;
	camHeight = 480;
	grabber.setDeviceID(cameraID);
	grabber.setup(camWidth, camHeight);

	ofx::HTTP::SimpleIPVideoServerSettings settings;

	// Many other settings are available.
	settings.setPort(7890);

	// The default maximum number of client connections is 5.
	settings.ipVideoRouteSettings.setMaxClientConnections(6);

	// Apply the settings.
	server.setup(settings);

	// Start the server.
	server.start();

	pix = new ofPixels();
	pix->allocate(camWidth, camHeight, 3);
	image = new ofImage();
	image->allocate(camWidth, camHeight, OF_IMAGE_COLOR);


	drawConnections = false;
}

//--------------------------------------------------------------
void ofApp::update(){
	accel = ofxAccelerometer.getForce();
	messages[0] = "g(x) = " + ofToString(accel.x,2);
	messages[1] = "g(y) = " + ofToString(accel.y,2);
	messages[2] = "g(z) = " + ofToString(accel.z,2);
	normAccel = accel.getNormalized();

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
		grabber.close();
		ofLog()<<"grabber.setDeviceID()"<<endl;
		grabber.setDeviceID(cameraID);
		ofLog()<<"grabber.setup()"<<endl;
		grabber.setup(camWidth, camHeight);
		preCameraID = cameraID;
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofBackground(0, 0, 0);
	ofSetHexColor(0xFFFFFF);
	float width = ofGetHeight();
	float height = width*image->getHeight()/image->getWidth();

	if(!cameraChange){
		ofPushMatrix();
		{
			ofTranslate(ofGetWidth()/2.0, ofGetHeight()/2.0);
			ofPushMatrix();
			{
				ofRotate(90);
				grabber.draw(-ofGetHeight()/2, -ofGetWidth()/2, width, height);
			}
			ofPopMatrix();
		}
		ofPopMatrix();
	}else{
		ofSetColor(255, 0, 255);
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	}

	ofDrawBitmapStringHighlight("FPS "+ofToString(ofGetFrameRate()), 10, 10);

	ofSetColor(255,255,0);
	font.drawString(messages[0],10,font.stringHeight(messages[0])+30);
	ofSetColor(255,0,255);
	font.drawString(messages[1],10,(font.stringHeight(messages[0])+30)*2);
	ofSetColor(0,255,255);
	font.drawString(messages[2],10,(font.stringHeight(messages[0])+30)*3);

	ofPushMatrix();
	ofTranslate(ofGetWidth()/2, ofGetHeight()/2);

	ofSetColor(255,255,0);
	ofDrawLine(0,0,normAccel.x*ofGetWidth()/2,0);
	ofSetColor(255,0,255);
	ofDrawLine(0,0,0,-normAccel.y*ofGetHeight()/2);
	// we don't draw z as the perspective might be confusing
	// but it's approximately one when the device is still and parallel
	// to the ground
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ 

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){ 

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::touchDown(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchMoved(int x, int y, int id){

}

//--------------------------------------------------------------
void ofApp::touchUp(int x, int y, int id){
	drawConnections = !drawConnections;
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
