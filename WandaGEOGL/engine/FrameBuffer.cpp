#include "FrameBuffer.h"

// FrameBuffer::FrameBuffer(int mHeight, int mWidth, int mChanel)
// {
//     this->mHeight = mHeight;
//     this->mWidth = mWidth;
//     this->mChanel = mChanel;

//     mPixels = new uint8_t[mHeight * mWidth * mChanel];
// }

// FrameBuffer::~FrameBuffer(){
//     delete mPixels;
//     CloseBackend();
// }

// void FrameBuffer::SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a){
//     if(x >= mWidth || y >= mHeight) return;
//     int indice = (y + x * mHeight) * mChanel;
//     mPixels[indice + 0] = r;    // red
//     mPixels[indice + 1] = g;    // green
//     mPixels[indice + 2] = b;    // blue
//     mPixels[indice + 3] = a;    // alpha
// }

// void FrameBuffer::SetPixel(int x, int y, Color color){
//     if(x >= mWidth || y >= mHeight) return;
//     int indice = (y + x * mHeight) * mChanel;
//     mPixels[indice + 0] = color.r;    // red
//     mPixels[indice + 1] = color.g;    // green
//     mPixels[indice + 2] = color.b;    // blue
//     mPixels[indice + 3] = color.a;    // alpha
// }

// uint32_t FrameBuffer::GetPixel(int x, int y){
//     uint32_t pixel;
//     if(x >= mWidth || y >= mHeight) return pixel;
//     int i = (y + x * mHeight) * mChanel;
//     pixel = ((uint32_t)mPixels[i ]   << 24)|      // red
//                     ((uint32_t)mPixels[i + 1] << 16)|      // green
//                     ((uint32_t)mPixels[i + 2] << 8 )|      // blue
//                     ((uint32_t)mPixels[i + 3]);            // alpha

//     return pixel;
// }

// void FrameBuffer::Clear(Color clearColor){
//     for (size_t i = 0; i < mHeight * mWidth ; ++i)
//     {
//         int j = i * mChanel;
//         mPixels[j + 0] = clearColor.r;
//         mPixels[j + 1] = clearColor.g;
//         mPixels[j + 2] = clearColor.b;
//         mPixels[j + 3] = clearColor.a;
//     }   
// }

// int FrameBuffer::ComputePitch() const {
//     return mWidth * mChanel; // RGBA = 4 octets/pixel
// }