/**
 * @file      Macros.h
 * @author    Michael Fitzmayer
 * @copyright "THE BEER-WARE LICENCE" (Revision 42)
 */

#ifndef _MACROS_H_
#define _MACROS_H_

#define FLAG_CLEAR(number, bit)      (number) &= ~(1 << (bit))
#define FLAG_IS_NOT_SET(number, bit) (0 == (((number) >> (bit)) & 1))
#define FLAG_IS_SET(number, bit)     (((number) >> (bit)) & 1)
#define FLAG_SET(number, bit)        (number) |= 1 << (bit)
#define FLAG_TOGGLE(number, bit)     (number) ^= 1 << (bit)

#endif // _MACROS_H_
