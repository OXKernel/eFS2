//
// @file:
//      bitmap.h
//
// @description:
//      Get and Set bits in a char stream of data.
//      The length of the data is not relevant and
//      can be arbitrary large.
//
// @author:
//      Dr. Roger G. Doss, PhD
//
#ifndef _BITMAP_H
#define _BITMAP_H

int bit_get (char *data, int bit);
void bit_set(char *data, int bit, int value);

#endif
