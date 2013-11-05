
// Common includes
#include <assert.h>

#if 1
#define SCREEN_W 800
#define SCREEN_H 600
#else
#define SCREEN_W 1024
#define SCREEN_H 768
#endif

#define SPRITE_DIAM 10
#define SPRITE_RADIUS (SPRITE_DIAM/2)

// Tweak menu
#define TWEAK_MENU _DEBUG || 1	// Useful in release for tuning water behavior
#if TWEAK_MENU
#include "AntTweakBar.h"
TwBar *TwkBar();
#endif // TWEAK_MENU

