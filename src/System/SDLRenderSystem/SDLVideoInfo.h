#	pragma once

#	include <SDL.h>
#	include <string>

class SDLVideoInfo
{
private:
	const SDL_VideoInfo * _info;
	static SDLVideoInfo* _Instance;
 	
	SDLVideoInfo();

public:
	static SDLVideoInfo* instance();
			 
	//return true if hardware acceleration is enabled
	inline bool isHWAvailable() const { return _info->hw_available; }
			 
	//return true if a window manager is available
	inline bool isWMAvailable() const { return _info->wm_available; }
			 
	//return true if hardware to hardware blits are accelerated
	inline bool isBlitHWAccelAvailable() const { return _info->blit_hw; }
			 
	//return true if hardware to hardware colorkey blits are accelerated
	inline bool isBlitHWCCAccelAvailable() const { return _info->blit_sw_CC; }
			 
	//return true if hardware to hardware alpha blits are accelerated
	inline bool isBlitHWAAccelAvailable() const { return _info->blit_sw_A; }
							 			 
	//return true if software to hardware blits are accelerated
	inline bool isBlitSWAccelAvailable() const { return _info->blit_sw; }
			 
	//return true if software to hardware colorkey blits are accelerated
	inline bool isBlitSWCCAccelAvailable() const { return _info->blit_sw_CC; }
			 
	//return true if software to hardware alpha blits are accelerated
	inline bool isBlitSWAAccelAvailable() const { return _info->blit_sw_A; }
			 
	//return true if color fills are accelerated
	inline bool isBlitFillAccelAvailable() const { return _info->blit_fill; }
			 
	//return the total amount of video memory in kilobytes
	inline unsigned long videoMemSize() const { return _info->video_mem; }
	
	std::string getDriverName(void) const;
	
	//display all detected informations about graphics
	void debug(void) const;
};