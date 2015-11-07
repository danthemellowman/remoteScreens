#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0, 0, 0);
    int x = (ofGetWidth() - floor(ofGetWidth()/480.0)*480)/2;
    int y = 0;
    for(int i = 0; i < 8; i++){
        VideoFrame foo;
        foo.aspectRatio = 640/480;
        foo.max.set(0, 0, 480, 640);
        foo.min.set(0, 0, 48, 64);
        foo.targetRect.set(x, y, 480, 640);
        foo.currentRect.set(x+foo.targetRect.width/2, y+foo.targetRect.height/2, 0, 0);
        foo.alive = true;
        frames.push_back(foo);
        x+=480;
        if(x+480 >= ofGetWidth()){
            x = (ofGetWidth() - floor(ofGetWidth()/480.0)*480)/2;
            y+=640;
        }
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    for(int i = 0; i < frames.size(); i++){
        if(frames[i].alive){
            if(frames[i].currentRect  == frames[i].targetRect){
                frames[i].alive = false;
            }else{
                frames[i].currentRect.set(ofLerp(frames[i].currentRect.x, frames[i].targetRect.x, 0.05), ofLerp(frames[i].currentRect.y, frames[i].targetRect.y, 0.05), ofLerp(frames[i].currentRect.width, frames[i].targetRect.width, 0.05), ofLerp(frames[i].currentRect.height, frames[i].targetRect.height, 0.05));
            }
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofPushMatrix();
//    ofScale(0.5, 0.5);
    for(int i = 0; i < frames.size(); i++){
        ofSetColor(255/(i+1), 0, 255/(i+1));
        ofRect(frames[i].currentRect);
    }
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    bToggleFrames = !bToggleFrames;
    if(bToggleFrames){
        for(int i = 0; i < frames.size(); i++){
            frames[i].alive = true;
            frames[i].targetRect.set(frames[i].targetRect.x+frames[i].targetRect.width/2, frames[i].targetRect.y+frames[i].targetRect.height/2, 0, 0);
        }
    }else{
        int x = (ofGetWidth() - floor(ofGetWidth()/480.0)*480)/2;
        int y = 0;
        for(int i = 0; i < frames.size(); i++){
            frames[i].alive = true;
            frames[i].targetRect.set(x, y, 480, 640);
            x+=480;
            if(x+480 >= ofGetWidth()){
                x = (ofGetWidth() - floor(ofGetWidth()/480.0)*480)/2;
                y+=640;
            }
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
