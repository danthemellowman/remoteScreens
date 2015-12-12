#include "ofApp.h"



void ofApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    //    ofEnableSmoothing();
    //    ofEnableAntiAliasing();
    
    numRects =6;
    fboWidth = 1024;
    fboHeight = 1024;
    textureWidth = fboWidth/(numRects/2);
    textureHeight = textureWidth;
    
    
    
    lerps.add(rectLerp.set("R-Lerp", 0.9, 0.001, 1.0));
    lerps.add(targetLerp.set("T-Lerp", 0.1, 0.001, 1.0));
    lerps.add(scaleOutputMin.set("Scale Min", 0.5, 0.001, 1.0));
    lerps.add(scaleOutputMax.set("Scale Max", 0.5, 0.001, 1.0));
    lerps.add(debugView.set("debugView",false));
    lerps.add(padding.set("paddings", 25, 0, 100));
    lerps.add(aspectRatio.set("aspectRatio", 1.33333333333, 1, 4));
    lerps.add(pBlur.set("Blur", 0.1, 0.0, 1.0));
    
    panel.setup(lerps);
    panel.loadFromFile("Settings.xml");
    numExpanded = 0;
    
    loadCameras();
    
    
    fbo.allocate(1024, 768, GL_RGBA);
    //    fbo.clear();
    fboPing.allocate(1024, 768, GL_RGBA);
    //    fboPing.clear();
    fboPong.allocate(1024, 768, GL_RGBA);
    //    fboPong.clear();
    
    motionBlur.load("shader/shader");
    
    receiver.setup(7777);
    
    //    debugSeq.loadSequence("debug/seq");
    //    debugSeq.preloadAllFrames();
    //    debugSeq.setFrameRate(7);
 
    
    reCreateRects();
    
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
    //    c->setReconnectTimeout(1000);
    grabbers.push_back(c);
    grabbermap[name] = c;
    
    nextCamera = ipcams.size();
}

//------------------------------------------------------------------------------
void ofApp::loadCameras()
{
    ofxXmlSettings camerasSettings;
    if(camerasSettings.load("cameras.xml")){
        camerasSettings.pushTag("settings");
        {
            int numCams = camerasSettings.getNumTags("camera");
            for(int i = 0; i < numCams; i++)
            {
                newCamera(camerasSettings.getValue("camera", "cam"+ofToString(i), i), camerasSettings.getValue("address", "127.0.0.1", i));
            }
        }
        camerasSettings.popTag();
    }else{
        newCamera("0", "http://phone0.wv.cc.cmu.edu:7890/ipvideo");
        newCamera("1", "http://phone1.wv.cc.cmu.edu:7890/ipvideo");
        newCamera("2", "http://phone3.wv.cc.cmu.edu:7890/ipvideo");
        newCamera("3", "http://phone4.wv.cc.cmu.edu:7890/ipvideo");
        newCamera("4", "http://phone5.wv.cc.cmu.edu:7890/ipvideo");
        newCamera("5", "http://phone6.wv.cc.cmu.edu:7890/ipvideo");

    }
    
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
            aspectRatio = arg.width/arg.height;
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
        ((ColoredRectangle*)rects[i])->bRecord = false;
        ((ColoredRectangle*)rects[i])->frameNum = 0;
        
        
        
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
        rectMap[((ColoredRectangle*)rects[i])->uuid] = (ColoredRectangle*)rectangles[i];
        delays[targetRects.back()->uuid] = FadeTimer();
        delays[targetRects.back()->uuid].setLength(0.5, 10);
        delays[targetRects.back()->uuid].stop();
        hyster[targetRects.back()->uuid] = Hysteresis();
        hyster[targetRects.back()->uuid].setDelay(0);
        recorder[targetRects.back()->uuid] = (new ofxImageSequenceRecorder());
        recorder[targetRects.back()->uuid]->setFormat("jpg");
        recordings[targetRects.back()->uuid] = (new ofxImageSequence());
        recordings[targetRects.back()->uuid]->enableThreadedLoad(false);
        recordings[targetRects.back()->uuid]->loadSequence(ofToDataPath("captures/"+ofToString(i)));
        recordings[targetRects.back()->uuid]->preloadAllFrames();
        recordings[targetRects.back()->uuid]->setFrameRate(7);
        
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
    
    for (int i = 0; i < rectangles.size(); i++) {
        bool didFit = false;
        for(int j = 0; j < packer.size() && !didFit; j++){
            didFit = packer[j]->pack(rectangles[i]->currentRect);
            if(didFit){
                rectangles[i]->screenOffset = j;
            }
            
        }
        if(!didFit){
            i--;
            packer.push_back(new ofRectanglePacker(bounds, padding));
        }
    }
    
    for(int i =0; i < packer.size(); i++){
        packer.erase(packer.begin()+i);
        delete packer[i];
    }
    maxw = -1;
    for (int i = 0; i < rectangles.size(); i++) {
        maxw = MAX(maxw, rectangles[i]->currentRect.x+rectangles[i]->currentRect.width);
    }
}



