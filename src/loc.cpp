#include <loc.h>

using namespace std;

void addSrcLoc(const Vec & v, const Vec & c, const Flt h)
{
	Loc loc(v, c, h);
	loc.valid = 1;
	src_locs.push_back(loc);
}

void addSrcLoc(Loc loc)
{
	loc.valid = 1;
	src_locs.push_back(loc);
}

void addDstLoc(const Vec & v, const Vec & c, const Flt h)
{
	Loc loc(v, c, h);
	loc.valid = 1;
	dst_locs.push_back(loc);
}

void addDstLoc(Loc loc)
{
	loc.valid = 1;
	dst_locs.push_back(loc);
}

// access an arbitrary loc by position
vector<Loc>::iterator locExists (const Vec & pos, const int & src)
{
	vector<Loc>::iterator cur;
	vector<Loc>::iterator end;
	if (src == 1)
	{
		cur = src_locs.begin();
		end = src_locs.end();
	}
	else
	{
		cur = dst_locs.begin();
		end = dst_locs.end();
	}
	while (cur != end)
	{
		if (cur->pos[0] == pos[0] &&
				cur->pos[1] == pos[1] &&
				cur->pos[2] == pos[2])
			return (cur);
		++cur;
	}
	return (cur);
}

Flt heatSum (const Vec & pos, const int & src)
{
	Flt h = 0.f;
	vector<Loc>::iterator cur;
	vector<Loc>::iterator end;
	if (src == 1)
	{
		cur = src_locs.begin();
		end = src_locs.end();
	}
	else
	{
		cur = dst_locs.begin();
		end = dst_locs.end();
	}
	while (cur != end)
	{
		if (cur->pos[0] == pos[0] &&
				cur->pos[1] == pos[1] &&
				cur->pos[2] == pos[2])
			h += cur->heat;
		++cur;
	}
	return (h);
}

void trackSourceExplosions()
{
	static GLUquadric * heatquad = gluNewQuadric();
	static GLUquadric * explquad = gluNewQuadric();
	if (!heatquad || !explquad)
	{
		fprintf(stderr, "Memory allocation error from gluNewQuadric() in trackExplosions()\n");
		fflush(stderr);
		done = 1;
		exit(EXIT_FAILURE);
	}
	vector<Loc>::iterator cur = src_locs.begin();
	vector<Loc>::iterator end = src_locs.end();
	vector<Loc>::iterator l;
	static const Flt width = 7.4f;
	static const Vec hot = {175.f, 30.f, 0.f};
	Flt heat_sum = 0.f;
	for (cur = src_locs.begin(); cur != end; ++cur)
	{
		if (cur->valid == 0)
		{
			continue;
		}
		// heat
		heat_sum = heatSum(cur->pos, 1);
		if (heat_sum > 0.f)
		{
			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glColor4ub((int)(hot[0]), (int)(hot[1]) + (heat_sum * 50.), (int)(hot[2]), 155);
			glTranslatef(cur->pos[0], cur->pos[1], cur->pos[2]);
			gluDisk(heatquad, 0.f, cur->heat * 8.5f, 15, 1);
			glPopMatrix();
			glDisable(GL_BLEND);
			cur->heat -= .009;
		}
		// explosion
		if (cur->cur_radius > 0.f)
		{
			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glColor4ub((int)cur->color[0], (int)cur->color[1], (int)cur->color[2], 255 - 255*cur->cur_radius/cur->max_radius);
			if (cur->expl_cent[0] < 0)
			{
				cur->expl_cent[0] = cur->pos[0] + (Flt)(rand() % 90)/10.f;
				cur->expl_cent[1] = cur->pos[1] + (Flt)(rand() % 90)/10.f;
				cur->expl_cent[2] = cur->pos[2] + (Flt)(rand() % 90)/10.f;
			}
			glTranslatef(cur->expl_cent[0], cur->expl_cent[1], 1.f);
			gluDisk(explquad, cur->cur_radius, cur->cur_radius + width, 35, 1);
			glPopMatrix();
			glDisable(GL_BLEND);
			cur->cur_radius += 1.7f;
			if (cur->cur_radius >= cur->max_radius)
			{
				cur->cur_radius = -1.f;
			}
		}
		if (cur->cur_radius < 0.f && cur->heat < 0.f)
		{
			cur->valid = 0;
		}
	}
	if (done)
	{
		gluDeleteQuadric(heatquad);
		gluDeleteQuadric(explquad);
	}
	// HERE - this op cleans up the vector of lines. VERY expensive i think
	bool repeat = true;
	while (repeat)
	{
		repeat = false;
		end = src_locs.end();
		for (cur = src_locs.begin(); cur != end; ++cur)
		{
			if (cur->valid != 1)
			{
				repeat = true;
				src_locs.erase(cur);
				break;
			}
		}
	}
}

