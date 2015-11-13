//cs371 Fall 2015
//
//program:       project.cpp
//author:        Ernest Richards
//modified from: bball.cpp by Gordon Griesel
//date:          fall 2015
//
//Depending on your Linux distribution,
//may have to install these packages:
// libx11-dev
// libglew1.6
// libglew1.6-dev
//
#include <time.h>

#include <fonts.h>
#include <capture.h>
#include <setup.h>
#include <gnu/libc-version.h>

using namespace std;

int LIVE_DATA = 0;
int image_dims[3];
size_t xres=840, yres=420;
volatile sig_atomic_t done = 0;
volatile sig_atomic_t restart = 0;
Flt MAX_LENGTH = sqrt((xres * xres) + (yres * yres));

Vec target1 = {35.3460973, -119.1032019, 0};
Vec target2 = {55.7267323, 37.5213289, 0};
Vec target3 = {39.897511, 116.3891782, 0};

vector<Line> lines;
vector<Loc> src_locs;
vector<Loc> dst_locs;
Ppmimage * background_map = NULL;
GLuint mapTextureId;

//X Windows variables
Display *dpy;
Window win;
GLXContext glc;

pthread_mutex_t lines_mutex;
threadStuff threadArgs[MAX_THREADS];

void initXWindows(void);
void init_opengl(void);
void init_textures(void);
void cleanupXWindows(void);
void physics(void);
void render(void);

inline static void title(void)
{
	cout << endl;
	cout << "\t\t\t======================" << endl
		<< "\t\t\tNetwork Traffic Mapper" << endl
		<< "\t\t\t======================" << endl;
	cout << endl;
}

inline static void version(void)
{
	cout << endl;
	cout << "Author: Ernest Richards" << endl
		<< "Written for:" << endl
		<< "\tCS371 Gordon Griesel" << endl
		<< "\tFall 2015" << endl
		<< "\tCSUB" << endl;
	cout << endl;
}

inline static void help(void)
{
	cout << endl
		<< "--help, -h\t\t---\t\tThis Help Message" << endl
		<< "--live-data, -l\t\t---\t\tCapture and Map Live Data" << endl
		<< "--simulate, -s\t\t---\t\tSimulate Live Network Traffic" << endl
		<< "--version, -v\t\t---\t\tVersion" << endl;
	cout << endl;
}

void takeScreenshot(const char *filename, int reset)
{
	static int picnum = 0;
	size_t x,y;
	XWindowAttributes gwa;
	XGetWindowAttributes(dpy, win, &gwa);
	size_t width = (size_t)gwa.width;
	size_t height = (size_t)gwa.height;
	if (reset)
	{
		picnum = 0;
	}
	XImage *image = XGetImage(dpy, win,
			0, 0 , width, height, AllPlanes, ZPixmap);
	Ppmimage *ppmImage = ppm6CreateImage(width, height);
	unsigned char *ptr;
	unsigned long red_mask   = image->red_mask;
	unsigned long green_mask = image->green_mask;
	unsigned long blue_mask  = image->blue_mask;
	unsigned long pixel;
	for (x=0; x<width; x++)
	{
		for (y=0; y<height; y++)
		{
			pixel = XGetPixel(image,x,y);
			ptr = (unsigned char *)ppmImage->data + ((x + width * y) * 3);
			*(ptr+0) = (pixel & red_mask) >> 16;
			*(ptr+1) = (pixel & green_mask) >> 8;
			*(ptr+2) = (pixel & blue_mask);
		}
	}
	//save image
	//generate an iterative filename...
	char ts[256] = "";
	strcpy(ts, filename);
	if (ts[0] == '\0')
	{
		sprintf(ts,"./sshot%03i.ppm", picnum);
		picnum++;
	}
	ppm6SaveImage(ppmImage, ts);
	ppm6CleanupImage(ppmImage);
	XFree(image);
}

inline static void check_resize(XEvent *e)
{
	//The ConfigureNotify is sent by the
	//server if the window is resized.
	if (e->type != ConfigureNotify)
	{
		return;
	}
	XConfigureEvent xce = e->xconfigure;
	size_t x = 0, y = 0;
	if ((xce.width >= 0 && xce.height >= 0) && ((size_t)xce.width != xres || (size_t)xce.height != yres))
	{
		//Window size did change.
		// enforce 2x1 dimensions
		if (xce.width < xce.height)
		{
			x = (size_t)xce.width;
			y = x * 2;
			MAX_LENGTH = sqrt((xres * xres) + (yres * yres));
		}
		else
		{
			y = (size_t)xce.height;
			x = y * 2;
		}
		reshape_window(x, y);
	}
}

