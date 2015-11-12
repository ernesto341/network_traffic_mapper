#include <line.h>

using namespace std;

void addLine(const Vec & src, const Vec & dst, const char * sip, const char * dip, const int & sp, const int & dp)
{
	Line line(src, dst, sip, dip, sp, dp);
	line.valid = 1;
	if (LIVE_DATA == 1)
	{
		pthread_mutex_lock(&lines_mutex);
	}
	lines.push_back(line);
	if (LIVE_DATA == 1)
	{
		pthread_mutex_unlock(&lines_mutex);
	}
}

void addLine(const Vec & src, const Vec & dst)
{
	Line line(src, dst);
	line.valid = 1;
	if (LIVE_DATA == 1)
	{
		pthread_mutex_lock(&lines_mutex);
	}
	lines.push_back(line);
	if (LIVE_DATA == 1)
	{
		pthread_mutex_unlock(&lines_mutex);
	}
}

void addLine(Line line)
{
	line.valid = 1;
	if (LIVE_DATA == 1)
	{
		pthread_mutex_lock(&lines_mutex);
	}
	lines.push_back(line);
	if (LIVE_DATA == 1)
	{
		pthread_mutex_unlock(&lines_mutex);
	}
}

void displayStats(void)
{
	Rect r;
	static vector<Line> data;
	vector<Line>::iterator it, end;
	char buf[65535];
	size_t i = 0, ct = 0, qty = 8;
	r.bot = yres/4;
	r.left = xres/2;
	r.center = 1;
	ggprint8b(&r, 10, 0x00111111, "Active Traffic");
	ggprint8b(&r, 10, 0x00111111, "===========================================================================================");
	ggprint8b(&r, 10, 0x00111111, "Source IP         -->  Destination IP    :  Port  |  Source Coords  -->  Destination Coords");
	vector<Line>::reverse_iterator rit;
	i = 0;
	ct = ((lines.size() > qty) ? qty : lines.size());
	rit = lines.rend();
	for (; i < ct && rit != lines.rbegin(); --rit, i++)
	{
		if (rit->valid == 1)
		{
			data.insert(data.begin(), *rit);
		}
	}
	while (data.size() > qty)
	{
		data.pop_back();
	}
	end = data.end();
	it = data.begin();
	for (; it != end; ++it)
	{
		memset(buf, 0, 65535);
		snprintf(buf, 65534, "%s  -->  %s  :  %d  |  (%0.8f, %0.8f)  -->  (%0.8f, %0.8f)", it->src_ip, it->dst_ip, it->proto, it->src[0], it->src[1], it->dst[0], it->dst[1]);
		ggprint8b(&r, 10, 0x00111111, buf);
	}
}

bool valid(const vector<Line>::iterator & l)
{
	// check if passing the end point
	if (l->inc <= l->inc_min)
	{
		l->valid = 0;
		return (false);
	}
	if (l->last_dst_vec[0] > xres || l->last_dst_vec[0] < 0.)
	{
		l->valid = 0;
		return (false);
	}
	if (l->last_dst_vec[1] > yres || l->last_dst_vec[1] < 0.)
	{
		l->valid = 0;
		return (false);
	}
	// check if have invalid length, src, dst, etc
	if (!l->checked)
	{
		if (l->length > MAX_LENGTH || l->length < 0.)
		{
			if (l->length > MAX_LENGTH || l->length < 0.)
			{
				l->length = dist(l->dst_vec, l->src_vec);
				l->valid = 0;
				return (false);
			}
		}
		if (l->src_vec[0] < 0. || l->src_vec[0] > xres || l->src_vec[1] < 0. || l->src_vec[1] > yres || dist(l->src_vec, l->dst_vec) < .09)
		{
			l->valid = 0;
			return (false);
		}
		l->checked = true;
	}
	return (l->valid != 0);
}

