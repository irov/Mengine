#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec2.h"

#	include <vector>
#	include <map>

namespace Menge
{
	struct MovieLayer
	{
		ConstString name;
		size_t index;

		float in;
		float out;

		ConstString source;

		struct Frame
		{
			mt::vec2f anchorPoint;
			mt::vec2f position;
			mt::vec2f scale;
			float angle;
			float opacity;
		};

		typedef std::vector<Frame> TVectorFrames;
		TVectorFrames frames;
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

		bool getFrame( const MovieLayer & _layer, float _timing, MovieLayer::Frame & _frame ) const;
		
		bool getFrameFirst( const MovieLayer & _layer, MovieLayer::Frame & _frame ) const;
		bool getFrameLast( const MovieLayer & _layer, MovieLayer::Frame & _frame ) const;

	protected:
		void loader( BinParser * _parser ) override;
		void loaderMovieLayer_( BinParser * _parser, MovieLayer & _ml );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		ConstString m_pathMOV;

		String m_name;

		float m_duration;
		float m_width;
		float m_height;

		typedef std::vector<MovieLayer> TVectorMovieLayers;
		TVectorMovieLayers m_layers;

		struct Footage
		{
			ConstString path;
		};

		typedef std::map<ConstString, Footage> TMapImagePaths;
		TMapImagePaths m_imagePaths;
	};
}