inline static void check_keys(XEvent *e)
{
	string cmd;
	//Was there input from the keyboard?
	if (e->type == KeyPress)
	{
		int key = XLookupKeysym(&e->xkey, 0);
		switch(key)
		{
			case XK_Escape:
				done = 1;
				break;
			case XK_s:
				takeScreenshot("", 0);
				break;
			case (XK_a):
				for (int j = 0; j < 50; j++)
				{
					takeScreenshot("", 0);
					render();
					physics();
					glXSwapBuffers(dpy, win);
				}
				cmd = "";
				cmd += "convert sshot*.ppm animation.gif\0";
				if (system(cmd.c_str()) != 0) {}
				cmd = "rm -f sshot*.ppm\0";
				if (system(cmd.c_str()) != 0) {}
				break;
		}
	}
}

int main(int argc, char * argv[], char *envp[])
{
	done = 0;
	LIVE_DATA = 0;
	title();
	if (argc < 2) // no args passed
	{
		help();
	}
	char tmp[256] = {0};
	char * dev = 0;
	char errbuf[PCAP_ERRBUF_SIZE];
	int i = 1;
	int free_dev = 0;
	while (i < argc)
	{
		strncpy(tmp, argv[i++], 255);
		if (strcmp(tmp, "-l") == 0 || strcmp(tmp, "--live-data") == 0)
		{
			if (atof(gnu_get_libc_version()) < 2.14)
			{
				LIVE_DATA = 2;
			}
			else
			{
				LIVE_DATA = 1;
			}
		}
		else if (strcmp(tmp, "-s") == 0 || strcmp(tmp, "--simulate") == 0)
		{
			LIVE_DATA = 2;
		}
		else if (strcmp(tmp, "-v") == 0 || strcmp(tmp, "--version") == 0)
		{
			version();
			return (0);
		}
		else if (strcmp(tmp, "-h") == 0 || strcmp(tmp, "--help") == 0)
		{
			version();
			help();
			return (0);
		}
		else if (strcmp(tmp, "-i") == 0 || strcmp(tmp, "--interface") == 0)
		{
			memset(tmp, 0, 255);
			strncpy(tmp, argv[i++], 255);
			size_t len = strlen(tmp);
			if (len + 1 > 0xffffffff) // potential overflow in any subsequent signed int math
			{
				return(1);
			}
			dev = (char *)malloc(sizeof(char) * len + 1);
			if (dev == 0)
			{
				return (1);
			}
			free_dev = 1;
			memset(dev, 0, len + 1);
			memcpy(dev, tmp, len);
		}
		else if (strcmp(tmp, "-f") == 0 ||  strstr(tmp, "--file") != 0)
		{
			// data in the file should be in the from of src_ip:src_prt-dst_ip:dst_prt
			cout << "This functionality has not been activated yet! Tune in later!\n";
			return (0);
		}
		memset(tmp, 0, 255);
	}
	initXWindows();
	init_opengl();
	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
	init_textures();
	XEvent e;
	if (LIVE_DATA == 1)
	{
		pthread_mutex_init(&lines_mutex, NULL);
		if (dev == 0)
		{
			dev = pcap_lookupdev(errbuf);
			if (dev == 000)
			{
				cerr << "Unable to lookup a capture device, simulating live traffic!\n" << flush;
				for (size_t i = 0; i < MAX_THREADS; i++)
				{
					threadArgs[i].tid = i;
					pthread_mutex_init(&threadArgs[i].live_data_mutex, NULL);
					pthread_create(&(threadArgs[i].tid), NULL, simulateCapture, (void *)(&(threadArgs[i])));
				}
			}
			else
			{
				for (size_t i = 0; i < MAX_THREADS; i++)
				{
					threadArgs[i].tid = i;
					threadArgs[i].dev = dev;
					pthread_mutex_init(&threadArgs[i].live_data_mutex, NULL);
					pthread_create(&(threadArgs[i].tid), NULL, capture, (void *)(&(threadArgs[i])));
				}
			}
		}
	}
	else if (LIVE_DATA == 2)
	{
		LIVE_DATA = 1;
		pthread_mutex_init(&lines_mutex, NULL);
		for (size_t i = 0; i < MAX_THREADS; i++)
		{
			threadArgs[i].tid = i;
			pthread_mutex_init(&threadArgs[i].live_data_mutex, NULL);
			pthread_create(&(threadArgs[i].tid), NULL, simulateCapture, (void *)(&(threadArgs[i])));
		}
	}
	while(!done)
	{
		if (restart > 10)
		{
			cerr << "restarting due to excessive invalid lengths...\n" << endl << flush;
			done = 1;
			break;
		}
		while(XPending(dpy))
		{
			XNextEvent(dpy, &e);
			check_resize(&e);
			check_keys(&e);
		}
		render();
		physics();
		glXSwapBuffers(dpy, win);
		fib(20);
	}
	if (LIVE_DATA == 1)
	{
		for (size_t i = 0; i < MAX_THREADS; i++)
		{
			pthread_join(threadArgs[i].tid, NULL);
			pthread_mutex_destroy(&threadArgs[i].live_data_mutex);
		}
		pthread_mutex_destroy(&lines_mutex);
		if (free_dev == 1 && dev != 000)
		{
			free(dev);
			dev = 000;
		}
	}
	cleanupXWindows();
	cleanup_fonts();
	if (restart > 10)
	{
		execve("./project\0", argv, envp);
	}
	return (0);
}

