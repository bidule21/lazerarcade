/*
 *  CameraManager.cpp
 *  PixelPhones
 *
 *  Created by Seb Lee-Delisle on 15/03/2012.
 *  Copyright 2012 seb.ly. All rights reserved.
 *
 */

#include "CameraManager.h"
#include <vector>

CameraManager::CameraManager() {
	cameraFirewire		= new CameraFirewire(); 
	cameraVidPlayer		= new CameraVidPlayer(); 
    cameraVidGrabber	= new CameraVidGrabber();
	//cameraCanon			= new CameraCanon();
    
   // captureInterval = 1.0f / CAPTURE_FPS;
    capturing = false;
    captureBase = "capturedFrames/";
}

void CameraManager::init() { 
	
    initParams();
    
	if(cameraFirewire->setup("Firewire", 1024, 768, 60)) {
		cameras.push_back(cameraFirewire);
		cameraFirewire->flipX = true;
		cameraFirewire->flipY = true;
		
	}; 
	
	if(cameraVidGrabber->setup("USB", 640, 480, 60)){
		cameras.push_back(cameraVidGrabber);
	};
//	if(cameraCanon->setup("Canon", 1024, 768, 60)){
//		cameras.push_back(cameraCanon);
//	};
	
//	if(cameraVidPlayer->setup("Video", "../../../TestMovies/TestPyrosCamCropped.mov", 1024, 768, 60)) {
//		cameras.push_back(cameraVidPlayer); 
//	}; 
	//cameraFirewire->setGain(700);
	if(cameras.size()==0) { 
		ofLog(OF_LOG_ERROR, "No cameras initialised!");
		//camera = new CameraWrapper(); 
	} else { 
		camera = cameras[0]; 
	}
	
	
	
	
//	cameraPreview->vid = ofBaseDraws(camera->getPixelsRef());
		
	
	
}

void CameraManager::initParams(){
	
    parameters.setName("Camera Manager");
	parameters.add(gainParam.set("gain", 500,200,1024));
	parameters.add(gammaParam.set("gamma", 15, 10,22));
    parameters.add(shutterParam.set("shutter speed", 30,1,500));
    parameters.add(brightnessParam.set("brightness", 0,0,255));

}


bool CameraManager::update() { 
	if(!camera) {
		ofLog(OF_LOG_ERROR, "CameraManager not initialised"); 
		return false; 
	}; 
	
//	cout << toggleShowUSBControls->value.getValueB() << endl; 
//
    /*
	if(toggleShowUSBControls->value.getValueB()){
		
		cameraVidGrabber->videoSettings(); 
		toggleShowUSBControls->value.setValue(0); 
		toggleShowUSBControls->update();
	}*/
	
	
	/*
	if(camera->getGain()!=gainParam) camera->setGain(gainParam);
	if(camera->getGamma()!=gammaParam) camera->setGamma(gammaParam);
	if(camera->getShutter()!=shutterParam) camera->setShutter(shutterParam);
	if(camera->getBrightness()!=brightnessParam) camera->setBrightness(brightnessParam);
	*/
	bool updateCamera = camera->update();
    if( updateCamera && capturing ) {
        captureFrame();
    }
	/*
	for(int i = 0; i<cameras.size(); i++) {
		
		CameraIP * cam = dynamic_cast<CameraIP*>(cameras[i]);
		if((cam) && (cam!=camera)) {
			cam->update();
			
		}
	}*/
	

    return updateCamera;
}

void CameraManager::draw(float x, float y) { 
	camera->draw(x, y);
	
}

CameraVidPlayer * CameraManager:: addVidPlayer(string name, string filename, int width, int height){
	
	CameraVidPlayer * vidPlayer = new CameraVidPlayer();
	
	// should probably add a unique id that relates to the file name
	
	if(vidPlayer->setup(name, filename, width, height, 60)) {
		cameras.push_back(vidPlayer);
		// last initialised becomes the cameraVidPlayer property 
		cameraVidPlayer = vidPlayer;
	} else {
		delete vidPlayer; 
		
	}
	
	
}

