#ifndef _CONVERT_H_
#define _CONVERT_H_

#pragma once

#include <iostream>
#include <math.h>
#include <vectors.h>

#define PI 3.14159265358979323846264338327950
extern int image_dims[3];

bool convert (const Flt &, const Flt &, Flt &, Flt &);
bool convert (const Vec &, Vec &);

#endif

