#if defined( ARDUINO_M5Stack_Core_ESP32 ) || defined( ARDUINO_M5STACK_FIRE )
	#include "M5Stack.h"
#elif defined( ARDUINO_M5STACK_Core2 ) // M5Stack Core2
	#include "M5Core2.h"
#endif
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

	render.setTextColor(WHITE);
	render.printf("Hello World\n");
	render.seekCursor(0, 10);
	render.setTextSize(30);
	render.setTextColor(GREEN);
	render.printf("完全なUnicodeサポート\n");
	render.seekCursor(0, 10);
	render.setTextSize(40);
	render.setTextColor(ORANGE);
	render.printf("こんにちは世界\n");
}

void loop() {
	// put your main code here, to run repeatedly:

}
