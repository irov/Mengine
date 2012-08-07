#	pragma once

#	include "ResourceReference.h"

#	include "Vertex.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"
#	include "Math/mat4.h"

#	include <vector>
#	include <map>
#	include "MovieFramePack.h"

namespace Menge
{
	class MovieFramePack;
	
	struct MovieLayer
	{
		MovieLayer()
			: index(-1)
			, parent(0)
			, in(0.f)
			, out(0.f)
			, startInterval(0.f)
			, animatable(false)
		{
		}

		ConstString name;

		size_t index;
		size_t parent;

		float in;
		float out;
		float startInterval;

		ConstString blendingMode;
		ConstString keyFramesName;
		WString keyFramesPath;

		ConstString source;
		bool animatable;
		bool movie;
		bool threeD;
	};

	typedef std::vector<MovieLayer> TVectorMovieLayers;

	struct MovieLayerCamera3D
	{
		float width;
		float height;

		mt::vec3f cameraPosition;
		mt::vec3f cameraInterest;
		float cameraFOV;
		float cameraAspect;
	};

	struct MovieLayer3D
		: public MovieLayer
	{
		MovieLayerCamera3D camera;
	};

	struct MovieFootage
	{
		ConstString path;
	};

	struct MovieInternal
	{
		ConstString group;
	};

	class VisitorResourceMovie
	{
	public:
		virtual void visitLayer( const MovieLayer & _layer, const MovieLayerFrame & _frames ) = 0;
	};

	class ResourceMovie
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceMovie )

	public:
		ResourceMovie();
		~ResourceMovie();

	public:
		float getFrameDuration() const;
		float getDuration() const;

		size_t getFrameCount() const;
		size_t getMaxLayerIndex() const;

	public:
		bool getMovieInternal( const ConstString & _source, MovieInternal & _internal ) const;

	public:
		const TVectorMovieLayers & getLayers() const;
		const MovieLayerCamera3D & getCamera3D() const;

	public:
		bool hasCamera3D() const;

	public:
		bool getFrame( const MovieLayer & _layer, size_t _index, MovieFrameSource & _frame ) const;
		
		bool getFrameFirst( const MovieLayer & _layer, MovieFrameSource & _frame ) const;
		bool getFrameLast( const MovieLayer & _layer, MovieFrameSource & _frame ) const;

	public:
		void visitResourceMovie( VisitorResourceMovie * _visitor );

	protected:
		void loader( const Metabuf::Metadata * _parser ) override;
		//void loaderMovieLayer2D_( BinParser * _parser, MovieLayer2D & _ml );
		//void loaderMovieLayer3D_( BinParser * _parser, MovieLayer3D & _ml, const MovieLayerSource3D & _layer );
		
	protected:
		bool _compile() override;
		void _release() override;

	protected:
		//void compileModel_( MovieLayer3D & _layer );
		//void convertSourceToFrame3D_( MovieFrame3D & _frame, const MovieLayerSource3D & _layer, const MovieFrameSource3D & _source );

	protected:
		ConstString m_pathMOV;

		WString m_keyFramePackPath;
		float m_frameDuration;
		float m_duration;
		
		MovieFramePack * m_framePack;

		size_t m_frameCount;

		float m_width;
		float m_height;

		TVectorMovieLayers m_layers;

		bool m_hasCamera3D;
		MovieLayerCamera3D m_camera3D;

		size_t m_maxLayerIndex;

		typedef std::map<ConstString, MovieInternal> TMapInternals;
		TMapInternals m_internals;
	};
}