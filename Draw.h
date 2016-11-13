#ifndef DRAW_H
#define DRAW_H


#include <cstdio>
#include <cstdlib>
#include "ShaderCode.h"

// #include <jni.h>
// #include <android/log.h>
// #include "GLES3/gl3.h"
// #include "EGL/egl.h"
// #include "EGL/eglext.h"


#include <string>
#include <cmath>

//**********************
#include <GL/glew.h>
#include <iostream>
//**********************
// #define LOG_TAG "libNative"
// #define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
// #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define LOGI(...) printf("%s\n",__VA_ARGS__)
#define LOGE(...) printf("%s\n",__VA_ARGS__)

using std::string;
using namespace MaliSDK;

void calc_mvp(Matrix& mvp);
void setupGraphics(int width, int height);
void renderFrame(void);
void cleanup(void);

void testSetup(int width, int height);
void testDraw(void);

#endif  /* DRAW_H */