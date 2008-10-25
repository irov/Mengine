#include "SDLWindow.h"

#if MENGE_PLATFORM == MENGE_PLATFORM_WINDOWS
//#   include <windows.h>
//#   include <wingdi.h>
//#   include <GL/gl.h>
//#   define GL_GLEXT_PROTOTYPES
//#   include "glprocs.h"
//#   include <GL/glu.h>
#elif MENGE_PLATFORM == MENGE_PLATFORM_APPLE
/*#   include <OpenGL/gl.h>
#   define GL_EXT_texture_env_combine 1
#   include <OpenGL/glext.h>
#   include <OpenGL/glu.h>*/
#endif
//////////////////////////////////////////////////////////////////////////
SDLWindow::SDLWindow() :
mScreen(NULL), mActive(false), mClosed(false)
{
}
//////////////////////////////////////////////////////////////////////////
SDLWindow::~SDLWindow()
{
	// according to http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fSetVideoMode
	// never free the surface returned from SDL_SetVideoMode
	/*if (mScreen != NULL)
	SDL_FreeSurface(mScreen);*/
}
//////////////////////////////////////////////////////////////////////////
void SDLWindow::create(const Menge::String& name, unsigned int width, unsigned int height,
					   bool fullScreen, const NameValuePairList *miscParams)
{
	int colourDepth = 32;
	Menge::String title = name;
	if(miscParams)
	{
		// Parse miscellenous parameters
		NameValuePairList::const_iterator opt;
		// Bit depth
	/*	opt = miscParams->find("colourDepth");

		if(opt != miscParams->end()) 
		{
			colourDepth = StringConverter::parseUnsignedInt(opt->second);
		}
		
		opt = miscParams->find("FSAA");

		if(opt != miscParams->end())
		{
			size_t fsaa_x_samples = StringConverter::parseUnsignedInt(opt->second);

			if(fsaa_x_samples>1)
			{
				SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,1);
				SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,fsaa_x_samples);
			}
		}
*/
		opt = miscParams->find("title");
		if(opt != miscParams->end())
		{
			title = opt->second;
		}
	}   

	SDL_Surface * screen;

	int flags = SDL_OPENGL | SDL_HWPALETTE | SDL_RESIZABLE;

	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	
	if (colourDepth == 32)
	{
		SDL_GL_SetAttribute( SDL_GL_STENCIL_SIZE, 8);
	}

	if (fullScreen)
	{
		flags |= SDL_FULLSCREEN;
	}

	screen = SDL_SetVideoMode(width, height, colourDepth, flags);

	if (!screen)
	{
		exit(1);
	}
	
	mScreen = screen;

/*	mName = name;

	mWidth = width;
	mHeight = height;
*/
	mActive = true;

	if (!fullScreen)
	{
		SDL_WM_SetCaption(title.c_str(), 0);
	}

	glXGetVideoSyncSGI = (int (*)(unsigned int *))SDL_GL_GetProcAddress("glXGetVideoSyncSGI");
	glXWaitVideoSyncSGI = (int (*)(int, int, unsigned int *))SDL_GL_GetProcAddress("glXWaitVideoSyncSGI");

	GLenum err = glewInit();

	if (GLEW_OK != err)	
	{

	}
}
//////////////////////////////////////////////////////////////////////////
void SDLWindow::destroy(void)
{
	// according to http://www.libsdl.org/cgi/docwiki.cgi/SDL_5fSetVideoMode
	// never free the surface returned from SDL_SetVideoMode
	//SDL_FreeSurface(mScreen);
	mScreen = NULL;
	mActive = false;
	//Root::getSingleton().getRenderSystem()->detachRenderTarget( this->getName() );
}
//////////////////////////////////////////////////////////////////////////
bool SDLWindow::isActive() const
{
	return mActive;
}
//////////////////////////////////////////////////////////////////////////
bool SDLWindow::isClosed() const
{
	return mClosed;
}
//////////////////////////////////////////////////////////////////////////
void SDLWindow::reposition(int left, int top)
{
	// XXX FIXME
}
//////////////////////////////////////////////////////////////////////////
void SDLWindow::resize(unsigned int width, unsigned int height)
{
	SDL_Surface* screen;
	int flags = SDL_OPENGL | SDL_HWPALETTE | SDL_RESIZABLE;

	screen = SDL_SetVideoMode(width, height, mScreen->format->BitsPerPixel, flags);
	if (!screen)
	{
		exit(1);
	}
	
	mScreen = screen;

	//	mWidth = width;
//	mHeight = height;

/*	for (ViewportList::iterator it = mViewportList.begin();
		it != mViewportList.end(); ++it)
	{
		(*it).second->_updateDimensions();
	}*/
}
//////////////////////////////////////////////////////////////////////////
void SDLWindow::swapBuffers(bool waitForVSync)
{
	if ( waitForVSync && glXGetVideoSyncSGI && glXWaitVideoSyncSGI )
	{
		unsigned int retraceCount;
		glXGetVideoSyncSGI( &retraceCount );
		glXWaitVideoSyncSGI( 2, ( retraceCount + 1 ) & 1, &retraceCount);
	}

	SDL_GL_SwapBuffers();
	// XXX More?
}