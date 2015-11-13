#include <setup.h>

using namespace std;

void set_title(void)
{
	//Set the window title bar.
	XMapWindow(dpy, win);
	XStoreName(dpy, win, "CS371 Project - Network Traffic Mapper");
}

void setup_screen_res(const size_t w, const size_t h)
{
	xres = w;
	yres = h;
	image_dims[0] = xres;
	image_dims[1] = yres;
}

void setup_screen_res(const int w, const int h)
{
	if (w >= 0 && h >= 0)
	{
		xres = (size_t)w;
		yres = (size_t)h;
		image_dims[0] = xres;
		image_dims[1] = yres;
	}
}

void reshape_window(int width, int height)
{
	//window has been resized.
	setup_screen_res(width, height);
	//
	glViewport(0, 0, (GLint)width, (GLint)height);
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	glOrtho(0, xres, 0, yres, -1, 1);
	set_title();
}

void cleanupXWindows(void)
{
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

void initXWindows(void)
{
	Window root;
	GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
	XVisualInfo *vi;
	Colormap cmap;
	XSetWindowAttributes swa;

	setup_screen_res(xres, yres);
	dpy = XOpenDisplay(NULL);
	if(dpy == NULL)
	{
		fprintf(stderr, "\n\tcannot connect to X server\n\n");
		fflush(stderr);
		done = 1;
		exit(EXIT_FAILURE);
	}
	root = DefaultRootWindow(dpy);
	vi = glXChooseVisual(dpy, 0, att);
	if(vi == NULL)
	{
		fprintf(stderr, "\n\tno appropriate visual found\n\n");
		fflush(stderr);
		done = 1;
		exit(EXIT_FAILURE);
	} 
	cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
		StructureNotifyMask | SubstructureNotifyMask;
	win = XCreateWindow(dpy, root, 0, 0, xres, yres, 0,
			vi->depth, InputOutput, vi->visual,
			CWColormap | CWEventMask, &swa);
	set_title();
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
	XFree(vi);
}

void init_textures(void)
{
	//load the images file into a ppm structure.
	string cmd = "";
	cmd = "convert -resize ";
	cmd += to_string(xres);
	cmd += "x";
	cmd += to_string(yres);
	cmd += " ./img/Political_World_Map.jpg ./img/map.ppm\0";
	if (system(cmd.c_str()) != 0)
	{
		cmd = "convert -resize ";
		cmd += to_string(xres);
		cmd += "x";
		cmd += to_string(yres);
		cmd += " ../img/Political_World_Map.jpg ../img/map.ppm\0";
		if (system(cmd.c_str()) < 0)
		{
			cerr << "Unable to find map\n" << flush;
			done = 1;
			exit(EXIT_FAILURE);
		}
		else
		{
			background_map = ppm6GetImage("../img/map.ppm");
		}
	}
	else
	{
		background_map = ppm6GetImage("./img/map.ppm");
	}
	//
	//create opengl texture elements
	glGenTextures(1, &mapTextureId);
	int w = background_map->width;
	int h = background_map->height;
	//
	glBindTexture(GL_TEXTURE_2D, mapTextureId);
	//
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, background_map->data);
}

void check_resize(XEvent *e)
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

void init_opengl(void)
{
	//OpenGL initialization
	glViewport(0, 0, xres, yres);
	glDepthFunc(GL_LESS);
	glDisable(GL_DEPTH_TEST);
	//Initialize matrices
	glMatrixMode(GL_PROJECTION); glLoadIdentity();
	glMatrixMode(GL_MODELVIEW); glLoadIdentity();
	//This sets 2D mode (no perspective)
	glOrtho(0, xres, 0, yres, -1, 1);
	//Clear the screen
	glClearColor(1.0, 1.0, 1.0, 1.0);
}