CameraIP * CameraManager:: addIPPlayer(string name, string url, string user, string password, int width, int height){
	
	CameraIP * ipPlayer = new CameraIP(); 

	// should probably add a unique id that relates to the file name
	
	if(ipPlayer->setup(name, width, height, 30, url, user, password)) {
		cameras.push_back(ipPlayer);
		// last initialised becomes the cameraVidPlayer property
		//cameraVidPlayer = ipPlayer;
		return ipPlayer; 
	} else {
		delete ipPlayer;
		return NULL;
	}
	
	
	
}





void CameraManager::captureFrame() {
   // if( capturing && ((ofGetElapsedTimef() - captureDelta) > captureInterval) ) {
        ofSaveImage(camera->getPixelsRef(), getCaptureFilename());
        
   //     captureDelta = ofGetElapsedTimef();
        framesCaptured++;
   // }
}

void CameraManager::draw(float x, float y, float w, float h){
	camera->draw(x,y,w,h);
}

string CameraManager::getCaptureFilename() {
    return captureBase + "/pixelpyros_" + captureTimestamp + "_" + ofToString(framesCaptured) + ".tiff";
}

void CameraManager::beginCapture() {
    capturing = true;
    framesCaptured = 0;
    captureDelta = ofGetElapsedTimef();
    captureTimestamp = ofGetTimestampString("%Y%m%d-%H%M%S");
    
    cout << "Capturing to " << getCaptureFilename() << " at " << CAPTURE_FPS << " FPS" << endl;
}

void CameraManager::endCapture() {
    if( capturing ) {
        capturing = false;
        
        cout << "End Capture: " << framesCaptured << " frames captured" << endl;
    }
}

ofPixelsRef CameraManager::getPixelsRef() {
	return camera->getPixelsRef();

}

string CameraManager:: getCameraLabel() {
	return camera->name;
	
}

float CameraManager::getWidth() { 
	return camera->getWidth(); 
}

float CameraManager::getHeight() { 
	return camera->getHeight(); 
}

void CameraManager::videoSettings() {
	
	camera->videoSettings(); 
	
}

void CameraManager::next() { 
	for(int i = 0; i<cameras.size(); i++) { 
		CameraWrapper * currentCamera = cameras[i]; 
		if(currentCamera == camera) {
			if(i==cameras.size()-1) {
				changeCamera(cameras[0]);
			} else { 
				changeCamera(cameras[i+1]); 
			}
			break; 
		}
	}
}

bool CameraManager:: changeCamera(string camname) { 
	for(int i = 0; i<cameras.size(); i++) { 
		CameraWrapper * cam = cameras[i]; 
		if(cam->name == camname) { 
			changeCamera(cam); 
			return true; 
		}
	}
	return false; 
	
}

bool CameraManager :: changeCamera(CameraWrapper* cam) { 
	camera = cam;
    capturing = false;
	//cout << "changed camera to " << camera->name << endl;
	return true;
}

/*
void CameraManager:: initControlPanel(ofxAutoControlPanel &gui){
	
	
	cameraPreview = gui.addDrawableRect("video", this, 400, 300);
	
	gui.addLabel("USB camera");
	toggleShowUSBControls = gui.addToggle("Show controls", "USB_SHOW_CONTROLS", false);

	
	cameraFirewire->initControlPanel(gui, 400); 
	
	
	ofAddListener(gui.guiEvent, this, &CameraManager::guiEventsIn);
	
}
 */

/*
void CameraManager::guiEventsIn(guiCallbackData & data){

	cout << "CameraManager::guiEventsIn " << data.getXmlName() <<" "<< data.getInt(0) << endl;
	
	if (data.getXmlName() == "USB_SHOW_CONTROLS") {
		
	
	} else if (data.getXmlName() == "FW_SHUTTER") { 
		
		cameraFirewire->setShutter(data.getInt(0));
		
	} else if (data.getXmlName() == "FW_GAIN") { 
		cameraFirewire->setGain(data.getInt(0));
		
	} else if (data.getXmlName() == "FW_BRIGHTNESS") { 
		cameraFirewire->setBrightness(data.getInt(0));
		
	} else if (data.getXmlName() == "FW_GAMMA") { 
		cameraFirewire->setGamma(data.getInt(0));
		
	} 
	
}
*/

void CameraManager::close() { 
	for(int i = 0; i<cameras.size(); i++) {
		cameras[i]->close();
		delete cameras[i]; 
	}
}