void iterateLines()
{
	if (LIVE_DATA == 1)
	{
		pthread_mutex_lock(&lines_mutex); // got the lock
	}
	displayStats();
	vector<Line>::iterator begin = lines.begin();
	vector<Line>::iterator end = lines.end();
	Vec start_pt, end_pt;
	Vec tmp1, tmp2;
	Flt widthDiv2 = 5.f, step = 0.;
	Flt ang = 0.f;
	for (begin = lines.begin(); begin != end; ++begin)
	{
		if (!valid(begin))
		{
			continue;
		}
		if (begin->pos == 0)
		{
			// start src explosion
			// add heat
			addSrcLoc(begin->src_vec, begin->color);
		}
		(begin->pos) += begin->inc; // HERE - modify inc when close to start/end
		step = begin->step_len;
		if (dist(start_pt, begin->dst_vec) < begin->step_len)
		{
			step = dist(start_pt, begin->dst_vec) + begin->inc;
		}
		else if (dist(start_pt, begin->src_vec) < begin->step_len)
		{
			step = dist(start_pt, begin->src_vec);
		}
		ang = begin->angle;
		start_pt[0] = begin->last_src_vec[0] + step * begin->dir[0];
		start_pt[1] = begin->last_src_vec[1] + step * begin->dir[1];
		end_pt[0] = (start_pt[0] + step * begin->dir[0]);
		end_pt[1] = (start_pt[1] + step * begin->dir[1]);
		vecSub(begin->dst_vec, end_pt, tmp1);
		vecSub(begin->dst_vec, start_pt, tmp2);
		switch(begin->valid)
		{
			case 1:
				if ((tmp1[0] <= 0. && tmp2[0] >= 0.) ||
						(tmp1[1] <= 0. && tmp2[1] >= 0.) ||
						(tmp1[0] >= 0. && tmp2[0] <= 0.) ||
						(tmp1[1] >= 0. && tmp2[1] <= 0.))
				{
					begin->valid = 0;
					addDstLoc(begin->dst_vec, begin->color);
				}
				break;
			case 2:
				begin->valid = begin->valid + 1;
				break;
			case 3:
				begin->valid = 0;
				break;
			default:
				begin->valid = 0;
				break;
		}
		if (begin->valid == 0)
		{
			continue;
		}
		glPushMatrix();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glTranslatef(start_pt[0], start_pt[1], start_pt[2]);
		if (begin->dst_vec[1] < begin->src_vec[1])
		{
			ang += 180.;
		}
		glRotatef(ang, 0.f, 0.f, -1.f);
		// left side
		glBegin(GL_QUADS);
		glColor4ub(begin->color[0],begin->color[1],begin->color[2],0);
		glVertex3f(-widthDiv2 * .5f, 0.f, 0.f); // tl
		glColor4ub(begin->color[0],begin->color[1],begin->color[2],0);
		glVertex3f(-widthDiv2, step, 0.f); // tr
		glColor4ub(begin->color[0],begin->color[1],begin->color[2],255);
		glVertex3f(0.f, step, 0.f); // br
		glColor4ub(begin->color[0],begin->color[1],begin->color[2],255);
		glVertex3f(0.f, 0.f, 0.f); // bl
		glEnd();
		// right side
		glBegin(GL_QUADS);
		glColor4ub(begin->color[0],begin->color[1],begin->color[2],255);
		glVertex3f(0.f, 0.f, 0.f); // tl
		glColor4ub(begin->color[0],begin->color[1],begin->color[2],255);
		glVertex3f(0.f, step, 0.f); // tr
		glColor4ub(begin->color[0],begin->color[1],begin->color[2],0);
		glVertex3f(widthDiv2, step, 0.f); // br
		glColor4ub(begin->color[0],begin->color[1],begin->color[2],0);
		glVertex3f(widthDiv2 * .5f, 0.f, 0.f); // bl
		glEnd();
		glPopMatrix();
		glDisable(GL_BLEND);

		begin->last_src_vec[0] = start_pt[0];
		begin->last_src_vec[1] = start_pt[1];
		begin->last_dst_vec[0] = end_pt[0];
		begin->last_dst_vec[1] = end_pt[1];
	}
	// HERE - this op cleans up the vector of lines. VERY expensive i think
	bool repeat = true;
	while (repeat)
	{
		repeat = false;
		end = lines.end();
		for (begin = lines.begin(); begin != end; ++begin)
		{
			if (begin->valid == 0)
			{
				repeat = true;
				lines.erase(begin);
				break;
			}
		}
	}
	if (LIVE_DATA == 1)
	{
		pthread_mutex_unlock(&lines_mutex);
	}
}