void trackDestinationExplosions()
{
	static GLUquadric * heatquad = gluNewQuadric();
	static GLUquadric * explquad = gluNewQuadric();
	if (!heatquad || !explquad)
	{
		fprintf(stderr, "Memory allocation error from gluNewQuadric() in trackExplosions()\n");
		fflush(stderr);
		done = 1;
		exit(EXIT_FAILURE);
	}
	vector<Loc>::iterator cur = dst_locs.begin();
	vector<Loc>::iterator end = dst_locs.end();
	vector<Loc>::iterator l;
	static const Flt width = 7.4f;
	static const Vec hot = {175.f, 30.f, 0.f};
	Flt heat_sum = 0.f;
	for (cur = dst_locs.begin(); cur != end; ++cur)
	{
		if (cur->valid == 0)
		{
			continue;
		}
		// heat
		heat_sum = heatSum(cur->pos, 0);
		if (heat_sum > 0.f)
		{
			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glColor4ub((int)(hot[0]), (int)(hot[1]) + (heat_sum * 50.), (int)(hot[2]), 155);
			glTranslatef(cur->pos[0], cur->pos[1], cur->pos[2]);
			gluDisk(heatquad, 0.f, cur->heat * 8.5f, 15, 1);
			glPopMatrix();
			glDisable(GL_BLEND);
			cur->heat -= .009;
		}
		// explosion
		if (cur->cur_radius > 0.f)
		{
			glPushMatrix();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glColor4ub((int)cur->color[0], (int)cur->color[1], (int)cur->color[2], 255 - 255*cur->cur_radius/cur->max_radius);
			if (cur->expl_cent[0] < 0)
			{
				cur->expl_cent[0] = cur->pos[0] + (Flt)(rand() % 90)/10.f;
				cur->expl_cent[1] = cur->pos[1] + (Flt)(rand() % 90)/10.f;
				cur->expl_cent[2] = cur->pos[2] + (Flt)(rand() % 90)/10.f;
			}
			glTranslatef(cur->expl_cent[0], cur->expl_cent[1], 1.f);
			gluDisk(explquad, cur->cur_radius, cur->cur_radius + width, 35, 1);
			glPopMatrix();
			glDisable(GL_BLEND);
			cur->cur_radius += 1.7f;
			if (cur->cur_radius >= cur->max_radius)
			{
				cur->cur_radius = -1.f;
			}
		}
		if (cur->cur_radius < 0.f && cur->heat < 0.f)
		{
			cur->valid = 0;
		}
	}
	if (done)
	{
		gluDeleteQuadric(heatquad);
		gluDeleteQuadric(explquad);
	}
	// HERE - this op cleans up the vector of lines. VERY expensive i think
	bool repeat = true;
	while (repeat)
	{
		repeat = false;
		end = dst_locs.end();
		for (cur = dst_locs.begin(); cur != end; ++cur)
		{
			if (cur->valid != 1)
			{
				repeat = true;
				dst_locs.erase(cur);
				break;
			}
		}
	}
}
