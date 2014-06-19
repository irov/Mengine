#	include "MovieFramePack.h"

#	include "Math/angle.h"

namespace Menge
{
	///////////////////////////////////////////////////////////////////////
	MovieFramePack::MovieFramePack()
	{		
	}
	///////////////////////////////////////////////////////////////////////
	MovieFramePack::~MovieFramePack()
	{
		for( TVectorMovieFrameLayer::iterator
			it = m_layers.begin(),
			it_end = m_layers.end();
		it != it_end;
		++it )
		{
			MovieLayerFrame & layer = *it;

			delete [] layer.anchorPoint;
			delete [] layer.position;
			delete [] layer.rotation;
			delete [] layer.scale;
			delete [] layer.opacity;
			delete [] layer.volume;
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void MovieFramePack::initialize( size_t _size )
    {
        m_layers.resize( _size );
    }
	//////////////////////////////////////////////////////////////////////////
	void MovieFramePack::initializeTimeremap( size_t _size )
	{
		m_timeremap.resize( _size );
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieFramePack::initializeShapes( size_t _size )
	{
		m_shapes.resize( _size );
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieFramePack::initializePolygons( size_t _size )
	{
		m_polygons.resize( _size );
	}
	//////////////////////////////////////////////////////////////////////////
	MovieLayerFrame & MovieFramePack::initializeLayer( size_t _layerIndex, size_t _count, bool _immutable )
	{
		MovieLayerFrame & layer = m_layers[_layerIndex - 1];

		layer.count = _count;
		layer.immutable = _immutable;

		layer.anchorPoint = nullptr;
		layer.position = nullptr;
		layer.rotation = nullptr;
		layer.scale = nullptr;
		layer.opacity = nullptr;
		layer.volume = nullptr;
		
		layer.immutable_mask = 0;

		return layer;
	}
    //////////////////////////////////////////////////////////////////////////
    bool MovieFramePack::hasLayer( size_t _layerIndex ) const
    {
        if( (_layerIndex - 1) >= m_layers.size() )
        {
            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	const MovieLayerFrame & MovieFramePack::getLayer( size_t _layerIndex ) const
	{
		const MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];

		return frameLayer;
	}
    //////////////////////////////////////////////////////////////////////////
    void MovieFramePack::addLayerTimeRemap( size_t _layerIndex, const MovieLayerTimeRemap & _timeremap )
    {
        m_timeremap[_layerIndex - 1] = _timeremap;
    }
	//////////////////////////////////////////////////////////////////////////
	MovieLayerTimeRemap & MovieFramePack::mutableLayerTimeRemap( size_t _layerIndex )
	{
		MovieLayerTimeRemap & timeremap = m_timeremap[_layerIndex - 1];

		return timeremap;
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieFramePack::addLayerShape( size_t _layerIndex, const MovieLayerShapes & _shapes )
	{
		m_shapes[_layerIndex - 1] = _shapes;
	}
	//////////////////////////////////////////////////////////////////////////
	MovieLayerShapes & MovieFramePack::mutableLayerShape( size_t _layerIndex )
	{
		MovieLayerShapes & shapes = m_shapes[_layerIndex - 1];

		return shapes;
	}
	void MovieFramePack::addLayerPolygon( size_t _layerIndex, const MovieLayerPolygon & _polygon )
	{
		m_polygons[_layerIndex - 1] = _polygon;
	}
	//////////////////////////////////////////////////////////////////////////
	MovieLayerPolygon & MovieFramePack::mutableLayerPolygon( size_t _layerIndex )
	{
		MovieLayerPolygon & polygon = m_polygons[_layerIndex - 1];

		return polygon;
	}
	///////////////////////////////////////////////////////////////////////
	bool MovieFramePack::getLayerFrame( size_t _layerIndex, size_t _frameIndex, MovieFrameSource & _frame ) const
	{
        if( (_layerIndex - 1) >= m_layers.size() )
        {
            return false;
        }

		const MovieLayerFrame & layer = m_layers[_layerIndex - 1];

		if( _frameIndex >= layer.count )
		{
			return false;
		}

		if( layer.immutable == true )
		{
			_frame = layer.source;

			return true;
		}
		
#	define MOVIE_FRAME_SETUP( Member, Mask )\
		if( layer.immutable_mask & Mask ) \
		{ \
			_frame.Member = layer.source.Member; \
		} \
		else \
		{ \
			_frame.Member = layer.Member[_frameIndex]; \
		}
		
		MOVIE_FRAME_SETUP( anchorPoint, MOVIE_KEY_FRAME_IMMUTABLE_ANCHOR_POINT );
		MOVIE_FRAME_SETUP( position, MOVIE_KEY_FRAME_IMMUTABLE_POSITION );
		MOVIE_FRAME_SETUP( rotation, MOVIE_KEY_FRAME_IMMUTABLE_ROTATION );
		MOVIE_FRAME_SETUP( scale, MOVIE_KEY_FRAME_IMMUTABLE_SCALE );
		MOVIE_FRAME_SETUP( opacity, MOVIE_KEY_FRAME_IMMUTABLE_OPACITY );
		MOVIE_FRAME_SETUP( volume, MOVIE_KEY_FRAME_IMMUTABLE_VOLUME );
		
#	undef MOVIE_FRAME_SETUP
				
		return true;
	}
	////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		static void s_linerp( float & _out, float _in1, float _in2, float _scale )
		{
			_out = _in1 + ( _in2 - _in1 ) * _scale;
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_linerp_f3( mt::vec3f & _out, const mt::vec3f & _in1, const mt::vec3f & _in2, float _scale )
		{
			s_linerp(_out.x, _in1.x, _in2.x, _scale);
			s_linerp(_out.y, _in1.y, _in2.y, _scale);
			s_linerp(_out.z, _in1.z, _in2.z, _scale);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieFramePack::getLayerFrameInterpolate( size_t _layerIndex, size_t _frameIndex, float _t, MovieFrameSource & _frame ) const
	{
		if( (_layerIndex - 1) >= m_layers.size() )
		{
			return false;
		}

		const MovieLayerFrame & layer = m_layers[_layerIndex - 1];

		if( _frameIndex + 1 >= layer.count )
		{
			return false;
		}

		if( layer.immutable == true )
		{
			_frame = layer.source;

			return true;
		}

#	define MOVIE_FRAME_SETUP_F3( Member, Mask )\
		if( layer.immutable_mask & Mask ) \
		{ \
			_frame.Member = layer.source.Member; \
		} \
		else \
		{ \
			const mt::vec3f & value0 = layer.Member[ _frameIndex + 0 ]; \
			const mt::vec3f & value1 = layer.Member[ _frameIndex + 1 ]; \
			\
			Helper::s_linerp_f3( _frame.Member, value0, value1, _t ); \
		}

		MOVIE_FRAME_SETUP_F3( anchorPoint, MOVIE_KEY_FRAME_IMMUTABLE_ANCHOR_POINT );
		MOVIE_FRAME_SETUP_F3( position, MOVIE_KEY_FRAME_IMMUTABLE_POSITION );
		//MOVIE_FRAME_SETUP( rotation, MOVIE_KEY_FRAME_IMMUTABLE_ROTATION );
		MOVIE_FRAME_SETUP_F3( scale, MOVIE_KEY_FRAME_IMMUTABLE_SCALE );

#	undef MOVIE_FRAME_SETUP_F3

#	define MOVIE_FRAME_SETUP_F1( Member, Mask )\
		if( layer.immutable_mask & Mask ) \
		{ \
			_frame.Member = layer.source.Member; \
		} \
		else \
		{ \
			float value0 = layer.Member[ _frameIndex + 0 ]; \
			float value1 = layer.Member[ _frameIndex + 1 ]; \
			\
			Helper::s_linerp( _frame.Member, value0, value1, _t ); \
		}

		MOVIE_FRAME_SETUP_F1( opacity, MOVIE_KEY_FRAME_IMMUTABLE_OPACITY );
		MOVIE_FRAME_SETUP_F1( volume, MOVIE_KEY_FRAME_IMMUTABLE_VOLUME );

		if( layer.immutable_mask & MOVIE_KEY_FRAME_IMMUTABLE_ROTATION )
		{
			_frame.rotation = layer.source.rotation;
		}
		else
		{
			const mt::vec3f & value0 = layer.rotation[ _frameIndex + 0 ];
			const mt::vec3f & value1 = layer.rotation[ _frameIndex + 1 ];
		
			mt::vec3f correct_rotate_from;
			mt::vec3f correct_rotate_to;
			mt::angle_correct_interpolate_from_to( value0.x, value1.x, correct_rotate_from.x, correct_rotate_to.x );
			mt::angle_correct_interpolate_from_to( value0.y, value1.y, correct_rotate_from.y, correct_rotate_to.y );
			mt::angle_correct_interpolate_from_to( value0.z, value1.z, correct_rotate_from.z, correct_rotate_to.z );

			Helper::s_linerp_f3( _frame.rotation, correct_rotate_from, correct_rotate_to, _t );
		}

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool MovieFramePack::getLayerTimeRemap( size_t _layerIndex, size_t _frameIndex, float & _time ) const
    {
        const MovieLayerTimeRemap & timeremap = m_timeremap[_layerIndex - 1];

        float time = timeremap.times[_frameIndex];

        _time = time;

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MovieFramePack::getLayerShape( size_t _layerIndex, size_t _frameIndex, const MovieFrameShape ** _shape ) const
	{
		const MovieLayerShapes & shapes = m_shapes[_layerIndex - 1];

		const MovieFrameShape & shape = shapes.shapes[_frameIndex];

		*_shape = &shape;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieFramePack::getLayerPolygon( size_t _layerIndex, const Polygon ** _polygon ) const
	{
		const MovieLayerPolygon & polygon = m_polygons[_layerIndex - 1];

		*_polygon = &polygon.polygon;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieFramePack::isLayerPermanentlyHide( size_t _layerIndex ) const
	{
		if( (_layerIndex - 1) >= m_layers.size() )
		{
			return false;
		}

		const MovieLayerFrame & layer = m_layers[_layerIndex - 1];
		
		if( layer.immutable == true || layer.immutable_mask & MOVIE_KEY_FRAME_IMMUTABLE_OPACITY )
		{
			bool hide = (layer.source.opacity == 0.f);

			return hide;
		}

		return false;
	}
}