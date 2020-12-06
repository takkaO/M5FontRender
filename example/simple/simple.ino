#include "M5Core2.h"
#include "binaryttf.h"
#include "M5FontRender.h" // Include after M5Stack.h / M5Core2.h

M5FontRender render;

void setup() {
	M5.begin();
	M5.Lcd.fillScreen(BLACK);
	// put your setup code here, to run once:
	if (!render.loadFont(binaryttf, sizeof(binaryttf))) {
		Serial.println("Load Error");
	}

	render.setTextColor(0x7b3d);
	render.printf("%s", "フォントJK");
}

void loop() {
	// put your main code here, to run repeatedly:

}
