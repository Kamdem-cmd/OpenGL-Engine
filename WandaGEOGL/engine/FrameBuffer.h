#pragma once

#include <iostream>
#include <cassert>
#include <cstdint>

#include "NKWindow/NkWindow.h"
#include "NKPlatform/NkPlatformDetect.h"
#include "../nkMath/NkColor.h"
#include "NKGlad/include/glad/wgl.h"
#include "NKGlad/include/glad/gl.h"
// #include "Image.h"

#if defined(NKENTSEU_PLATFORM_WINDOWS)
    #include <windows.h>
#endif

using namespace nkentseu;

class FrameBuffer
{
 private:
NkVector<uint8> pixels;
math::NkVec2u size;
uint32 stride = 0;

#if defined(NKENTSEU_PLATFORM_WINDOWS)
    HWND hwnd      = nullptr;  // HWND
    HDC hdc       = nullptr;  // HDC
    HBITMAP dibBitmap = nullptr;  // HBITMAP DIBSection
    HDC dibDC     = nullptr;  // HDC mémoire
    void* dibBits   = nullptr;  // Pointeur pixels DIB
#endif

bool InitBackend(NkWindow& window) {
    #if defined(NKENTSEU_PLATFORM_WINDOWS)
        this->hwnd = window.GetSurfaceDesc().hwnd;
        // this->hwnd  = static_cast<HWND>(window.GetSurfaceDesc().hwnd);
        this->hdc  = GetDC(this->hwnd);

        BITMAPINFO bmi = {};
        bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth       = (LONG)size.width;
        bmi.bmiHeader.biHeight      = -(LONG)size.height; // top-down
        bmi.bmiHeader.biPlanes      = 1;
        bmi.bmiHeader.biBitCount    = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        this->dibBitmap = CreateDIBSection(static_cast<HDC>(this->hdc), &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (!this->dibBitmap) { 
            logger.Error("CreateDIBSection failed"); 
            return false; 
        }

        this->dibBits = bits;
        this->dibDC   = CreateCompatibleDC(static_cast<HDC>(this->hdc));
        SelectObject(static_cast<HDC>(this->dibDC), static_cast<HBITMAP>(this->dibBitmap));
        return true;
    #else
        return false;
    #endif
}

void CloseBackend() {
    #if defined(NKENTSEU_PLATFORM_WINDOWS)
        if (this->dibDC) { 
            DeleteDC(static_cast<HDC>(this->dibDC));
            this->dibDC = nullptr;
        }

        if (this->dibBitmap) { 
            DeleteObject(static_cast<HBITMAP>(this->dibBitmap)); 
            this->dibBitmap = nullptr;
        }

        if (this->hdc && this->hwnd) {
            ReleaseDC(static_cast<HWND>(this->hwnd), static_cast<HDC>(this->hdc));
            this->hdc = nullptr;
        }
    #endif
}

public:
FrameBuffer(NkWindow& window) : size(window.GetSize()) {
    if (!IsValide()) return;
    stride = size.width * 4u;

    pixels.Reserve(size.width * size.height * 4);

    InitBackend(window);
}

~FrameBuffer() {
    CloseBackend();
}

bool Resize(uint32 w, uint32 h, NkWindow& window) {
    if (w == 0 || h == 0) return true;
    
    CloseBackend();

    stride = w * 4u;
    size.width = w;
    size.height = h;

    const usize pixelCount = static_cast<usize>(stride) * static_cast<usize>(h);
    pixels.Assign(static_cast<uint8>(0), static_cast<NkVector<uint8>::SizeType>(pixelCount));

    if (!InitBackend(window)) {
        return false;
    }
    return true;
}

bool IsValide() {
    if (size.width <= 0 || size.height <= 0) return false;
    return true;
}

uint8*       RowPtr(uint32 y)       { return pixels.Data() + y * stride; }
const uint8* RowPtr(uint32 y) const { return pixels.Data() + y * stride; }

void SetPixel(int x, int y, const NkColor& color) {
    if (x < 0 || x >= size.width || y < 0 || y >= size.height) return;

    uint8* p = RowPtr(y) + x * 4u;

    p[0] = color.mR;
    p[1] = color.mG;
    p[2] = color.mB;
    p[3] = color.mA;
}

void SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
    if(x >= size.width || y >= size.height) return;
    int indice = (y + x * size.height) * 4u;
    pixels[indice + 0] = r;    // red
    pixels[indice + 1] = g;    // green
    pixels[indice + 2] = b;    // blue
    pixels[indice + 3] = a;    // alpha
}

math::NkColor GetPixel(int x, int y) {
    if (x < 0 || x >= size.width || y < 0 || y >= size.height) return math::NkColor::Transparent();

    uint8* p = RowPtr(y) + x * 4u;
    return math::NkColor(p[0], p[1], p[2], p[3]);
}

void Clear(const NkColor& color) {
    for (usize i = 0; i < pixels.Size(); i += 4) {
        pixels[i + 0] = color.mR;
        pixels[i + 1] = color.mG;
        pixels[i + 2] = color.mB;
        pixels[i + 3] = color.mA;
    }
}

void Clear(uint32_t clearColor){
    for (size_t i = 0; i < size.height * size.width ; ++i)
    {
        int j = i * 4u;
        pixels[j + 0] = clearColor >> 16 & 0xFF;
        pixels[j + 1] = clearColor >> 8  & 0xFF;
        pixels[j + 2] = clearColor >> 0  & 0xFF;
        pixels[j + 3] = clearColor >> 24 & 0xFF;
    }   
}

void Clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
    for (size_t i = 0; i < size.height * size.width ; ++i)
    {
        int j = i * 4u;
        pixels[j + 0] = r;
        pixels[j + 1] = g;
        pixels[j + 2] = b;
        pixels[j + 3] = a;
    }   
}

