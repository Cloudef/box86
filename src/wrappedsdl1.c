#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <dlfcn.h>

#include "wrappedlibs.h"

#include "debug.h"
#include "bridge.h"
#include "library_private.h"
#include "x86emu.h"
#include "callback.h"
#include "librarian.h"
#include "x86emu_private.h"
#include "box86context.h"
#include "sdl1rwops.h"

#include "x86trace.h"
#include "khash.h"

const char* sdl1Name = "libSDL-1.2.so.0";
#define LIBNAME sdl1

int sdl_Yes() { return 1; }
int sdl_No() { return 0; }
int EXPORT my_SDL_Has3DNow() __attribute__((alias("sdl_No")));
int EXPORT my_SDL_Has3DNowExt() __attribute__((alias("sdl_No")));
int EXPORT my_SDL_HasAltiVec() __attribute__((alias("sdl_No")));
int EXPORT my_SDL_HasMMX() __attribute__((alias("sdl_Yes")));
int EXPORT my_SDL_HasMMXExt() __attribute__((alias("sdl_Yes")));
int EXPORT my_SDL_HasRDTSC() __attribute__((alias("sdl_Yes")));
int EXPORT my_SDL_HasSSE() __attribute__((alias("sdl_Yes")));
int EXPORT my_SDL_HasSSE2() __attribute__((alias("sdl_Yes")));

typedef struct {
  int32_t freq;
  uint16_t format;
  uint8_t channels;
  uint8_t silence;
  uint16_t samples;
  uint32_t size;
  void (*callback)(void *userdata, uint8_t *stream, int32_t len);
  void *userdata;
} SDL_AudioSpec;

// TODO: put the wrapper type in a dedicate include
typedef void* (*pFpi_t)(void*, int32_t);
typedef void* (*pFpp_t)(void*, void*);
typedef int32_t (*iFppi_t)(void*, void*, int32_t);
typedef void* (*pFpippp_t)(void*, int32_t, void*, void*, void*);
typedef void  (*vFp_t)(void*);
typedef uint32_t (*uFp_t)(void*);
typedef uint64_t (*UFp_t)(void*);
typedef uint32_t (*uFu_t)(uint32_t);
typedef int32_t (*iFpp_t)(void*, void*);
typedef uint32_t (*uFpW_t)(void*, uint16_t);
typedef uint32_t (*uFpu_t)(void*, uint32_t);
typedef uint32_t (*uFpU_t)(void*, uint64_t);
typedef uint32_t (*uFupp_t)(uint32_t, void*, void*);

KHASH_MAP_INIT_INT(timercb, x86emu_t*)

typedef struct sdl1_my_s {
    // functions
    iFpp_t     SDL_OpenAudio;
    pFpi_t     SDL_LoadBMP_RW;
    pFpi_t     SDL_RWFromConstMem;
    pFpi_t     SDL_RWFromFP;
    pFpp_t     SDL_RWFromFile;
    pFpi_t     SDL_RWFromMem;
    iFppi_t    SDL_SaveBMP_RW;
    pFpippp_t  SDL_LoadWAV_RW;
    vFp_t      SDL_FreeRW;
    uFp_t      SDL_ReadBE16;
    uFp_t      SDL_ReadBE32;
    UFp_t      SDL_ReadBE64;
    uFp_t      SDL_ReadLE16;
    uFp_t      SDL_ReadLE32;
    UFp_t      SDL_ReadLE64;
    uFpW_t     SDL_WriteBE16;
    uFpu_t     SDL_WriteBE32;
    uFpU_t     SDL_WriteBE64;
    uFpW_t     SDL_WriteLE16;
    uFpu_t     SDL_WriteLE32;
    uFpU_t     SDL_WriteLE64;
    uFupp_t    SDL_AddTimer;
    uFu_t      SDL_RemoveTimer;
    // timer map
    kh_timercb_t    *timercb;
    uint32_t        settimer;
} sdl1_my_t;

