#include <cstdio>
#include <cstdlib>
#include "Draw.h"
//*****************************
#include "PCUtils.h"
//*****************************
#include <string>
#include <cmath>

using namespace std;

int main(){
	SetupWindowAndGLContext();
	setupGraphics(1366, 768);
	// testSetup(1366,768);
	while(1){
		renderFrame();
		// testDraw();
		SwapBuffer();
		if(PollForESC()){
			cleanup();
			break;
		}
	}
	// PrintNewTriTable();
	return 0;
}

// extern "C"
// {
//     JNIEXPORT void JNICALL Java_com_arm_malideveloper_openglessdk_metaballs_NativeLibrary_init
//     (JNIEnv *env, jclass jcls, jint width, jint height)
//     {
//         /* Initialze OpenGL ES and model environment for metaballs: allocate buffers and textures, bind them, etc. */
//         setupGraphics(width, height);
//     }

//     JNIEXPORT void JNICALL Java_com_arm_malideveloper_openglessdk_metaballs_NativeLibrary_step
//     (JNIEnv *env, jclass jcls)
//     {
//         /* Render a frame */
//         renderFrame();
//     }

//     JNIEXPORT void JNICALL Java_com_arm_malideveloper_openglessdk_metaballs_NativeLibrary_uninit
//     (JNIEnv *, jclass)
//     {
//         cleanup();
//     }
// }