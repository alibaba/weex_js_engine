#ifndef _WEEX_PROXY_H_
#define _WEEX_PROXY_H_

#include <jni.h>
#include <sys/types.h>

namespace WEEXJSC
{
    typedef const char* (*FunType)(const char*, int, const char*);
    const char* callGCanvasFun(FunType fp, const char* conextId, int x, const char* args);
    extern "C" void Inject_GCanvasFunc(FunType fp);

}

#endif //_WEEX_PROXY_H_