void* getSDL1My(library_t* lib)
{
    sdl1_my_t* my = (sdl1_my_t*)calloc(1, sizeof(sdl1_my_t));
    #define GO(A, W) my->A = (W)dlsym(lib->priv.w.lib, #A);
    GO(SDL_OpenAudio, iFpp_t)
    GO(SDL_LoadBMP_RW, pFpi_t)
    GO(SDL_RWFromConstMem, pFpi_t)
    GO(SDL_RWFromFP, pFpi_t)
    GO(SDL_RWFromFile, pFpp_t)
    GO(SDL_RWFromMem, pFpi_t)
    GO(SDL_SaveBMP_RW, iFppi_t)
    GO(SDL_LoadWAV_RW, pFpippp_t)
    GO(SDL_FreeRW, vFp_t)
    GO(SDL_ReadBE16, uFp_t)
    GO(SDL_ReadBE32, uFp_t)
    GO(SDL_ReadBE64, UFp_t)
    GO(SDL_ReadLE16, uFp_t)
    GO(SDL_ReadLE32, uFp_t)
    GO(SDL_ReadLE64, UFp_t)
    GO(SDL_WriteBE16, uFpW_t)
    GO(SDL_WriteBE32, uFpu_t)
    GO(SDL_WriteBE64, uFpU_t)
    GO(SDL_WriteLE16, uFpW_t)
    GO(SDL_WriteLE32, uFpu_t)
    GO(SDL_WriteLE64, uFpU_t)
    GO(SDL_AddTimer, uFupp_t)
    GO(SDL_RemoveTimer, uFu_t)
    #undef GO
    my->timercb = kh_init(timercb);
    return my;
}

void freeSDL1My(void* lib)
{
    sdl1_my_t *my = (sdl1_my_t *)lib;
    x86emu_t *x;
    kh_foreach_value(my->timercb, x, 
        FreeCallback(x);
    );
    kh_destroy(timercb, my->timercb);
}

void sdl1Callback(void *userdata, uint8_t *stream, int32_t len)
{
    x86emu_t *emu = (x86emu_t*) userdata;
    SetCallbackArg(emu, 1, stream);
    SetCallbackArg(emu, 2, (void*)len);
    RunCallback(emu);
}

uint32_t sdl1TimerCallback(void *userdata)
{
    x86emu_t *emu = (x86emu_t*) userdata;
    RunCallback(emu);
    return R_EAX;
}

// TODO: track the memory for those callback
int EXPORT my_SDL_OpenAudio(x86emu_t* emu, void* d, void* o)
{
    SDL_AudioSpec *desired = (SDL_AudioSpec*)d;
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    // create a callback
    void *fnc = (void*)desired->callback;
    void *olduser = desired->userdata;
    x86emu_t *cbemu = (desired->callback)?AddCallback(emu, (uintptr_t)fnc, 3, olduser, NULL, NULL, NULL):NULL;
    desired->callback = sdl1Callback;
    desired->userdata = cbemu;
    int ret = my->SDL_OpenAudio(desired, (SDL_AudioSpec*)o);
    if (ret!=0) {
        // error, clean the callback...
        desired->callback = fnc;
        desired->userdata = olduser;
        FreeCallback(cbemu);
        return ret;
    }
    // put back stuff in place?
    desired->callback = fnc;
    desired->userdata = olduser;

    return ret;
}

void EXPORT *my_SDL_LoadBMP_RW(x86emu_t* emu, void* a, int b)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    void* r = my->SDL_LoadBMP_RW(a, b);
    if(b==0)
        RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}
int32_t EXPORT my_SDL_SaveBMP_RW(x86emu_t* emu, void* a, void* b, int c)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    int32_t r = my->SDL_SaveBMP_RW(a, b, c);
    if(c==0)
        RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}
void EXPORT *my_SDL_LoadWAV_RW(x86emu_t* emu, void* a, int b, void* c, void* d, void* e)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    void* r = my->SDL_LoadWAV_RW(a, b, c, d, e);
    if(b==0)
        RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}
uint32_t EXPORT my_SDL_ReadBE16(x86emu_t* emu, void* a)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    uint32_t r = my->SDL_ReadBE16(a);
    RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}
uint32_t EXPORT my_SDL_ReadBE32(x86emu_t* emu, void* a)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    uint32_t r = my->SDL_ReadBE32(a);
    RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}
uint64_t EXPORT my_SDL_ReadBE64(x86emu_t* emu, void* a)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    uint64_t r = my->SDL_ReadBE64(a);
    RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}
uint32_t EXPORT my_SDL_ReadLE16(x86emu_t* emu, void* a)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    uint32_t r = my->SDL_ReadLE16(a);
    RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}
uint32_t EXPORT my_SDL_ReadLE32(x86emu_t* emu, void* a)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    uint32_t r = my->SDL_ReadLE32(a);
    RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}
