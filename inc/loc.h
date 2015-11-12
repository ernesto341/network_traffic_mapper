#ifndef _LOC_H_
#define _LOC_H_
#pragma once

#include <cstdlib>
#include <cstdio>
#include <signal.h>
#include <vector>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include <vectors.h>

using namespace std;

struct Loc
{
	volatile sig_atomic_t valid;
	Vec pos;
	Vec expl_cent;
	Vec color;
	Flt heat;
	Flt max_radius;
	Flt cur_radius;
	Loc()
	{
		valid = 1;
		color[0] = 0.f;
		color[1] = 0.f;
		color[2] = 0.f;
		expl_cent[0] = -1.f;
		expl_cent[1] = -1.f;
		expl_cent[2] = -1.f;
		pos[0] = 0.f;
		pos[1] = 0.f;
		pos[2] = 0.f;
		heat = 0.f;
		max_radius = 65.f;
		cur_radius = 5.f;
	}
	Loc (const Vec p, const Vec c, Flt h = 0.f, Flt r = 75.f)
	{
		valid = 1;
		color[0] = c[0];
		color[1] = c[1];
		color[2] = c[2];
		expl_cent[0] = -1.f;
		expl_cent[1] = -1.f;
		expl_cent[2] = -1.f;
		pos[0] = p[0];
		pos[1] = p[1];
		pos[2] = p[2];
		max_radius = 75.f;
		cur_radius = 5.f;
		heat = 0.f;
		if (r > 1.f)
			max_radius = r;
		if (h >= 0.f)
			heat = h;
	}
	~Loc()
	{
		valid = 1;
		color[0] = 0.f;
		color[1] = 0.f;
		color[2] = 0.f;
		expl_cent[0] = 0.f;
		expl_cent[1] = 0.f;
		expl_cent[2] = 0.f;
		pos[0] = 0.f;
		pos[1] = 0.f;
		pos[2] = 0.f;
		heat = 0.f;
		max_radius = 0.f;
		cur_radius = 0.f;
	}
};

extern volatile sig_atomic_t done;
extern vector<Loc> src_locs;
extern vector<Loc> dst_locs;

void addSrcLoc(const Vec & v, const Vec & c, const Flt h = 0.5f);
void addSrcLoc(Loc);
void addDstLoc(const Vec & v, const Vec & c, const Flt h = 0.5f);
void addDstLoc(Loc);
vector<Loc>::iterator locExists (const Vec &, const int &);
Flt heatSum (const Vec &, const int &);
void trackSourceExplosions();
void trackDestinationExplosions();

#endif
