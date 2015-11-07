// =============================================================================
//
// Copyright (c) 2009-2013 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================


#include "ofApp.h"


//------------------------------------------------------------------------------
void ofApp::setup()
{
    ofSetLogLevel(OF_LOG_NOTICE);
    ofSetFrameRate(30);
    loadCameras();
    
    server.setName("Screen");
}

//------------------------------------------------------------------------------
IPCameraDef& ofApp::getNextCamera()
{
    nextCamera = (nextCamera + 1) % ipcams.size();
    return ipcams[nextCamera];
}


void ofApp::newCamera(string name, string ip){
    IPCameraDef def(name, ip, "", "");
    
    std::string logMessage = "STREAM LOADED: " + def.getName() +
    " url: " +  def.getURL() +
    " username: " + def.getUsername() +
    " password: " + def.getPassword();
    
    ofLogNotice() << logMessage;
    
    ipcams.push_back(def);
    
    SharedIPVideoGrabber c = IPVideoGrabber::makeShared();
    
    c->setCameraName(def.getName());
    c->setURI(def.getURL());
    c->connect(); // connect immediately
    
    // if desired, set up a video resize listener
    ofAddListener(c->videoResized, this, &ofApp::videoResized);
    
    grabbers.push_back(c);
    
    nextCamera = ipcams.size();
}

//------------------------------------------------------------------------------
void ofApp::loadCameras()
{
    newCamera("phone1", "http://128.237.161.251:7890/ipvideo");
    newCamera("phone2", "http://128.237.198.49:7890/ipvideo");
}

//------------------------------------------------------------------------------
void ofApp::videoResized(const void* sender, ofResizeEventArgs& arg)
{
    // find the camera that sent the resize event changed
    for(std::size_t i = 0; i < NUM_CAMERAS; i++)
    {
        if(sender == &grabbers[i])
        {
            std::stringstream ss;
            ss << "videoResized: ";
            ss << "Camera connected to: " << grabbers[i]->getURI() + " ";
            ss << "New DIM = " << arg.width << "/" << arg.height;
            ofLogVerbose("ofApp") << ss.str();
        }
    }
}


//------------------------------------------------------------------------------
void ofApp::update()
{
    // update the cameras
    for(std::size_t i = 0; i < grabbers.size(); i++)
    {
        grabbers[i]->update();
    }
}

//------------------------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0,0,0);
    
    ofSetHexColor(0xffffff);
    
    int row = 0;
    int col = 0;
    
    int x = 0;
    int y = 0;
    
    int w = ofGetWidth() / NUM_COLS;
    int h = ofGetHeight() / NUM_ROWS;

    
    for(std::size_t i = 0; i < grabbers.size(); i++)
    {
        x = col * w;
        y = row * h;
        
        // draw in a grid
        row = (row + 1) % NUM_ROWS;
        
        if(row == 0)
        {
            col = (col + 1) % NUM_COLS;
        }
        
        
        ofPushMatrix();
        ofTranslate(x,y);
        ofSetColor(255,255,255,255);
        float width = w;
        float height = width*grabbers[i]->getHeight()/grabbers[i]->getWidth();
        grabbers[i]->draw(0, 0,width,height); // draw the camera
        ofPopMatrix();
    }
    
    
}

//------------------------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if(key == ' ')
    {
        // initialize connection
        for(std::size_t i = 0; i < NUM_CAMERAS; i++)
        {
            ofRemoveListener(grabbers[i]->videoResized, this, &ofApp::videoResized);
            SharedIPVideoGrabber c = IPVideoGrabber::makeShared();
            IPCameraDef& cam = getNextCamera();
            c->setUsername(cam.getUsername());
            c->setPassword(cam.getPassword());
            Poco::URI uri(cam.getURL());
            c->setURI(uri);
            c->connect();
            
            grabbers[i] = c;
        }
    }
}