uint64_t EXPORT my_SDL_ReadLE64(x86emu_t* emu, void* a)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    uint64_t r = my->SDL_ReadLE64(a);
    RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}
uint32_t EXPORT my_SDL_WriteBE16(x86emu_t* emu, void* a, uint16_t v)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    uint32_t r = my->SDL_WriteBE16(a, v);
    RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}
uint32_t EXPORT my_SDL_WriteBE32(x86emu_t* emu, void* a, uint32_t v)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    uint32_t r = my->SDL_WriteBE32(a, v);
    RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}
uint32_t EXPORT my_SDL_WriteBE64(x86emu_t* emu, void* a, uint64_t v)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    uint32_t r = my->SDL_WriteBE64(a, v);
    RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}
uint32_t EXPORT my_SDL_WriteLE16(x86emu_t* emu, void* a, uint16_t v)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    uint32_t r = my->SDL_WriteLE16(a, v);
    RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}
uint32_t EXPORT my_SDL_WriteLE32(x86emu_t* emu, void* a, uint32_t v)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    uint32_t r = my->SDL_WriteLE32(a, v);
    RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}
uint32_t EXPORT my_SDL_WriteLE64(x86emu_t* emu, void* a, uint64_t v)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    uint32_t r = my->SDL_WriteLE64(a, v);
    RWNativeEnd(emu, (SDL1_RWops_t*)a, &save);
    return r;
}

void EXPORT *my_SDL_RWFromConstMem(x86emu_t* emu, void* a, int b)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    void* r = my->SDL_RWFromConstMem(a, b);
    AddNativeRW(emu, (SDL1_RWops_t*)r);
    return r;
}
void EXPORT *my_SDL_RWFromFP(x86emu_t* emu, void* a, int b)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    void* r = my->SDL_RWFromFP(a, b);
    AddNativeRW(emu, (SDL1_RWops_t*)r);
    return r;
}
void EXPORT *my_SDL_RWFromFile(x86emu_t* emu, void* a, void* b)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    void* r = my->SDL_RWFromFile(a, b);
    AddNativeRW(emu, (SDL1_RWops_t*)r);
    return r;
}
void EXPORT *my_SDL_RWFromMem(x86emu_t* emu, void* a, int b)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    void* r = my->SDL_RWFromMem(a, b);
    AddNativeRW(emu, (SDL1_RWops_t*)r);
    return r;
}

void EXPORT my_SDL_FreeRW(x86emu_t* emu, void* a)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    SDLRWSave_t save;
    RWNativeStart(emu, (SDL1_RWops_t*)a, &save);
    my->SDL_FreeRW(a);
}

uint32_t EXPORT my_SDL_AddTimer(x86emu_t* emu, uint32_t a, void* cb, void* p)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    x86emu_t *cbemu = AddCallback(emu, (uintptr_t)cb, 1, p, NULL, NULL, NULL);
    uint32_t t = my->SDL_AddTimer(a, sdl1TimerCallback, cbemu);
    int ret;
    khint_t k = kh_put(timercb, my->timercb, t, &ret);
    kh_value(my->timercb, k) = cbemu;
    return t;
}

void EXPORT my_SDL_RemoveTimer(x86emu_t* emu, uint32_t t)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    my->SDL_RemoveTimer(t);
    khint_t k = kh_get(timercb,my->timercb, t);
    if(k!=kh_end(my->timercb))
    {
        FreeCallback(kh_value(my->timercb, k));
        kh_del(timercb, my->timercb, k);
    }
}

int32_t EXPORT my_SDL_SetTimer(x86emu_t* emu, uint32_t t, void* p)
{
    sdl1_my_t *my = (sdl1_my_t *)emu->context->sdl1lib->priv.w.p2;
    if(my->settimer) {
        my_SDL_RemoveTimer(emu, my->settimer);
        my->settimer=0;
    }
    if(p)
        my->settimer = my_SDL_AddTimer(emu, t, p, NULL);
    return 0;
}

#define CUSTOM_INIT \
    box86->sdl1lib = lib; \
    lib->priv.w.priv = NewSDL1RWops(); \
    lib->priv.w.p2 = getSDL1My(lib);

#define CUSTOM_FINI \
    FreeSDL1RWops((sdl1rwops_t**)&lib->priv.w.priv); \
    freeSDL1My(lib->priv.w.p2); \
    free(lib->priv.w.p2); \
    ((box86context_t*)(lib->context))->sdl1lib = NULL;

#include "wrappedlib_init.h"

