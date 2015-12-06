#include "ofApp.h"



void ofApp::setup(){
    ofSetVerticalSync(true);
    ofSetFrameRate(60);
    //    ofEnableSmoothing();
    //    ofEnableAntiAliasing();
    
    numRects =6;
    fboWidth = 1024;
    fboHeight = 1024;
    textureWidth = fboWidth/(numRects/2);
    textureHeight = textureWidth;
    reCreateRects();
    
    
    
    lerps.add(rectLerp.set("R-Lerp", 0.9, 0.001, 1.0));
    lerps.add(targetLerp.set("T-Lerp", 0.1, 0.001, 1.0));
    lerps.add(scaleOutputMin.set("Scale Min", 0.5, 0.001, 1.0));
    lerps.add(scaleOutputMax.set("Scale Max", 0.5, 0.001, 1.0));
    lerps.add(debugView.set("debugView",false));
    panel.setup(lerps);
    panel.loadFromFile("Settings.xml");
    numExpanded = 0;
    
    ofSetFullscreen(true);
    loadCameras();
    fbo.allocate(ofGetScreenWidth(), ofGetScreenHeight(), GL_RGBA32F);
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
    
    std::shared_ptr<Video::IPVideoGrabber> c = std::make_shared<Video::IPVideoGrabber>();
    
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
    newCamera("phone1", "http://phone0.wv.cc.cmu.edu:7890/ipvideo");
    newCamera("phone2", "http://phone1.wv.cc.cmu.edu:7890/ipvideo");
}

