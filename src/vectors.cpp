#include <vectors.h>

Flt dist (const Vec a)
{
	return (sqrt(((a[0]) * (a[0])) + ((a[1]) * (a[1])) + ((a[2]) * (a[2]))));
}

Flt dist (const Vec & a, const Vec & b)
{
	return (sqrt(((b[0] - a[0]) * (b[0] - a[0])) + ((b[1] - a[1]) * (b[1] - a[1])) + ((b[2] - a[2]) * (b[2] - a[2]))));
}

void vecNormalize(Vec v)
{
	Flt len = dist(v);
	if (len == 0.0)
	{
		v[0] = 0.f;
		v[1] = 0.f;
		v[2] = 1.f;
		return;
	}
	len = 1.0 / len;
	v[0] *= len;
	v[1] *= len;
	v[2] *= len;
}

void vecSub(const Vec & v0, const Vec & v1, Vec dest)
{
	dest[0] = v0[0] - v1[0];
	dest[1] = v0[1] - v1[1];
	dest[2] = v0[2] - v1[2];
}
