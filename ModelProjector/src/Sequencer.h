#pragma once

#include "ofMain.h"
#include "ofxMidi.h"

class Sequencer : ofThread {
protected:	
	unsigned long startTime;
	unsigned int port;
	unsigned int channel;
	unsigned int rows, cols;
	float measureLength, colLength;
	vector<bool> notesOn;
	vector<vector<bool> > threadedBuffer, middleBuffer, updateBuffer;
	vector<unsigned char > threadedNotes, middleNotes, updateNotes;
	
	void trigger(unsigned int col) {
		for(int i = 0; i < rows; i++) {
			if(notesOn[i]) {
				midiOut.sendNoteOff(channel, threadedNotes[i], 0);
				notesOn[i] = false;
			}
			if(threadedBuffer[i][col]) {
				midiOut.sendNoteOn(channel, threadedNotes[i], 100);
				notesOn[i] = true;
			}
		}
	}
	void threadedFunction() {
		midiOut.listPorts();
		midiOut.openPort(port);
		unsigned int lastCol = 0;
		while(isThreadRunning()) {
			unsigned int curCol = (unsigned int) getPosition();
			if(curCol != lastCol) {
				trigger(curCol);
			}
			lastCol = curCol;
			
			lock();
			threadedBuffer = middleBuffer;
			threadedNotes = middleNotes;
			unlock();
		
			ofSleepMillis(1);
		}
	}
public:
	ofxMidiOut midiOut;
	
	void setup(vector<unsigned char>& notes, unsigned int cols, float bpm = 120, unsigned int port = 0, unsigned int channel = 1) {				
		middleNotes = notes;
		updateNotes = notes;
		threadedNotes = notes;
	
		this->rows = updateNotes.size();
		this->cols = cols;
		this->channel = channel;
		
		middleBuffer.resize(rows, vector<bool>(cols, false));
		updateBuffer = middleBuffer;
		threadedBuffer = middleBuffer;
		
		notesOn.resize(rows, false);
		setBpm(bpm);
		startTime = ofGetSystemTime();
		startThread(true, false);
		
		ofAddListener(ofEvents.update, this, &Sequencer::update);
	}
	void update() {
		lock();
		middleBuffer = updateBuffer;
		middleNotes = updateNotes;
		unlock();
	}
	void update(ofEventArgs& event) {
		update();
	}
	void setBpm(float bpm) {
		measureLength = 1000 * 60 * 4 / bpm;
		colLength = measureLength / cols;
	}
	void setState(int row, int col, bool state) {
		if(row >= 0 && row < rows && col >= 0 && col < cols) {
			updateBuffer[row][col] = state;
		}
	}
	bool getState(int row, int col) const {
		if(row >= 0 && row < rows && col >= 0 && col < cols) {
			return updateBuffer[row][col];
		} else {
			return false;
		}
	}
	bool getMomentaryState(int row, int col) const {
		unsigned int curCol = (unsigned int) getPosition();
		return col == curCol && getState(row, col);
	}
	float getPosition() const {
		unsigned long curTime = ofGetSystemTime();
		unsigned long timeDiff = curTime - startTime;
		return fmod(timeDiff, measureLength) * (cols / measureLength);
	}
	float getStepsPerFrame(float fps) {
		float timeDiff = 1000. / fps;
		return timeDiff * (cols / measureLength);
	}
	unsigned int getRows() const {
		return rows;
	}
	unsigned int getCols() const {
		return cols;
	}
	void setNotes(vector<unsigned char>& notes) {
		updateNotes = notes;
	}
};