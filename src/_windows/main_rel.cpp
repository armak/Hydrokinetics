#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#define VC_LEANMEAN
#define VC_EXTRALEAN
#include <windows.h>
#include <mmsystem.h>
#include "../config.h"
#include <GL/gl.h>
#include "../ext.h"
#include "../shaders/fragment.inl"

#include "../4klang.h"
#include "mmsystem.h"
#include "mmreg.h"

static const PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
    32, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 32, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };

static DEVMODE screenSettings = { {0},
    #if _MSC_VER < 1400
    0,0,148,0,0x001c0000,{0},0,0,0,0,0,0,0,0,0,{0},0,32,XRES,YRES,0,0,      // Visual C++ 6.0
    #else
    0,0,156,0,0x001c0000,{0},0,0,0,0,0,{0},0,32,XRES,YRES,{0}, 0,           // Visual Studio 2005+
    #endif
    #if(WINVER >= 0x0400)
    0,0,0,0,0,0,
    #if (WINVER >= 0x0500) || (_WIN32_WINNT >= 0x0400)
    0,0
    #endif
    #endif
    };

SAMPLE_TYPE	lpSoundBuffer[MAX_SAMPLES * 2];
static HWAVEOUT hWaveOut;

#pragma data_seg(".wavefmt")
static WAVEFORMATEX WaveFMT =
{
#ifdef FLOAT_32BIT	
	WAVE_FORMAT_IEEE_FLOAT,
#else
	WAVE_FORMAT_PCM,
#endif		
	2,                                   // channels
	SAMPLE_RATE,                         // samples per sec
	SAMPLE_RATE*sizeof(SAMPLE_TYPE) * 2, // bytes per sec
	sizeof(SAMPLE_TYPE) * 2,             // block alignment;
	sizeof(SAMPLE_TYPE) * 8,             // bits per sample
	0                                    // extension not needed
};

#pragma data_seg(".wavehdr")
static WAVEHDR WaveHDR =
{
	// MAX_SAMPLES*sizeof(float)*2(stereo)
	(LPSTR)lpSoundBuffer, MAX_SAMPLES*sizeof(SAMPLE_TYPE)*2,0,0,0,0,0,0
};

static MMTIME MMTime =
{
	TIME_SAMPLES, 0
};

#define FULLSCREEN
void entrypoint( void )
{
	// initialize window
	#ifdef FULLSCREEN
    ChangeDisplaySettings(&screenSettings,CDS_FULLSCREEN);
    ShowCursor(0);
	HDC hDC = GetDC(CreateWindow((LPCSTR)0xC018, 0, WS_POPUP | WS_VISIBLE, 0, 0, XRES, YRES, 0, 0, 0, 0));
	#else
	HDC hDC = GetDC(CreateWindow("static", 0, WS_POPUP | WS_VISIBLE, 0, 0, XRES, YRES, 0, 0, 0, 0));
	#endif	

	// initalize opengl
    SetPixelFormat(hDC,ChoosePixelFormat(hDC,&pfd),&pfd);
    wglMakeCurrent(hDC,wglCreateContext(hDC));
    EXT_Init();

	// initialize, compile and link shader(s)
	const int pid = oglCreateProgram();
	const int fsId = oglCreateShader(GL_FRAGMENT_SHADER);
	oglShaderSource(fsId, 1, &fragment, 0);
	oglCompileShader(fsId);
	oglAttachShader(pid, fsId);
	oglLinkProgram(pid);

	// initialize sound
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_4klang_render, lpSoundBuffer, 0, 0);
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveFMT, NULL, 0, CALLBACK_NULL);
	waveOutPrepareHeader(hWaveOut, &WaveHDR, sizeof(WaveHDR));
	waveOutWrite(hWaveOut, &WaveHDR, sizeof(WaveHDR));
	const int to = timeGetTime();
	
    // play intro
    do 
    {
		// update time
		waveOutGetPosition(hWaveOut, &MMTime, sizeof(MMTIME));
        const int t = timeGetTime() - to;
		// use shader and pass time variable
		oglUseProgram(pid);
		oglUniform1i(oglGetUniformLocation(pid, "m"), t);
		// draw to buffer and swap
		glRects(-1, -1, 1, 1);
        SwapBuffers(hDC);
    } while(!GetAsyncKeyState(VK_ESCAPE) && MMTime.u.sample < 6615000);

    #ifdef CLEANDESTROY
    sndPlaySound(0,0);
    ChangeDisplaySettings( 0, 0 );
    ShowCursor(1);
    #endif

    ExitProcess(0);
}
