#ifndef _SETUP_H_
#define _SETUP_H_

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <unistd.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <signal.h>

#include <ppm.h>

using namespace std;

extern size_t xres;
extern size_t yres;
extern int image_dims[3];
extern volatile sig_atomic_t done;
extern Display *dpy;
extern Window win;
extern GLuint mapTextureId;
extern GLXContext glc;
extern Ppmimage * background_map;

void cleanupXWindows(void);
void initXWindows(void);
void init_textures(void);
void init_opengl(void);
void set_title(void);
void setup_screen_res(const size_t, const size_t);
void setup_screen_res(const int, const int);
void reshape_window(int, int);

#endif
