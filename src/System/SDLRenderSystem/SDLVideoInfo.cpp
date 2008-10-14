#	include "SDLVideoInfo.h"

#	include <iostream>

SDLVideoInfo* SDLVideoInfo::_Instance=NULL;

SDLVideoInfo::SDLVideoInfo()
{
	_info = SDL_GetVideoInfo();
}

SDLVideoInfo * SDLVideoInfo::instance(void)
{
	if (_Instance == NULL)
	{
		_Instance=new SDLVideoInfo();
	}

	return _Instance;
}

std::string SDLVideoInfo::getDriverName(void) const
{
	char name[32]; 
	SDL_VideoDriverName(name,32);
	return std::string(name);
}

void SDLVideoInfo::debug(void) const
{
	std::cout << "\nSDLVideoInfo::debug()"  << "\n" <<
	"- Driver Name : " << getDriverName() << "\n" <<

	std::boolalpha << 
	"- Is it possible to create Hardware Surfaces? " << isHWAvailable() << "\n" <<
	"- Is there a window manager available? " << isWMAvailable() << "\n" <<
	"- Are hardware to hardware blits accelerated? " << isBlitHWAccelAvailable() << "\n" <<
	"- Are hardware to hardware colorkey blits accelerated? " << isBlitHWCCAccelAvailable() << "\n" <<
	"- Are hardware to hardware alpha blits accelerated? " << isBlitHWAAccelAvailable() << "\n" <<
	"- Are software to hardware blits accelerated? " << isBlitSWAccelAvailable() << "\n" <<
	"- Are software to hardware colorkey blits accelerated? " << isBlitSWCCAccelAvailable() << "\n" <<
	"- Are software to hardware alpha blits accelerated? " << isBlitSWAAccelAvailable() << "\n" <<
	"- Are color fills accelerated? " << isBlitFillAccelAvailable() << "\n" <<
	"- Total amount of video memory in Kilobytes : " << videoMemSize() << "\n" <<

	std::endl;
}
