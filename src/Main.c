/** @file Main.c
 * @author    Michael Fitzmayer
 * @copyright "THE BEER-WARE LICENCE" (Revision 42)
 */

#include <stdint.h>
#include <stdlib.h>
#include "Background.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#define EXIT_UNSET 2

int32_t g_s32ExecStatus = EXIT_UNSET;

int32_t main()
{

    return g_s32ExecStatus;
}
