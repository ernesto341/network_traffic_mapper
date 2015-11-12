#ifndef _LINE_H_
#define _LINE_H_
#pragma once

#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <vector>
#include <pthread.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

#include <fonts.h>
#include <convert.h>
#include <vectors.h>
#include <loc.h>

using namespace std;

inline static Flt getAngle (const Vec src, const Vec dst)
{
	Flt ang = 0.f;

	ang = dst[1] - src[1];
	if (ang == 0.f)
		return (0.f);
	ang = atan((dst[0] - src[0])/(ang));
	ang = (180.f * ang) / PI;

	return ((ang));
}

inline static bool common(const int & p)
{
	if (p > 1 && p < 10000)
		return (true);
	return (false);
}

struct Line
{
	bool checked;
	volatile sig_atomic_t valid;
	char src_ip[16], dst_ip[16];
	int src_port, dst_port, proto;
	Vec color; // used for explosion
	Vec src;
	Vec dst;
	Vec last_src_vec;
	Vec last_dst_vec;
	Vec src_vec;
	Vec dst_vec;
	Vec offset;
	Vec midpoint;
	Vec dir;
	Flt length;
	Flt step_len;
	Flt pos;
	Flt angle;
	Flt inc;
	Flt inc_min;
	Flt inc_max;

	void setLength(void)
	{
		length = dist(dst_vec, src_vec);
	}

	void setColor(const int port = -1)
	{
		if (port > 0 && port < 65536)
		{
			if ((port) <= (65535 / 3))
			{
				color[0] = (Flt)((Flt)port / 65535. / 3. * 255.);
				color[1] = 0.f;
				color[2] = 0.f;
				if (color[0] < 150.)
				{
					color[0] += 40.;
				}
			}
			else if ((port) <= (65535 / 3 * 2))
			{
				color[0] = 0.f;
				color[1] = (Flt)((Flt)port / 65535. / 3. * 2. * 255.);
				color[2] = 0.f;
				if (color[1] < 150.)
				{
					color[1] += 40.;
				}
			}
			else
			{
				color[0] = 0.f;
				color[1] = 0.f;
				color[2] = (Flt)((Flt)port / 65535. * 255.);
				if (color[2] < 150.)
				{
					color[2] += 40.;
				}
			}
		}
		else
		{
			color[0] = (Flt)(rand() % 255);
			color[1] = (Flt)(rand() % 255);
			color[2] = (Flt)(rand() % 255);
		}
	}

	void setMidpoint(void)
	{
		offset[0] = 0.;
		offset[1] = 0.;
		offset[2] = 0.;
		Vec n;
		Flt tmp = 0.;
		vecSub(dst_vec, src_vec, n);
		vecNormalize(n);
		tmp = n[0];
		n[0] = n[1];
		n[1] = tmp;
		switch(rand() % 3)
		{
			case 0:
				break;
			case 1:
				offset[0] = n[0] * -10./length;
				offset[1] = n[1] * 10./length;
				break;
			case 2:
				offset[0] = n[0] * 10./length;
				offset[1] = n[1] * -10./length;
				break;
		}
		midpoint[0] = dst_vec[0] - src_vec[0] + offset[0];
		midpoint[1] = dst_vec[1] - src_vec[1] + offset[1];
	}

	Line()
	{
		valid = 0;
		color[0] = 0.f;
		color[1] = 0.f;
		color[2] = 0.f;
		dst_vec[0] = 0.;
		dst_vec[1] = 0.;
		dst_vec[2] = 0.;
		src_vec[0] = 0.;
		src_vec[1] = 0.;
		src_vec[2] = 0.;
		midpoint[0] = 0.;
		midpoint[1] = 0.;
		midpoint[2] = 0.;

		dst[0] = 0.;
		dst[1] = 0.;
		dst[2] = 0.;
		src[0] = 0.;
		src[1] = 0.;
		src[2] = 0.;
		step_len = 0.;
		length = 0.;
		pos = 0.;
	}
	Line (const Vec & s, const Vec & d, const char * sip = 0, const char * dip = 0, const int sp = -1, const int dp = -1, Flt sl = 30., Flt p = 0)
	{
		checked = false;
		valid = 1;
		dst[0] = d[0];
		dst[1] = d[1];
		dst[2] = d[2];
		src[0] = s[0];
		src[1] = s[1];
		src[2] = s[2];
		convert(src, src_vec);
		convert(dst, dst_vec);
		step_len = (sl > 0. ? sl : 30.);
		pos = (p >= 0 ? p : 0);
		setLength();
		if (sip)
		{
			strncpy(src_ip, sip, 15);
			src_ip[15] = '\0';
		}
		if (dip)
		{
			strncpy(dst_ip, dip, 15);
			dst_ip[15] = '\0';
		}
		src_port = sp;
		dst_port = dp;
		if (common(dp))
		{
			proto = dp;
			setColor(dp);
		}
		else
		{
			proto = sp;
			setColor(sp);
		}
		//cerr << "color: (" << color[0] << ", " << color[1] << ", " << color[2] << ")\n" << flush;
		setMidpoint();
		angle = getAngle(src_vec, dst_vec);
		vecSub(dst_vec, src_vec, dir);
		vecNormalize(dir);
		inc_min = .01;
		inc_max = .06;
		inc = inc_max;
		last_src_vec[0] = last_dst_vec[0] = src_vec[0];
		last_src_vec[1] = last_dst_vec[1] = src_vec[1];
		last_src_vec[2] = last_dst_vec[2] = src_vec[2];
	}
	~Line()
	{
		valid = 0;
		color[0] = 0.f;
		color[1] = 0.f;
		color[2] = 0.f;
		dst[0] = 0.;
		dst[1] = 0.;
		dst[2] = 0.;
		src[0] = 0.;
		src[1] = 0.;
		src[2] = 0.;
		dst_vec[0] = 0.;
		dst_vec[1] = 0.;
		dst_vec[2] = 0.;
		src_vec[0] = 0.;
		src_vec[1] = 0.;
		src_vec[2] = 0.;
		midpoint[0] = 0.;
		midpoint[1] = 0.;
		midpoint[2] = 0.;
		length = 0.;
		step_len = 0.;
		pos = 0.;
	}
};

extern int LIVE_DATA;
extern size_t xres;
extern size_t yres;
extern pthread_mutex_t lines_mutex;
extern vector<Line> lines;
extern Flt MAX_LENGTH;

void addLine(const Vec &, const Vec &, const char *, const char *, const int &, const int &);
void addLine(const Vec &, const Vec &);
void addLine(Line);
bool valid(const vector<Line>::iterator &);
void iterateLines();

#endif
