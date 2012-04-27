#	pragma once
#	include <map>
#	include <vector>
#	include "Config/Typedef.h"
#	include <Core/Holder.h>
#	include "Math/vec2.h"
#	include "Math/vec3.h"
#	include "Utils/Core/ConstString.h"

namespace Menge
{
	class BinParser;

	struct MovieFrameSource
	{
		mt::vec3f anchorPoint;
		mt::vec3f position;
		mt::vec3f rotation;
		mt::vec2f scale;
		float opacity;
	};
	
	typedef std::vector<MovieFrameSource> TVectorMovieFrameSource;

	struct MovieFrameLayer
	{
		TVectorMovieFrameSource frames;
	};

	typedef std::vector<MovieFrameLayer> TVectorMovieFrameLayer;

	class MovieFramePack
	{
	public:
		void initialise( size_t _size )
		{
			m_size = _size;
			m_layers.resize(m_size);
		}

		const MovieFrameSource & getLastLayerFrame( size_t _layerIndex )
		{
			return m_layers[_layerIndex].frames.back();
		}

		bool isLayerEmpty( size_t _layerIndex )
		{
			bool isEmpty = m_layers[_layerIndex].frames.empty();
			return isEmpty;
		}

		void setLayerFrame( size_t _layerIndex, const MovieFrameSource & _frame )
		{
			m_layers[_layerIndex].frames.push_back( _frame );
		}

		const MovieFrameSource & getLayerFrame( size_t _layerIndex, size_t _frameIndex )
		{
			return m_layers[_layerIndex].frames[_frameIndex];
		}

		size_t getLayerCountFrames( size_t _layerIndex )
		{
			return m_layers[_layerIndex].frames.size();
		}

		TVectorMovieFrameLayer m_layers;
	protected:
		size_t m_size;
		
	};

	typedef std::vector<MovieFramePack *> TVectorMovieFramePack;
	
	class MovieKeyFrameManager
		: public Holder<MovieKeyFrameManager>
	{
	public:
		MovieFramePack * getMovieFramePak( const ConstString & _pak, const WString & _path );
		void releaseMovieFramePak( MovieFramePack * _framePak );

		void loaderMovieFramePak( BinParser * _parser, MovieFramePack * _framePak );
	protected:
		MovieFramePack * createMovieFramePak_( const ConstString & _pak, const WString & _path );
		void loaderKeyFrames2D_( BinParser * _parser, size_t _layerIndex, MovieFramePack * _framePak );
		void loaderKeyFrames3D_( BinParser * _parser, size_t _layerIndex, MovieFramePack * _framePak );
		TVectorMovieFramePack m_framePacks;
		size_t m_curLayer;
	};
}