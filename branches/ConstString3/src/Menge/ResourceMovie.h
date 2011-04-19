#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec2.h"
#	include "Math/vec3.h"

#	include <vector>
#	include <map>

namespace Menge
{
	struct MovieLayer
	{
		MovieLayer()
			: index(-1)
			, parent(-1)
			, in(0.f)
			, out(0.f)
			, internal(false)
		{
		}

		ConstString name;

		size_t index;
		size_t parent;

		float in;
		float out;

		ConstString source;
		bool internal;
	};

	struct MovieFrame2D
	{
		mt::vec2f anchorPoint;
		mt::vec2f position;
		mt::vec2f scale;
		float angle;
		float opacity;
	};

	struct MovieLayer2D
		: public MovieLayer
	{
		typedef std::vector<MovieFrame2D> TVectorFrames;
		TVectorFrames frames;
	};

	struct MovieFrame3D
	{
		mt::vec3f anchorPoint;
		mt::vec3f position;
		mt::vec2f scale;
		mt::vec3f rotation;
		float opacity;
	};

	struct MovieLayer3D
		: public MovieLayer
	{
		mt::vec3f cameraPosition;
		mt::vec3f cameraInterest;
		float cameraFOV;

		float width;
		float height;

		typedef std::vector<MovieFrame3D> TVectorFrames;
		TVectorFrames frames;
	};

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
		std::size_t getLayerSize() const;
		const MovieLayer2D & getLayer( std::size_t _index ) const;

		bool getFrame( const MovieLayer2D & _layer, float _timing, MovieFrame2D & _frame ) const;
		
		bool getFrameFirst( const MovieLayer2D & _layer, MovieFrame2D & _frame ) const;
		bool getFrameLast( const MovieLayer2D & _layer, MovieFrame2D & _frame ) const;

		bool getMovieInternal( const ConstString & _source, MovieInternal & _internal ) const;

	protected:
		void loader( BinParser * _parser ) override;
		void loaderMovieLayer2D_( BinParser * _parser, MovieLayer2D & _ml );
		void loaderMovieLayer3D_( BinParser * _parser, MovieLayer3D & _ml );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		void compileModel_( MovieLayer3D & _layer );

	protected:
		ConstString m_pathMOV;

		float m_duration;
		float m_width;
		float m_height;

		typedef std::vector<MovieLayer2D> TVectorMovieLayers2D;
		TVectorMovieLayers2D m_layers2D;

		typedef std::vector<MovieLayer3D> TVectorMovieLayers3D;
		TVectorMovieLayers3D m_layers3D;
		

		typedef std::map<ConstString, MovieFootage> TMapImagePaths;
		TMapImagePaths m_imagePaths;

		typedef std::map<ConstString, MovieInternal> TMapInternals;
		TMapInternals m_internals;
	};
}