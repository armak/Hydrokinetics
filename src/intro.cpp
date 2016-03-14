#ifdef DEBUG
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#define VC_LEANMEAN
#define VC_EXTRALEAN
#include <windows.h>
#include "intro.h"
#include "config.h"

void initShader(int* p_pid, const char* fs)
{
    p_pid[0] = oglCreateProgram();                           
	const int fsId = oglCreateShader(GL_FRAGMENT_SHADER);
	oglShaderSource(fsId,1,&fs,0);
    oglCompileShader(fsId);
	oglAttachShader(p_pid[0],fsId);
	oglLinkProgram(p_pid[0]);

    int result;
    char info[1536];
	//oglGetObjectParameteriv( vsId,   GL_OBJECT_COMPILE_STATUS_ARB, &result ); oglGetInfoLog( vsId,   1024, NULL, (char *)info ); if (!result) MessageBox(NULL, info, "", 0x00000000L);
	oglGetObjectParameteriv( fsId, GL_OBJECT_COMPILE_STATUS_ARB,   &result); oglGetInfoLog(fsId, 1024, NULL, (char *)info); if (!result) MessageBox(NULL, info, "", 0x00000000L);
	oglGetObjectParameteriv( p_pid[0], GL_OBJECT_LINK_STATUS_ARB,  &result ); oglGetInfoLog( p_pid[0], 1024, NULL, (char*)info ); //if (!result) MessageBox(NULL, info, "", 0x00000000L);
    
}
void intro_init(void)
{
	EXT_Init();
    //initShader(&pid, fragment);
}
#endif
