#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class DisplayManager {
private:
	Adafruit_SSD1306 display;
	unsigned long lastFrameTime = 0;
	int currentFrame = 0;

	// Static const array
	static const unsigned char heart_frame0 [1024] PROGMEM;
	static const unsigned char heart_frame1 [1024] PROGMEM;
	static const unsigned char heart_frame2 [1024] PROGMEM;
	static const unsigned char heart_frame3 [1024] PROGMEM;
	static const unsigned char heart_frame4 [1024] PROGMEM;

	static const unsigned char* heart_frameallArray[5];

	void drawTextOverlay(int bpm, int spo2, float rmssd, float sdrr, int pnn50, String statusText);

public:
	DisplayManager() : display(128, 64, &Wire, -1) {}

	void init();
	void showMessage(String message);
	void showNoFinger();
	void showAnimationIntro();
	void updateDisplay(int bpm, int spo2, float rmssd, float sdrr, int pnn50, String status, bool fingerDetected, bool frozen);
};

#endif