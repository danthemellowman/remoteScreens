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


#pragma once

#include "ofMain.h"
#include "ofxRectangleUtils.h"
#include "ofxRectangle.h"
#include "ofxGui.h"
#include "ofxTween.h"
#include "IPVideoGrabber.h"

class ColoredRectangle : public ofRectangle{
public:
    ofRectangle rect;
    ofRectangle targetRect;
    ofRectangle currentRect;
    ofRectangle min;
    ofRectangle max;
    int screenOffset;
    float aspectRatio;
    bool alive;
    bool bExpanded;
    ofColor color;
    string uuid;
};

class IPCameraDef
{
public:
    IPCameraDef()
    {
    }
    
    IPCameraDef(const std::string& url): _url(url)
    {
    }
    
    IPCameraDef(const std::string& name,
                const std::string& url,
                const std::string& username,
                const std::string& password):
    _name(name),
    _url(url),
    _username(username),
    _password(password)
    {
    }
    
    void setName(const std::string& name) { _name = name; }
    std::string getName() const { return _name; }
    
    void setURL(const std::string& url) { _url = url; }
    std::string getURL() const { return _url; }
    
    void setUsername(const std::string& username) { _username = username; }
    std::string getUsername() const { return _username; }
    
    void setPassword(const std::string& password) { _password = password; }
    std::string getPassword() const { return _password; }
    
    
private:
    std::string _name;
    std::string _url;
    std::string _username;
    std::string _password;
};

using namespace ofx;


class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();

	void keyPressed(int key);
	void keyReleased(int key){};
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);

	void reCreateRects();
	void packAll(float padding);
    
    IPCameraDef& getNextCamera();
    void newCamera(string name, string ip);
    void loadCameras();
    void videoResized(const void* sender, ofResizeEventArgs& arg);
    std::vector<std::shared_ptr<Video::IPVideoGrabber>> grabbers;
    std::vector<IPCameraDef> ipcams; // a list of IPCameras
    int nextCamera;
    
    
    bool bTogglerectangles;
  


	vector<ofRectanglePacker*> packer;
    map<string, ColoredRectangle*> targetRectMap;
    map<string, ColoredRectangle*> rectMap;
	vector<ofFbo> fbos;
	vector<ColoredRectangle*> rectangles;
    vector<ofRectangle*> rects;
    vector<ColoredRectangle*> targetRects;
	vector< vector<ColoredRectangle*> > targetrectsPerFbo;
    vector< vector<ColoredRectangle*> > rectsPerFbo;

    
    vector<ofxTween> tweeners;
    
    
    ofxEasingBounce easeBounce;
    ofxEasingCubic easeCubic;
    ofxEasingElastic easeElastic;
    ofxEasingLinear easeLinear;
    ofxEasingQuart easeQuart;
    ofxEasingBack easeBack;
    ofxEasingCirc easeCirc;
    ofxEasingExpo easeExpo;
    ofxEasingQuad easeQuad;
    ofxEasingQuint easeQuint;
    ofxEasingSine easeSine;
    
    float clamp;
    ofxTween::ofxEasingType easingType;
    string easestring;

    
    
	int padding;
    
    ofxPanel panel;
    ofParameterGroup lerps;
    ofParameter<float> rectLerp;
    ofParameter<float> targetLerp;
    ofParameter<float> scaleOutputMin, scaleOutputMax;
    ofParameter<bool> debugView;
    int numExpanded;
    int numRects;
    float fboWidth;
    float fboHeight;
    float textureHeight;
    float textureWidth;
    
    ofFbo fbo;
};
