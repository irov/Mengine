#	include "MovieFramePack.h"

#	include "Core/Memory.h"
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

			Helper::freeMemory( layer.anchorPoint );
			Helper::freeMemory( layer.position );
			Helper::freeMemory( layer.rotation_x );
			Helper::freeMemory( layer.rotation_y );
			Helper::freeMemory( layer.rotation_z );
			Helper::freeMemory( layer.scale );
			Helper::freeMemory( layer.opacity );
			Helper::freeMemory( layer.volume );
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
		MovieLayerFrame & layer = m_layers[_layerIndex];

		layer.count = _count;
		layer.immutable = _immutable;

		layer.anchorPoint = nullptr;
		layer.position = nullptr;
		layer.rotation_x = nullptr;
		layer.rotation_y = nullptr;
		layer.rotation_z = nullptr;
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
	MovieLayerTimeRemap & MovieFramePack::mutableLayerTimeRemap( size_t _layerIndex )
	{
		MovieLayerTimeRemap & timeremap = m_timeremap[_layerIndex];

		return timeremap;
	}
	//////////////////////////////////////////////////////////////////////////
	MovieLayerShapes & MovieFramePack::mutableLayerShape( size_t _layerIndex )
	{
		MovieLayerShapes & shapes = m_shapes[_layerIndex];

		return shapes;
	}
	//////////////////////////////////////////////////////////////////////////
	MovieLayerPolygon & MovieFramePack::mutableLayerPolygon( size_t _layerIndex )
	{
		MovieLayerPolygon & polygon = m_polygons[_layerIndex];

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
		
#	define MOVIE_FRAME_SETUP( Member1, Member2, Mask )\
		if( layer.immutable_mask & Mask ) \
		{ \
			_frame.Member1 = layer.source.Member1; \
		} \
		else \
		{ \
			_frame.Member1 = layer.Member2[_frameIndex]; \
		}
		
		MOVIE_FRAME_SETUP( anchorPoint, anchorPoint, MOVIE_KEY_FRAME_IMMUTABLE_ANCHOR_POINT );
		MOVIE_FRAME_SETUP( position, position, MOVIE_KEY_FRAME_IMMUTABLE_POSITION );
		MOVIE_FRAME_SETUP( rotation.x, rotation_x, MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_X );
		MOVIE_FRAME_SETUP( rotation.y, rotation_y, MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_Y );
		MOVIE_FRAME_SETUP( rotation.z, rotation_z, MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_Z );
		MOVIE_FRAME_SETUP( scale, scale, MOVIE_KEY_FRAME_IMMUTABLE_SCALE );
		MOVIE_FRAME_SETUP( opacity, opacity, MOVIE_KEY_FRAME_IMMUTABLE_OPACITY );
		MOVIE_FRAME_SETUP( volume, volume, MOVIE_KEY_FRAME_IMMUTABLE_VOLUME );
		
#	undef MOVIE_FRAME_SETUP
				
		return true;
	}
	////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		//////////////////////////////////////////////////////////////////////////
		static void s_linerp_f1( float & _out, float _in1, float _in2, float _scale )
		{
			_out = _in1 + ( _in2 - _in1 ) * _scale;
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_linerp_f3( mt::vec3f & _out, const mt::vec3f & _in1, const mt::vec3f & _in2, float _scale )
		{
			s_linerp_f1(_out.x, _in1.x, _in2.x, _scale);
			s_linerp_f1(_out.y, _in1.y, _in2.y, _scale);
			s_linerp_f1(_out.z, _in1.z, _in2.z, _scale);
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
			Helper::s_linerp_f1( _frame.Member, value0, value1, _t ); \
		}

		MOVIE_FRAME_SETUP_F1( opacity, MOVIE_KEY_FRAME_IMMUTABLE_OPACITY );
		MOVIE_FRAME_SETUP_F1( volume, MOVIE_KEY_FRAME_IMMUTABLE_VOLUME );

		if( layer.immutable_mask & MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_X )
		{
			_frame.rotation.x = layer.source.rotation.x;
		}
		else
		{
			float value0 = layer.rotation_x[_frameIndex + 0];
			float value1 = layer.rotation_x[_frameIndex + 1];
		
			float correct_rotate_from;
			float correct_rotate_to;
			mt::angle_correct_interpolate_from_to( value0, value1, correct_rotate_from, correct_rotate_to );

			Helper::s_linerp_f1( _frame.rotation.x, correct_rotate_from, correct_rotate_to, _t );
		}

		if( layer.immutable_mask & MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_Y )
		{
			_frame.rotation.y = layer.source.rotation.y;
		}
		else
		{
			float value0 = layer.rotation_y[_frameIndex + 0];
			float value1 = layer.rotation_y[_frameIndex + 1];

			float correct_rotate_from;
			float correct_rotate_to;
			mt::angle_correct_interpolate_from_to( value0, value1, correct_rotate_from, correct_rotate_to );

			Helper::s_linerp_f1( _frame.rotation.y, correct_rotate_from, correct_rotate_to, _t );
		}

		if( layer.immutable_mask & MOVIE_KEY_FRAME_IMMUTABLE_ROTATION_Z )
		{
			_frame.rotation.z = layer.source.rotation.z;
		}
		else
		{
			float value0 = layer.rotation_z[_frameIndex + 0];
			float value1 = layer.rotation_z[_frameIndex + 1];

			float correct_rotate_from;
			float correct_rotate_to;
			mt::angle_correct_interpolate_from_to( value0, value1, correct_rotate_from, correct_rotate_to );

			Helper::s_linerp_f1( _frame.rotation.z, correct_rotate_from, correct_rotate_to, _t );
		}

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool MovieFramePack::getLayerTimeRemap( size_t _layerIndex, size_t _frameIndex, float & _time ) const
    {
		for( TVectorMovieLayerTimeRemap::const_iterator
			it = m_timeremap.begin(),
			it_end = m_timeremap.end();
		it != it_end;
		++it )
		{
			const MovieLayerTimeRemap & timeremap = *it;

			if( timeremap.layerId != _layerIndex )
			{
				continue;
			}

			float time = timeremap.times[_frameIndex];

			_time = time;

			return true;
		}

		return false;
    }
	//////////////////////////////////////////////////////////////////////////
	bool MovieFramePack::getLayerShape( size_t _layerIndex, size_t _frameIndex, const MovieFrameShape ** _shape ) const
	{
		for( TVectorMovieLayerShapes::const_iterator
			it = m_shapes.begin(),
			it_end = m_shapes.end();
		it != it_end;
		++it )
		{
			const MovieLayerShapes & shapes = *it;

			if( shapes.layerId != _layerIndex )
			{
				continue;
			}
			
			const MovieFrameShape & shape = shapes.shapes[_frameIndex];

			*_shape = &shape;

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieFramePack::getLayerPolygon( size_t _layerIndex, const mt::vec2f ** _polygon, uint8_t & _vertexCount ) const
	{
		for( TVectorMovieLayerPolygons::const_iterator
			it = m_polygons.begin(),
			it_end = m_polygons.end();
		it != it_end;
		++it )
		{
			const MovieLayerPolygon & polygon = *it;

			if( polygon.layerId != _layerIndex )
			{
				continue;
			}

			*_polygon = polygon.polygon;
			_vertexCount = (size_t)polygon.vertexCount;

			return true;
		}

		return false;
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