#pragma once

/*
Wrapper is necessary to prevent conflicts with Windows macros.
It does not include the actual raylib, but another wrapper, raylib c++.
*/





// Keep track of whether we've saved Windows macros
#ifndef RAYLIB_WRAPPER_INCLUDED
#define RAYLIB_WRAPPER_INCLUDED

// Save all potentially conflicting Windows macros before including raylib
#ifdef _WIN32
#ifdef near
#define SAVED_NEAR near
#undef near
#endif

#ifdef far
#define SAVED_FAR far
#undef far
#endif

#ifdef PlaySound
#define WIN_PlaySound PlaySound
#undef PlaySound
#endif

#ifdef CloseWindow
#define WIN_CloseWindow CloseWindow
#undef CloseWindow
#endif

#ifdef DrawText
#define WIN_DrawText DrawText
#undef DrawText
#endif

#ifdef LoadImage
#define WIN_LoadImage LoadImage
#undef LoadImage
#endif

#ifdef ShowCursor
#define WIN_ShowCursor ShowCursor
#undef ShowCursor
#endif

// Additional Windows macros that might conflict with raylib
#ifdef DrawLine
#define WIN_DrawLine DrawLine
#undef DrawLine
#endif

#ifdef DrawCircle
#define WIN_DrawCircle DrawCircle
#undef DrawCircle
#endif

#ifdef DrawRectangle
#define WIN_DrawRectangle DrawRectangle
#undef DrawRectangle
#endif

#ifdef Beep
#define WIN_Beep Beep
#undef Beep
#endif

#ifdef CreateWindow
#define WIN_CreateWindow CreateWindow
#undef CreateWindow
#endif

#ifdef SetCursor
#define WIN_SetCursor SetCursor
#undef SetCursor
#endif
#endif

// Include raylib with conflicts prevented

#include "raylib-cpp.hpp"

// If we need Windows functions, redefine them with the WIN_ prefix
// #define PlaySound WIN_PlaySound
// etc.

#endif // RAYLIB_WRAPPER_INCLUDED