void ofApp::update(){
    float time = ofGetElapsedTimef();
    while(receiver.hasWaitingMessages()){
        ofxOscMessage b;
        receiver.getNextMessage(b);
        if(b.getAddress() == "/record"){
            bool isTouched = b.getArgAsBool(1);
            if(isTouched && !rectMap[b.getArgAsString(0)]->bExpanded){
                rectMap[b.getArgAsString(0)]->bExpanded = true;
                rectMap[b.getArgAsString(0)]->bRecord = true;
                rectMap[b.getArgAsString(0)]->startTime = ofGetElapsedTimef();
                rectMap[b.getArgAsString(0)]->recordingPath = ofToDataPath("captures/"+b.getArgAsString(0));
                rectMap[b.getArgAsString(0)]->frameNum = 0;
                ofDirectory::createDirectory(rectMap[b.getArgAsString(0)]->recordingPath);
                recordingPaths.push_back(rectMap[b.getArgAsString(0)]->recordingPath);
                recorder[b.getArgAsString(0)]->setPrefix(rectMap[b.getArgAsString(0)]->recordingPath+"/frame-");
                if(!recorder[b.getArgAsString(0)]->isThreadRunning())
                    recorder[b.getArgAsString(0)]->startThread();
                
                numExpanded++;
            }
            if(!isTouched && rectMap[b.getArgAsString(0)]->bExpanded){
                rectMap[b.getArgAsString(0)]->bExpanded = false;
                rectMap[b.getArgAsString(0)]->bRecord = false;
                rectMap[b.getArgAsString(0)]->lastActive = ofGetElapsedTimef();
                numExpanded--;
                if(numExpanded < 0){
                    numExpanded = 0;
                }
            }
            hyster[b.getArgAsString(0)].set(isTouched);
            delays[b.getArgAsString(0)].update(hyster[b.getArgAsString(0)]);
        }
        if(b.getAddress() == "/motion"){
            //            if(delays[b.getArgAsString(0)].getActive())
            //                delays[b.getArgAsString(0)].stop();
            //            delays[b.getArgAsString(0)].start();
        }
    }
    
    
    // update the cameras
    for(int i = 0; i < grabbers.size(); i++)
    {
        grabbers[i]->update();
        if(grabbers[i]->isFrameNew()){
            if(rectMap[ipcams[i].getName()]->bRecord){
                recorder[ipcams[i].getName()]->addFrame(grabbers[i]->getPixels());
                recordings[ipcams[i].getName()]->addImage(grabbers[i]->getPixels());
                recordings[ipcams[i].getName()]->dumpImage();
            }
        }
    }
    
    
    
    
    float totalArea = fboWidth*fboHeight;
    int offset = numExpanded+1;
    float expandedHeight = floor(sqrt(totalArea/(offset)));
    float expandedArea = (expandedHeight+padding)*((expandedHeight+padding)/aspectRatio)*numExpanded;
    float height = sqrt((totalArea-expandedArea)/(numRects-numExpanded));
    
    if(expandedHeight<height){
        height = expandedHeight*0.75;
    }
    
    for(int i = 0; i < rectangles.size(); i++){
        if(rectangles[i]->bExpanded){
            ofRectangle fooRect = ofRectangle(0, 0, expandedHeight/aspectRatio, expandedHeight);
            rectangles[i]->targetRect = fooRect;
        }else{
            
            rectangles[i]->targetRect = ofRectangle(0, 0, height/aspectRatio, height);
            
        }
        float offset = rectangles[i]->screenOffset>0?maxw:0;
        //        cout<<offset<<endl;
        rectangles[i]->currentRect.set(targetRects[i]->currentRect.x, targetRects[i]->currentRect.y, ofLerp(rectangles[i]->currentRect.width, rectangles[i]->targetRect.width, rectLerp), ofLerp(rectangles[i]->currentRect.height, rectangles[i]->targetRect.height, rectLerp));
        rectangles[i]->set(rectangles[i]->currentRect);
        
    }
    
    
    ofx::RectangleUtils::sortByHeight(rects);
    
    
    packAll(padding);
    
    
    
    
    for(int i = 0; i < targetRects.size(); i++){
        float offset = rectangles[i]->screenOffset>0?maxw:0;
        
        
        targetRects[i]->currentRect.set(ofLerp(targetRects[i]->currentRect.x, rectangles[i]->currentRect.x+offset, targetLerp), ofLerp(targetRects[i]->currentRect.y, rectangles[i]->currentRect.y, targetLerp), ofLerp(targetRects[i]->currentRect.width, rectangles[i]->targetRect.width, targetLerp), ofLerp(targetRects[i]->currentRect.height, rectangles[i]->targetRect.height, targetLerp));
    }
    
    smoothScale = ofLerp(smoothScale, ofMap(numExpanded, 0, numRects, scaleOutputMin, scaleOutputMax), targetLerp);
    
    for(map<string, ofxImageSequenceRecorder*>::iterator iter = recorder.begin(); iter != recorder.end(); ++iter){
        if(iter->second->isThreadRunning() && !rectMap[iter->first]->bRecord){
            if(iter->second->getCount() < 1){
                iter->second->stopThread();
            }
        }
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
    float time = ofGetElapsedTimef();
    ofBackground(0);
    ofSetColor(255);
    ofEnableAlphaBlending();
    
    fboPing.begin();
    ofSetColor(255, 255, 255, 255);
    fboPong.draw(0, 0);
    fboPing.end();
    
    
    
    fbo.begin();
    ofClear(0, 0, 0, 0);
    ofSetColor(255, 255, 255, 255);
    ofPushMatrix();
    {
        ofTranslate(fbo.getWidth()/2, fbo.getHeight()/2);
        ofScale(smoothScale, smoothScale, smoothScale);
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
                ofTranslate(-padding/2, -padding/2);
                int fooCamera = 0;
                for (int i = 0; i<rectangles.size(); i++){
                    fooCamera = ofToInt(rectangles[i]->uuid)%grabbers.size();
                    ColoredRectangle & rect = *targetRectMap[rectangles[i]->uuid];
                    
                    ofPushMatrix();
                    {
                        ofSetColor(255*(delays[rectangles[i]->uuid].get()), 255*(delays[rectangles[i]->uuid].get()), 255*(delays[rectangles[i]->uuid].get()), 255*(delays[rectangles[i]->uuid].get()));
                        ofFill();
                        ofTranslate(rect.currentRect.x, rect.currentRect.y);
                        //                        if(delays[rect.uuid].get() != 1.0){
                        ofPushMatrix();
                        {
                            ofRotate(90);
                            
                            ofTranslate(rect.currentRect.getHeight()/2, -rect.currentRect.getWidth()/2);
                            
                            if(j==0 || j == 3){
                                grabbers[fooCamera]->draw(rect.currentRect.getHeight()/2,-rect.currentRect.getWidth()/2, -rect.currentRect.getHeight(), rect.currentRect.getWidth());
                            }
                            if(j == 1 || j == 2){
                                
                                grabbers[fooCamera]->draw(-rect.currentRect.getHeight()/2,rect.currentRect.getWidth()/2, rect.currentRect.getHeight(), -rect.currentRect.getWidth());
                                
                            }
                            
                            
                            
                        }
                        ofPopMatrix();
                        //                        }else{
                        ofSetColor(255*(1-delays[rectangles[i]->uuid].get()), 255*(1-delays[rectangles[i]->uuid].get()), 255*(1-delays[rectangles[i]->uuid].get()), 255*(1-delays[rectangles[i]->uuid].get()));
                        ofPushMatrix();
                        {
                            ofRotate(90);
                            
                            ofTranslate(rect.currentRect.getHeight()/2, -rect.currentRect.getWidth()/2);
                            
                            if(j==0 || j == 3){
                                recordings[rectangles[i]->uuid]->getTextureForTime(time+i).draw(rect.currentRect.getHeight()/2,-rect.currentRect.getWidth()/2, -rect.currentRect.getHeight(), rect.currentRect.getWidth());
                            }
                            if(j == 1 || j == 2){
                                
                                recordings[rectangles[i]->uuid]->getTextureForTime(time+i).draw(-rect.currentRect.getHeight()/2,rect.currentRect.getWidth()/2, rect.currentRect.getHeight(), -rect.currentRect.getWidth());
                                
                            }
                            
                            
                            
                        }
                        ofPopMatrix();
                        
                        
                        if(debugView){
                            ofNoFill();
                            if(rectMap[rect.uuid]->bExpanded) ofSetColor(255, 0, 255, 255);
                            else ofSetColor(255, 255, 0, 255);
                            ofDrawRectangle(0,0,rect.currentRect.width, rect.currentRect.height);
                            ofFill();
                            ofSetColor(255, 255, 255);
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
    
    fboPong.begin();
    ofClear(0, 0, 0, 0);
    motionBlur.begin();
    motionBlur.setUniform1f("blur", pBlur);
    motionBlur.setUniformTexture("tex0", fbo.getTexture(), 1);
    motionBlur.setUniformTexture("tex1", fboPing.getTexture(), 2);
    ofSetColor(255, 255, 255, 255);
    fbo.draw(0, 0);
    motionBlur.end();
    fboPong.end();
    
    fboPong.draw(0, 0);
    
    ofDisableAlphaBlending();
    
    server.publishScreen();
    
    //    fbo.draw(0, 0);
    
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
            {
                ofDrawRectangle(rect.currentRect);
                if(rectMap[((ColoredRectangle*)rects[i])->uuid]->bExpanded) ofSetColor(255, 0, 255, 100);
                else ofSetColor(255, 255, 0, 100);
                ofFill();
                ofDrawRectangle(rect.currentRect);
            }
            ofPopMatrix();
        }
        ofPopMatrix();
        ofDisableAlphaBlending();
        ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate(),1), 10, 10);
        
        
        
        debugSeq.getTextureForTime(ofGetElapsedTimef()).draw(ofGetScreenWidth()-debugSeq.getWidth()/10, 0, debugSeq.getWidth()/10, debugSeq.getHeight()/10);
        
        panel.draw();
    }
}

void ofApp::exit(){
    for(int i = 0; i < recorder.size(); i++){
        recorder[rectangles[i]->uuid]->waitForThread();
        recorder[rectangles[i]->uuid]->stopThread();
    }
    panel.saveToFile("Settings.xml");
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
            rectangles[expandScreen]->bRecord = true;
            rectangles[expandScreen]->startTime = ofGetElapsedTimef();
            rectangles[expandScreen]->recordingPath = ofToDataPath("captures/"+ofToString(expandScreen));
            rectangles[expandScreen]->frameNum = 0;
            ofDirectory::createDirectory(rectangles[expandScreen]->recordingPath);
            recordingPaths.push_back(rectangles[expandScreen]->recordingPath);
            recorder[rectangles[expandScreen]->uuid]->setPrefix(rectangles[expandScreen]->recordingPath+"/frame-");
            if(!recorder[rectangles[expandScreen]->uuid]->isThreadRunning())
                recorder[rectangles[expandScreen]->uuid]->startThread();
            
            numExpanded++;
        }else{
            rectangles[expandScreen]->bExpanded = false;
            rectangles[expandScreen]->bRecord = false;
            rectangles[expandScreen]->lastActive = ofGetElapsedTimef();
            numExpanded--;
            if(numExpanded < 0){
                numExpanded = 0;
            }
        }
    }else if(key == ' '){
        for(int i = 0; i < targetRects.size(); i++){
            rectangles[i]->bExpanded = false;
            rectangles[i]->bRecord = false;
            rectangles[i]->lastActive = ofGetElapsedTimef();
            
        }
        numExpanded = 0;
    }
    
    if(key == 'd'){
        debugView = !debugView;
    }
    
    if(key == 'f'){
        ofToggleFullscreen();
    }
}

