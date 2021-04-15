#include "M5FontRender.h"

M5FontRender::M5FontRender() {
    #if defined (ARDUINO_M5Stack_Core_ESP32)
        _font_cache_size = 32;
    #elif defined (ARDUINO_M5STACK_FIRE) || defined(ARDUINO_M5STACK_Core2)
        _font_cache_size = 256;
    #else
        _font_cache_size = 64;
    #endif
    _font_size = 44;

    // Somehow height and width are reversed 
    _width = M5.Lcd.height();
    _height = M5.Lcd.width();
}

M5FontRender::~M5FontRender() {
    font_render_destroy(&_font_render);
    font_face_destroy(&_font_face);
}

void M5FontRender::unloadFont() {
    font_render_destroy(&_font_render);
    font_face_destroy(&_font_face);
}

bool M5FontRender::loadFont(const font_data_t *data, font_data_size_t size) {
    // load from array
    if (font_face_init(&_font_face, data, size) != ESP_OK) {
        return false;
    }

    if (font_render_init(&_font_render, &_font_face, _font_size, _font_cache_size) != ESP_OK) {
        return false;
    }

    return true;
}

bool M5FontRender::loadFont(const char *path) {
    // load from SD
    #ifdef ESP_PLATFORM
    ffsupport_setffs(SD);
    #endif
    if (font_face_init_fs(&_font_face, path) != ESP_OK) {
        return false;
    }

    if (font_render_init(&_font_render, &_font_face, _font_size, _font_cache_size) != ESP_OK) {
        return false;
    }
    
    return true;
}

bool M5FontRender::setFontCacheSize(unsigned int font_cache_size) {
    _font_cache_size = font_cache_size;
    return initializeRender();
}

bool M5FontRender::setTextSize(uint16_t font_size) {
    _font_size = font_size;
    return initializeRender();
}

void M5FontRender::setTextColor(uint16_t color_fg) {
    _font_color = color_fg;
}

void M5FontRender::setTextColor(uint16_t color_fg, uint16_t color_bg) {
    _font_color = color_fg;
    _font_background_color = color_bg;
}

void M5FontRender::setTextBackgroundColor(uint16_t color_bg) {
    _font_background_color = color_bg;
}

void M5FontRender::setCursor(uint32_t posX, uint32_t posY) {
    _posX = posX;
    _posY = posY;
}

uint32_t M5FontRender::getCursorX(){
    return _posX;
}

uint32_t M5FontRender::getCursorY(){
    return _posY;
}

void M5FontRender::seekCursor(int deltaX, int deltaY) {
    _posX += deltaX;
    _posY += deltaY;
}

void M5FontRender::enableAutoNewline(bool enable) {
    _auto_newline = enable;
}

void M5FontRender::drawString(const char *string, int32_t poX, int32_t poY, uint16_t fg, uint16_t bg) {
    drawString(string, poX, poY, fg, bg, &_font_render);
}

void M5FontRender::drawString(const char *string, int32_t poX, int32_t poY, uint16_t fg) {
    drawString(string, poX, poY, fg, _font_background_color, &_font_render);
}

void M5FontRender::drawString(const char *string, int32_t poX, int32_t poY) {
    drawString(string, poX, poY, _font_color, _font_background_color, &_font_render);
}

void M5FontRender::printf(const char* fmt, ...) {
    char str[256] = {0};
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(str, 256, fmt, ap);
    va_end(ap);

    drawString(str, _posX, _posY);
}

/************************
 * Protected function
 ***********************/
bool M5FontRender::initializeRender() {
    font_render_destroy(&_font_render);
    if(font_render_init(&_font_render, &_font_face, _font_size, _font_cache_size) != ESP_OK) {
        return false;
    }
    return true;    
}

void M5FontRender::drawFreetypeBitmap(int32_t cx, int32_t cy, uint16_t bw, uint16_t bh, uint16_t fg, uint16_t bg, uint8_t *bitmap)
{
    const uint8_t alphamap[16] = {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};
    M5.Lcd.startWrite();
    uint32_t pos = 0;
    for (int y = 0; y < bh; y++) {
        for (int x = 0; x < bw; x++) {
            uint32_t tx = (cx + x) * cos(0.5) - (cy + y) * sin(0.5);
            uint32_t ty = (cx + x) * sin(0.5) + (cy + y) * cos(0.5);
            if (pos & 0x1) {
                M5.Lcd.drawPixel(tx, ty, M5.Lcd.alphaBlend(alphamap[bitmap[pos >> 1] & 0x0F], fg, bg));
                //M5.Lcd.drawPixel(cx + x, cy + y, M5.Lcd.alphaBlend(alphamap[bitmap[pos >> 1] & 0x0F], fg, bg));
            }
            else {
                M5.Lcd.drawPixel(tx, ty, M5.Lcd.alphaBlend(alphamap[bitmap[pos >> 1] >> 4], fg, bg));
                //M5.Lcd.drawPixel(cx + x, cy + y, M5.Lcd.alphaBlend(alphamap[bitmap[pos >> 1] >> 4], fg, bg));
            }
            pos++;
        }
    }
    M5.Lcd.endWrite();
}

uint16_t M5FontRender::decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining)
{
    uint16_t c = buf[(*index)++];
    //
    // 7 bit Unicode
    if ((c & 0x80) == 0x00)
        return c;

    // 11 bit Unicode
    if (((c & 0xE0) == 0xC0) && (remaining > 1))
        return ((c & 0x1F) << 6) | (buf[(*index)++] & 0x3F);

    // 16 bit Unicode
    if (((c & 0xF0) == 0xE0) && (remaining > 2)) {
        c = ((c & 0x0F) << 12) | ((buf[(*index)++] & 0x3F) << 6);
        return c | ((buf[(*index)++] & 0x3F));
    }

    // 21 bit Unicode not supported so fall-back to extended ASCII
    // if ((c & 0xF8) == 0xF0) return c;

    return c; // fall-back to extended ASCII
}

void M5FontRender::drawString(const char *string, int32_t poX, int32_t poY, uint16_t fg, uint16_t bg, font_render_t *render)
{
    int16_t sumX = 0;
    uint16_t len = strlen(string);
    uint16_t n = 0;
    uint16_t base_y = poY + render->max_pixel_height;

    while (n < len) {
        uint16_t uniCode = decodeUTF8((uint8_t *)string, &n, len - n);
        
        if (uniCode == 	0x000A || uniCode == 0x000D) {
            // LF or CR
            poX = 0;
            if (uniCode == 0x000A) {
                base_y += render->max_pixel_height;
            }
            continue;
        }

        if (font_render_glyph(render, uniCode) != ESP_OK) {
            ESP_LOGE(TAG, "Font render faild.");
        }
        if (_auto_newline == true && poX + (render->bitmap_width) > _width) {
            poX = 0;
            base_y += render->max_pixel_height;
        }
        drawFreetypeBitmap(poX + render->bitmap_left, base_y - render->bitmap_top, render->bitmap_width, render->bitmap_height, fg, bg, render->bitmap);
        poX += render->advance;
        sumX += render->advance;
    }
    _posX = poX;
    _posY = base_y - render->max_pixel_height;
}