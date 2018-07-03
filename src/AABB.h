/**
 * @file    AABB.h
 * @ingroup AABB
 */

#ifndef _AABB_H_
#define _AABB_H_

#include <stdint.h>

/**
 * @ingroup AABB
 */
typedef struct AABB_t {
    double dBottom;
    double dLeft;
    double dRight;
    double dTop;
} AABB;

uint8_t AreIntersecting(AABB stBoxA, AABB stBoxB);

#endif
