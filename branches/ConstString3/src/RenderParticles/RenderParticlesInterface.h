#	pragma once
#ifndef MENGINE_CONFIG
	#define MENGINE_CONFIG Config_WIN32.h
#endif

#	include "Config/Typedef.h"
#	include "Utils/Core/String.h"
#	include "Utils/Core/Rect.h"
#	include "WinIncluder.h"

namespace Menge
{
	class EmitterDescriptionInterface
	{
	public:
		virtual float getDuration() const = 0;
		virtual float getLeftBorder() const = 0;
		virtual float getRightBorder() const = 0;
		virtual int getEmitterId() const = 0;
		virtual const String& getName() const = 0;
		virtual const Rect& getSize() const = 0;
	};
	typedef std::vector<EmitterDescriptionInterface *> TVectorEmitterDescriptionInterface;

	struct ParticlesAtlas
	{
		ParticlesAtlas(){}
		ParticlesAtlas( const String & _file, const String & _path )
			: file(_file)
			, path(_path)
		{}
		String file;
		String path;
	};

	class RenderParticlesListener
	{
	public:
		virtual void onLoadEmitter( EmitterDescriptionInterface * _descr ) = 0;
		virtual void onLoadAtlas( ParticlesAtlas * _atlas ) = 0;
	};
	
	struct RenderParticlesFrame
	{
		uint32 width;
		uint32 height;
		uint32 pitch;
		uint32 size;
		double time;
		unsigned char * data;
	};
		
	class RenderParticlesInterface
	{	
	public:
		virtual bool initialise( HINSTANCE _hinstance, size_t _windowWidth, size_t _windowHeight ) = 0;
		virtual void destroy() = 0;
		virtual void addListener( RenderParticlesListener * _listener ) = 0;
		virtual void removeListener( RenderParticlesListener * _listener ) = 0;
		virtual void loadAstralaxFile( const String& _path ) = 0;
		virtual void createEmitterFromFile( const String& _path, const String & _emitterName ) = 0;
		virtual void loadFrame( RenderParticlesFrame * _frame, const TVectorEmitterDescriptionInterface& _emittersToUpdate  ) = 0;
		virtual void log( const String & _message ) = 0;
	};
}

bool initRenderParticles( Menge::RenderParticlesInterface** );
void releaseRenderParticles( Menge::RenderParticlesInterface* );
