#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec2.h"

#	include <vector>
#	include <map>

namespace Menge
{
	struct MovieFrame
	{
		mt::vec2f anchorPoint;
		mt::vec2f position;
		mt::vec2f scale;
		float angle;
		float opacity;
	};

	struct MovieLayer
	{
		MovieLayer()
			: index(-1)
			, in(0.f)
			, out(0.f)
			, internal(false)
		{
		}

		ConstString name;
		size_t parent;
		size_t index;

		float in;
		float out;

		ConstString source;
		bool internal;

		typedef std::vector<MovieFrame> TVectorFrames;
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
		const MovieLayer & getLayer( std::size_t _index ) const;

		bool getFrame( const MovieLayer & _layer, float _timing, MovieFrame & _frame ) const;
		
		bool getFrameFirst( const MovieLayer & _layer, MovieFrame & _frame ) const;
		bool getFrameLast( const MovieLayer & _layer, MovieFrame & _frame ) const;

		bool getMovieInternal( const ConstString & _source, MovieInternal & _internal ) const;

	protected:
		void loader( BinParser * _parser ) override;
		void loaderMovieLayer_( BinParser * _parser, MovieLayer & _ml );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		ConstString m_pathMOV;

		float m_duration;
		float m_width;
		float m_height;

		typedef std::vector<MovieLayer> TVectorMovieLayers;
		TVectorMovieLayers m_layers;

		typedef std::map<ConstString, MovieFootage> TMapImagePaths;
		TMapImagePaths m_imagePaths;

		typedef std::map<ConstString, MovieInternal> TMapInternals;
		TMapInternals m_internals;
	};
}