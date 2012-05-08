#	pragma once

#	include "ResourceReference.h"

#	include "Vertex.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"
#	include "Math/mat4.h"

#	include <vector>
#	include <map>

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
			, internal(false)
			, animatable(false)
		{
		}

		ConstString name;

		size_t index;
		size_t parent;

		float in;
		float out;

		ConstString blendingMode;
		ConstString keyFramesName;
		WString keyFramesPath;

		ConstString source;
		bool internal;
		bool animatable;
		bool movie;
	};

	struct MovieFrame2D
	{
		mt::vec3f anchorPoint;
		mt::vec3f position;
		mt::vec3f scale;
		float angle;
		float opacity;
	};

	typedef std::vector<MovieFrame2D> TVectorFrames;

	struct MovieLayer2D
		: public MovieLayer
	{
		TVectorFrames frames;
	};

	typedef std::vector<MovieLayer2D> TVectorMovieLayers2D;

	struct MovieFrameSource3D
	{
		mt::vec3f anchorPoint;
		mt::vec3f position;
		mt::vec3f rotation;
		mt::vec2f scale;
		float opacity;
	};

	struct MovieLayerSource3D
	{
		mt::vec3f cameraPosition;
		mt::vec3f cameraInterest;
		float cameraFOV;
		float cameraAspect;

		float width;
		float height;

		mt::mat4f vp;
	};

	struct MovieFrame3D
	{
		Vertex2D vertices[4];
	};

	struct MovieLayer3D
		: public MovieLayer
	{
		typedef std::vector<MovieFrame3D> TVectorFrames;
		TVectorFrames frames;
	};

	typedef std::vector<MovieLayer3D> TVectorMovieLayers3D;

	struct MovieFootage
	{
		ConstString path;
	};

	struct MovieInternal
	{
		ConstString group;
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
		float getWorkAreaDuration() const;

		size_t getFrameCount() const;

	public:
		bool getMovieInternal( const ConstString & _source, MovieInternal & _internal ) const;

	public:
		const TVectorMovieLayers2D & getLayers2D() const;
		const TVectorMovieLayers3D & getLayers3D() const;

	public:
		bool getFrame2D( const MovieLayer2D & _layer, size_t _index, MovieFrame2D & _frame ) const;
		
		bool getFrame2DFirst( const MovieLayer2D & _layer, MovieFrame2D & _frame ) const;
		bool getFrame2DLast( const MovieLayer2D & _layer, MovieFrame2D & _frame ) const;

	public:

		bool getFrame3D( const MovieLayer3D & _layer, float _timing, MovieFrame3D & _frame ) const;

		bool getFrame3DFirst( const MovieLayer3D & _layer, MovieFrame3D & _frame ) const;
		bool getFrame3DLast( const MovieLayer3D & _layer, MovieFrame3D & _frame ) const;

	protected:
		void loader( BinParser * _parser ) override;
		//void loaderMovieLayer2D_( BinParser * _parser, MovieLayer2D & _ml );
		//void loaderMovieLayer3D_( BinParser * _parser, MovieLayer3D & _ml, const MovieLayerSource3D & _layer );
		
	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void compileModel_( MovieLayer3D & _layer );
		void convertSourceToFrame3D_( MovieFrame3D & _frame, const MovieLayerSource3D & _layer, const MovieFrameSource3D & _source );

	protected:
		ConstString m_pathMOV;

		WString m_keyFramePackPath;
		float m_frameDuration;
		float m_workAreaDuration;
		
		MovieFramePack * m_framePack;

		size_t m_frameCount;

		float m_width;
		float m_height;

		TVectorMovieLayers2D m_layers2D;
		TVectorMovieLayers3D m_layers3D;

		typedef std::map<ConstString, MovieInternal> TMapInternals;
		TMapInternals m_internals;
	};
}