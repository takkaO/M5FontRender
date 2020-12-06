#include "M5FontRender.h"

bool M5FontRender::loadFont(const font_data_t *data, font_data_size_t size) {
    // load from array
    if (font_face_init(&font_face, data, size) != ESP_OK) {
        return false;
    }

    int font_size = 44;
    if (font_render_init(&font_render, &font_face, font_size, font_cache_size) != ESP_OK) {
        return false;
    }

    return true;
}

bool M5FontRender::loadFont(const char *path) {
    // load from SD
    if (font_face_init_fs(&font_face, path) != ESP_OK) {
        return false;
    }
    return true;
}

bool M5FontRender::setTextSize(uint16_t font_size) {
    font_render_destroy(&font_render);
    if(font_render_init(&font_render, &font_face, font_size, font_cache_size) != ESP_OK) {
        return false;
    }
    return true;
}

void M5FontRender::setTextColor(uint16_t font_color) {
    _font_color = font_color;
}

void M5FontRender::setCursor(uint32_t posX, uint32_t posY) {
    _posX = posX;
    _posY = posY;
}

void M5FontRender::drawString(const char *string, int32_t poX, int32_t poY, uint16_t fg) {
    drawString(string, poX, poY, fg, &font_render);
}

void M5FontRender::drawString(const char *string, int32_t poX, int32_t poY) {
    drawString(string, poX, poY, _font_color, &font_render);
}

void M5FontRender::printf(const char* fmt, ...) {
    uint16_t len;
    char str[256] = {0};
    va_list ap;

    va_start(ap, fmt);
    len = vsnprintf(str, 256, fmt, ap);
    va_end(ap);

    drawString(str, _posX, _posY);
}

/************************
 * Protected function
 ***********************/
void M5FontRender::drawFreetypeBitmap(int32_t cx, int32_t cy, uint16_t bw, uint16_t bh, uint16_t fg, uint8_t *bitmap)
{
    const uint8_t alphamap[16] = {0, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};
    M5.Lcd.startWrite();
    uint32_t pos = 0;
    uint16_t bg = 0;
    for (int y = 0; y < bh; y++) {
        for (int x = 0; x < bw; x++) {
            if (pos & 0x1) {
                M5.Lcd.drawPixel(cx + x, cy + y, M5.Lcd.alphaBlend(alphamap[bitmap[pos >> 1] & 0x0F], fg, bg));
            }
            else {
                M5.Lcd.drawPixel(cx + x, cy + y, M5.Lcd.alphaBlend(alphamap[bitmap[pos >> 1] >> 4], fg, bg));
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

void M5FontRender::drawString(const char *string, int32_t poX, int32_t poY, uint16_t fg, font_render_t *render)
{
    int16_t sumX = 0;
    uint16_t len = strlen(string);
    uint16_t n = 0;
    uint16_t base_y = poY + font_render.max_pixel_height;

    while (n < len) {
        uint16_t uniCode = decodeUTF8((uint8_t *)string, &n, len - n);
        if (font_render_glyph(render, uniCode) != ESP_OK) {
            ESP_LOGE(TAG, "Font render faild.");
        }
        drawFreetypeBitmap(poX + render->bitmap_left, base_y - render->bitmap_top, render->bitmap_width, render->bitmap_height, fg, render->bitmap);
        poX += render->advance;
        sumX += render->advance;
    }
}