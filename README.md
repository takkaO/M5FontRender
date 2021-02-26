# M5 Font Render

TTF font render support for M5Stack / M5Core2 with Arduino IDE.  
This library can render TTF font files in the SD card or TTF font files embedded in the program.

![image](https://github.com/takkaO/M5FontRender/blob/images/sample.jpg?raw=true)


## How to use
1. Clone this repository into Arduino library directory.
2. Include ```M5FontRender.h``` **AFTER include MtStack.h / M5Core2.h**.

## Sample code
#### Load TTF from binary TTF file
```c++
#include "M5Core2.h"
#include "binaryttf.h" // This is font file
#include "M5FontRender.h" // Include after M5Stack.h / M5Core2.h

M5FontRender render;

void setup() {
	// put your setup code here, to run once:
	M5.begin();
	M5.Lcd.fillScreen(BLACK);
	
	// Load TTF from C header file
	if (!render.loadFont(binaryttf, sizeof(binaryttf))) {
		Serial.println("Render initialize error");
		return;
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
```

#### Load TTF from SD card
```c++
#include "M5Core2.h"
#include "M5FontRender.h" // Include after M5Stack.h / M5Core2.h

M5FontRender render;

void setup() {
	// put your setup code here, to run once:
	M5.begin();
	M5.Lcd.fillScreen(BLACK);
	
	// Load TTF from microSD card
	if (!render.loadFont("/JKG-M_3.ttf")) {
		Serial.println("Render initialize error");
		return;
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
```

## How to create binary TTF file
We use [binary2ttf.py](https://github.com/takkaO/M5FontRender/tree/master/tools/ttf2bin) in tools directory to create binary TTF font file.  
The ```binary2ttf.py``` is provided in the [M5EPD](https://github.com/m5stack/M5EPD/tree/main/tools/ttf2bin) library.
The same program is included in ```tools``` directory in this repo.  
You only execute below command.

```sh
python3 binary2ttf.py your_font_file.ttf
```

## Reduce TTF font size
By deleting unnecessary characters in a TTF font file, you can reduce the size of the TTF font file.  
I used [「サブセットフォントメーカー」](https://opentype.jp/subsetfontmk.htm) to reduce TTF font size. (I'm sorry. I was not able to find English lang software.)

## Note
We have used below font file for the sample program.  
We would like to thank sozai-font-hako for providing us with an easy-to-use license for these wonderful fonts.

| Font | Copyright | 
| --- | --- | 
|[JK Gothic M](http://font.cutegirl.jp/jk-font-medium.html#i)|Copyright  (c) 2014 M+ FONTS PROJECT <br> Copyright  (c) 2015 JK FONTS|

## Reference
- [m5stack/M5EPD - github](https://github.com/m5stack/M5EPD)
- [h1romas4/m5stack-core2-template at freetype - github](https://github.com/h1romas4/m5stack-core2-template/tree/freetype)