//------------------------------------------------------------------------------
void ofApp::videoResized(const void* sender, ofResizeEventArgs& arg)
{
    // find the camera that sent the resize event changed
    for(std::size_t i = 0; i < grabbers.size(); i++)
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



void ofApp::reCreateRects(){
    
    rectangles.clear();
    
    int x = (fboWidth - floor(fboWidth/textureWidth)*textureWidth)/2;
    int y = 0;
    
    // create a random set of rectangles to play with
    rects.resize(numRects);
    for(int i = 0; i < numRects; ++i){
        
        rects[i] = new ColoredRectangle();
        ((ColoredRectangle*)rects[i])->color = ofColor(ofRandom(32,255), ofRandom(32,255), ofRandom(32,255));
        ((ColoredRectangle*)rects[i])->aspectRatio = textureWidth/textureHeight;
        ((ColoredRectangle*)rects[i])->max.set(0, 0, textureWidth*2, textureWidth*2);
        ((ColoredRectangle*)rects[i])->min.set(0, 0, textureWidth, textureHeight);
        ((ColoredRectangle*)rects[i])->targetRect = ((ColoredRectangle*)rects[i])->min;
        ((ColoredRectangle*)rects[i])->currentRect = ((ColoredRectangle*)rects[i])->min;
        ((ColoredRectangle*)rects[i])->rect = ((ColoredRectangle*)rects[i])->min;
        ((ColoredRectangle*)rects[i])->alive = true;
        ((ColoredRectangle*)rects[i])->uuid = ofToString(i);
        ((ColoredRectangle*)rects[i])->bExpanded = false;
        
        x+=textureWidth;
        if(x+textureWidth >= fboWidth){
            x = (fboWidth - floor(fboWidth/textureWidth)*textureWidth)/2;
            y+=textureHeight;
        }
        
        rectangles.push_back((ColoredRectangle*)rects[i]);
        targetRects.push_back(new ColoredRectangle());
        targetRects.back()->uuid = ((ColoredRectangle*)rects[i])->uuid;
        targetRects.back()->color = ((ColoredRectangle*)rects[i])->color;
        targetRectMap[targetRects.back()->uuid] = targetRects.back();
        rectMap[((ColoredRectangle*)rects[i])->uuid] = (ColoredRectangle*)rects[i];
        
    }
}


void ofApp::packAll(float padding){
    
    fbos.clear();
    rectsPerFbo.clear();
    
    //demo packing
    ofRectangle bounds = ofRectangle(0,0, fboWidth, fboHeight);
    packer.clear();
    packer.push_back(new ofRectanglePacker(bounds, padding));
    rectsPerFbo.resize(1); //add the 1st one
    
    for (int i = rects.size()-1; i >=0; --i) {
        bool didFit = false;
        for(int j = 0; j < packer.size() && !didFit; j++){
            didFit = packer[j]->pack( ((ColoredRectangle*)rects[i])->currentRect);
            if(didFit){
                ((ColoredRectangle*)rects[i])->screenOffset = j;
            }
            
        }
        if(!didFit){
            i++;
            packer.push_back(new ofRectanglePacker(bounds, padding));
        }
    }
    
    for(int i =0; i < packer.size(); i++){
        packer.erase(packer.begin()+i);
        delete packer[i];
    }
}



void ofApp::update(){
    
    // update the cameras
    for(std::size_t i = 0; i < grabbers.size(); i++)
    {
        grabbers[i]->update();
    }
    
    float totalArea = fboWidth*fboHeight;
    int offset = numExpanded<numRects?numExpanded+1:numRects;
    float expandedHeight = floor(sqrt(totalArea/(offset)));
    float expandedArea = expandedHeight*(expandedHeight/1.7)*(numExpanded==0?0:offset);
    float height = sqrt((totalArea-expandedArea)/(numRects));
    
    
    for(int i = 0; i < rectangles.size(); i++){
        if(rectangles[i]->bExpanded){
            ofRectangle fooRect = ofRectangle(0, 0, expandedHeight/1.7, expandedHeight);
            rectangles[i]->targetRect = fooRect;
        }else{
            
            rectangles[i]->targetRect = ofRectangle(0, 0, height/1.7, height);
            
        }
        
        rectangles[i]->currentRect.set(targetRects[i]->currentRect.x, targetRects[i]->currentRect.y, ofLerp(rectangles[i]->currentRect.width, rectangles[i]->targetRect.width, rectLerp), ofLerp(rectangles[i]->currentRect.height, rectangles[i]->targetRect.height, rectLerp));
        rectangles[i]->set(rectangles[i]->currentRect);
        
    }
    
    
    
    
    
    packAll(0);
    
    ofx::RectangleUtils::sortByHeight(rects);
    
    
    for(int i = 0; i < targetRects.size(); i++){
        targetRects[i]->currentRect.set(ofLerp(targetRects[i]->currentRect.x, rectangles[i]->currentRect.x+rectangles[i]->screenOffset*fboWidth, targetLerp), ofLerp(targetRects[i]->currentRect.y, rectangles[i]->currentRect.y, targetLerp), ofLerp(targetRects[i]->currentRect.width, rectangles[i]->targetRect.width, targetLerp), ofLerp(targetRects[i]->currentRect.height, rectangles[i]->targetRect.height, targetLerp));
    }
}
void ofApp::mouseMoved(int x, int y ){
    
}
void ofApp::mouseDragged(int x, int y, int button){
    
}
void ofApp::mousePressed(int x, int y, int button){
    
}
void ofApp::mouseReleased(int x, int y, int button){
    
}

void ofApp::draw(){
    
    ofBackground(0);
    ofSetColor(255);
    
    fbo.begin();
    ofClear(0, 0, 0, 0);
    ofPushMatrix();
    {
        ofTranslate(fbo.getWidth()/2, fbo.getHeight()/2);
        ofScale(ofMap(numExpanded, 0, numRects, scaleOutputMin, scaleOutputMax), ofMap(numExpanded, 0, numRects, scaleOutputMin, scaleOutputMax));
        for(int j = 0; j < 4; j++){
            ofPushMatrix();
            {
                if(j == 1){
                    ofRotateX(180);
                }
                if(j == 2){
                    ofRotateX(180);
                    ofRotateY(180);
                }
                if(j == 3){
                    ofRotateX(180);
                    ofRotateY(180);
                    ofRotateX(180);
                }
                
                //                ofRotateZ(90*i);
                int fooCamera = 0;
                for (int i = rects.size()-1; i>=0; i--){
                    if(i%2==0)fooCamera = 1;
                    else fooCamera = 0;
                    ColoredRectangle & rect = *targetRectMap[((ColoredRectangle*)rects[i])->uuid];
                    
                    ofPushMatrix();
                    {
                        ofSetColor(255, 255, 255, 255);
                        ofFill();
                        ofTranslate(rect.currentRect.x, rect.currentRect.y);
                        ofPushMatrix();
                        {
                            ofRotate(90);
                            
                            ofTranslate(rect.currentRect.getHeight()/2, -rect.currentRect.getWidth()/2);
                            if(j==0 || j == 3){
                                grabbers[fooCamera]->draw(rect.currentRect.getHeight()/2,-rect.currentRect.getWidth()/2, -rect.currentRect.getHeight(), rect.currentRect.getWidth());
                            }
                            if(j == 1 || j == 2){
                                
                                grabbers[fooCamera]->draw(-rect.currentRect.getHeight()/2,-rect.currentRect.getWidth()/2, rect.currentRect.getHeight(), rect.currentRect.getWidth());
                                
                            }
                            
                        }
                        ofPopMatrix();
                        if(debugView){
                            ofNoFill();
                            if(rectMap[((ColoredRectangle*)rects[i])->uuid]->bExpanded) ofSetColor(255, 0, 255, 255);
                            else ofSetColor(255, 255, 0, 255);
                            ofDrawRectangle(0,0,rect.currentRect.width, rect.currentRect.height);
                        }
                    }
                    ofPopMatrix();
                }
                
            }
            ofPopMatrix();
            
            
        }
    }
    ofPopMatrix();
    fbo.end();
    
    fbo.draw(0, 0);
    
    if(debugView){
        ofEnableAlphaBlending();
        ofSetColor(0, 0, 0, 200);
        ofPushMatrix();
        ofScale(0.5, 0.5);
        for (int i = rects.size()-1; i>=0; i--){
            ColoredRectangle & rect = *targetRectMap[((ColoredRectangle*)rects[i])->uuid];
            if(rectMap[((ColoredRectangle*)rects[i])->uuid]->bExpanded) ofSetColor(255, 0, 255, 255);
            else ofSetColor(255, 255, 0, 255);
            ofNoFill();
            ofPushMatrix();
            
            ofDrawRectangle(rect.currentRect);
            if(rectMap[((ColoredRectangle*)rects[i])->uuid]->bExpanded) ofSetColor(255, 0, 255, 100);
            else ofSetColor(255, 255, 0, 100);
            ofFill();
            ofDrawRectangle(rect.currentRect);
            ofPopMatrix();
        }
        ofPopMatrix();
        ofDisableAlphaBlending();
        ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate(),1), 10, 10);
    }
    
    
    panel.draw();
}


void ofApp::keyPressed(int key){
    int expandScreen = -1;
    switch (key) {
        case '1':
            expandScreen = 0;
            break;
        case '2':
            expandScreen = 1;
            break;
        case '3':
            expandScreen = 2;
            break;
        case '4':
            expandScreen = 3;
            break;
        case '5':
            expandScreen = 4;
            break;
        case '6':
            expandScreen = 5;
            break;
    }
    if(expandScreen != -1){
        if(!rectangles[expandScreen]->bExpanded){
            rectangles[expandScreen]->bExpanded = true;
            numExpanded++;
        }else{
            rectangles[expandScreen]->bExpanded = false;
            numExpanded--;
            if(numExpanded < 0){
                numExpanded = 0;
            }
        }
    }else if(key == ' '){
        for(int i = 0; i < targetRects.size(); i++){
            rectangles[i]->bExpanded = false;
            
        }
        numExpanded = 0;
    }
}

