#	pragma once
#	include "Interface/RenderSystemInterface.h"
#	include "libs/Math/vec2.h"
#	include "libs/Math/box2.h"


#define NO_SDL_GLEXT
//#define GLEW_STATIC
#include "gl/glew.h"
#include <SDL_opengl.h>

//#include "windows.h"
//#include "gl/gl.h"


//#define GL_GLEXT_PROTOTYPES 1

//#include "gl/glext.h"
#	include <map>

typedef std::map<Menge::String, Menge::String> NameValuePairList;

#include <SDL.h>

class SDLWindow
{
private:
	SDL_Surface* mScreen;
	bool mActive;
	bool mClosed;

	void processEvents(void);

	int (*glXGetVideoSyncSGI) (unsigned int *);
	int (*glXWaitVideoSyncSGI) (int, int, unsigned int *);

public:
	SDLWindow();
	~SDLWindow();

	void create(const Menge::String& name, unsigned int width, unsigned int height,
		bool fullScreen, const NameValuePairList * miscParams);
	
	void destroy(void);
	
	bool isActive(void) const;
	
	bool isClosed(void) const;
	
	void reposition(int left, int top);
	
	void resize(unsigned int width, unsigned int height);
	
	void swapBuffers(bool waitForVSync);

	bool isFullScreen(void) const
	{
		return ( mScreen->flags & SDL_FULLSCREEN ) == SDL_FULLSCREEN;
	}
};