void physics(void)
{
	if (LIVE_DATA == 0)
	{
		Vec tmp1, tmp2;
		if (lines.size() < 5 && rand() % 4 == 0)
		{
			switch(rand() % 3)
			{
				case 0:
					tmp1[0] = target1[0];
					tmp1[1] = target1[1];
					tmp1[2] = 0.;
					tmp2[0] = target2[0];
					tmp2[1] = target2[1];
					tmp2[2] = 0.;
					if (rand() % 2)
					{
						addLine(tmp1, tmp2, "136.168.201.100\0", "138.156.174.100\0", 43245, 80);
					}
					else
					{
						addLine(tmp2, tmp1, "136.168.201.100\0", "138.156.174.100\0", 43245, 80);
					}
					break;
				case 1:
					tmp1[0] = target1[0];
					tmp1[1] = target1[1];
					tmp1[2] = 0.;
					tmp2[0] = target3[0];
					tmp2[1] = target3[1];
					tmp2[2] = 0.;
					if (rand() % 2)
					{
						addLine(tmp1, tmp2, "136.168.201.100\0", "87.54.23.123\0", 62135, 21);
					}
					else
					{
						addLine(tmp2, tmp1, "136.168.201.100\0", "87.54.23.123\0", 62135, 21);
					}
					break;
				case 2:
					tmp1[0] = target3[0];
					tmp1[1] = target3[1];
					tmp1[2] = 0.;
					tmp2[0] = target2[0];
					tmp2[1] = target2[1];
					tmp2[2] = 0.;
					if (rand() % 2)
					{
						addLine(tmp1, tmp2, "136.168.201.100\0", "123.94.22.85\0", 10441, 23);
					}
					else
					{
						addLine(tmp2, tmp1, "136.168.201.100\0", "123.94.22.85\0", 10441, 23);
					}
					break;
			}
		}
	}
	trackSourceExplosions();
	trackDestinationExplosions();
	iterateLines();
}

void render(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3ub(135,135,135); // tint. meh.
	glBindTexture(GL_TEXTURE_2D, mapTextureId);
	glBegin(GL_QUADS);
	glNormal3f(0.0,0.0,1.0);

	glTexCoord2f(-1.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(xres, 0.0, 0.0);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(xres, yres, 0.0);

	glTexCoord2f(-1.0, 0.0);
	glVertex3f(0.0, yres, 0.0);

	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	Rect r;
	r.bot = yres - 20;
	r.left = 10;
	r.center = 0;
	if (xres > 1000)
	{
		ggprint16(&r, 16, 0x00111111, "CS371 Project");
		ggprint16(&r, 16, 0x00111111, "Network Traffic Mapper");
		ggprint16(&r, 16, 0x00111111, "s - take a screenshot");
		ggprint16(&r, 16, 0x00111111, "a - produce an animation");
	}
	else
	{
		ggprint8b(&r, 16, 0x00111111, "CS371 Project");
		ggprint8b(&r, 16, 0x00111111, "Network Traffic Mapper");
		ggprint8b(&r, 16, 0x00111111, "s - take a screenshot");
		ggprint8b(&r, 16, 0x00111111, "a - produce an animation");
	}
}



