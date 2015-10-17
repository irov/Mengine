#	pragma once

#	include "AEIO.h"

#	include "Emitter.h"
#	include "Atlas.h"

#	include "magic.h"

#	include <vector>
#	include <map>

namespace Menge
{
	class AstralaxRender
		: public Render
	{
	public:
		AstralaxRender();

	public:
		bool initialize() override;
		void finalize() override;
		
	public:
		Emitter * createEmitter( const std::wstring & _ptc );
		bool destroyEmitter( Emitter * _emitter );

	public:
		bool renderEmitter( Emitter * _emitter, const ParticleEmitterRenderFlush & _flush );

	public:
		bool updateEmittersAtlas();

	protected:
		void releaseResource_() override;
		bool resetDevice_() override;

	protected:		
		bool setAtlas_( int _stage, int _texture );
		Atlas * getAtlas_( int _id );

		Emitter * createEmitter_( HM_FILE _mf, std::auto_ptr<char> _buffer );
		HM_EMITTER createEmitterId_( HM_FILE _mf );

	protected:
		typedef std::vector<Emitter *> TVectorEmitters;
		TVectorEmitters m_emitters;
		
		typedef std::vector<Atlas *> TVectorAtlas;
		TVectorAtlas m_atlas;
	};
}