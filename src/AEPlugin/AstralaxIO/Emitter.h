#	pragma once

#	include "Render.h"

#	include "Math/mat4.h"

#	include "magic.h"

#	include <vector>
#	include <memory>

#	include <stdint.h>

namespace Menge
{
	struct ParticleMesh
	{		
		uint32_t vertexCount;
		uint32_t indexBegin;
		uint32_t primCount;

		int texture[8];
		int material;
	};	

	typedef std::vector<ParticleMesh> TVectorParticleMesh;
	typedef std::vector<Vertex> TVectorParticleVertices;
	typedef std::vector<Indices> TVectorParticleIndices;

	struct ParticleEmitterRenderFlush
	{
		TVectorParticleMesh meshes;		
		TVectorParticleVertices vertices;
		TVectorParticleIndices indices;
	};

	class Emitter
	{
	public:
		Emitter();
		~Emitter();

	public:
		bool initialize( HM_FILE _mf, std::auto_ptr<char> _buffer, HM_EMITTER _emitterId );
		void finalize();

	public:
		const char * getName() const;
		HM_EMITTER getId() const;
		

	public:
		double getTime() const;
		double getDuration() const;

	public:
		bool getDimension( int & _w, int & _h ) const;

	public:
		bool update( double _time );
		bool flushEmitter( ParticleEmitterRenderFlush & _flush );

	public:
		bool getTransform( mt::mat4f & _projection, mt::mat4f & _view );
		
	protected:
		HM_FILE m_mf;
		std::auto_ptr<char> m_buffer;
		HM_EMITTER m_emitterId;		
		
		double m_interval1;

		int m_width;
		int m_height;
	};
}