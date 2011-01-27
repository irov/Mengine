#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec2.h"

#	include <vector>

namespace Menge
{

	template<class T>
	struct MovieKeyFrames
	{
		struct Key
		{
			T value;
			float time;
			int interp;
		};

		typedef std::vector<Key> TVectorKey;
		TVectorKey keys;

		T value;
	};

	struct MovieLayer
	{
		size_t index;

		float in;
		float out;

		String path;

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

	protected:
		void loader( BinParser * _parser ) override;
		void loaderMovieLayer_( BinParser * _parser, MovieLayer & _ml );

	protected:
		bool _compile() override;
		void _release() override;

	protected:
		bool readMOV_( const ConstString & _path );

	protected:
		ConstString m_pathMOV;

		String m_name;

		float m_duration;
		float m_width;
		float m_height;

		typedef std::vector<MovieLayer> TVectorMovieLayers;
		TVectorMovieLayers m_layers;
	};
}