void Clear(uint8_t r, uint8_t g, uint8_t b){
    for (size_t i = 0; i < size.height * size.width ; ++i)
    {
        int j = i * 4u;
        pixels[j + 0] = r;
        pixels[j + 1] = g;
        pixels[j + 2] = b;
        pixels[j + 3] = 255.0f;
    }   
}

void Present() {
    #if defined(NKENTSEU_PLATFORM_WINDOWS)
        if (this->dibBits && this->dibDC && this->hdc) {
            memcpy(this->dibBits, pixels.Data(), pixels.Size());
            BitBlt(static_cast<HDC>(this->hdc), 0, 0, (int)size.width, (int)size.height, static_cast<HDC>(this->dibDC), 0, 0, SRCCOPY);
        }
    #endif
}

    GLuint fbo = 0, colorTex = 0, depthRbo = 0; 
    int width, height; 
 
    void create(int w, int h, bool hdr = true) { 
        width = w; height = h; 
        glGenFramebuffers(1, &fbo); 
        glBindFramebuffer(GL_FRAMEBUFFER, fbo); 
 
        // Texture couleur (HDR si float16) 
        glGenTextures(1, &colorTex); 
        glBindTexture(GL_TEXTURE_2D, colorTex); 
        GLenum fmt = hdr ? GL_RGBA16F : GL_RGBA8; 
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, 
                     GL_RGBA, GL_FLOAT, nullptr); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
        glFramebufferTexture2D(GL_FRAMEBUFFER, 
            GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0); 
 
        // Depth + Stencil via Renderbuffer 
        glGenRenderbuffers(1, &depthRbo); 
        glBindRenderbuffer(GL_RENDERBUFFER, depthRbo); 
        glRenderbufferStorage(GL_RENDERBUFFER, 
            GL_DEPTH24_STENCIL8, w, h); 
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, 
            GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRbo); 
 
        
        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE); 
        glBindFramebuffer(GL_FRAMEBUFFER, 0); 
    } 
 
    void bind()   { glBindFramebuffer(GL_FRAMEBUFFER, fbo); } 
    void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0);   } 
};

