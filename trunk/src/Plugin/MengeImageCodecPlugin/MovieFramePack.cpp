#	include "MovieFramePack.h"

#	include "Core/Memory.h"
#	include "Math/angle.h"

namespace Menge
{
	///////////////////////////////////////////////////////////////////////
	MovieFramePack::MovieFramePack()
		: m_sizeLayers(0)
		, m_sizeTimeremap(0)
		, m_sizeShapes(0)
		, m_sizePolygons(0)
	{		
	}
	///////////////////////////////////////////////////////////////////////
	MovieFramePack::~MovieFramePack()
	{
		for( uint32_t index = 0; index != m_sizeLayers; ++index )
		{
			MovieLayerFrame & layer = m_layers[index];

			Helper::freeMemory( layer.anchorPoint );
			Helper::freeMemory( layer.position );
			Helper::freeMemory( layer.rotation_x );
			Helper::freeMemory( layer.rotation_y );
			Helper::freeMemory( layer.rotation_z );
			Helper::freeMemory( layer.scale );
			Helper::freeMemory( layer.opacity );
			Helper::freeMemory( layer.volume );
		}

		for( uint32_t index = 0; index != m_sizeTimeremap; ++index )
		{
			MovieLayerTimeRemap & layer = m_timeremap[index];

			Helper::freeMemory( layer.times );
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void MovieFramePack::initialize( uint32_t _size )
    {
		m_sizeLayers = _size;

		m_layers = Helper::allocateMemory<MovieLayerFrame>( _size );
    }
	//////////////////////////////////////////////////////////////////////////
	void MovieFramePack::initializeTimeremap( uint32_t _size )
	{
		m_sizeTimeremap = _size;

		m_timeremap = Helper::allocateMemory<MovieLayerTimeRemap>( _size );
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieFramePack::initializeShapes( uint32_t _size )
	{
		m_sizeShapes = _size;

		m_shapes = Helper::allocateMemory<MovieLayerShapes>( _size );
	}
	//////////////////////////////////////////////////////////////////////////
	void MovieFramePack::initializePolygons( uint32_t _size )
	{
		m_sizePolygons = _size;

		m_polygons = Helper::allocateMemory<MovieLayerPolygon>( _size );
	}
	//////////////////////////////////////////////////////////////////////////
	MovieLayerFrame & MovieFramePack::initializeLayer( uint32_t _layerIndex, uint32_t _count, bool _immutable )
	{
		MovieLayerFrame & layer = m_layers[_layerIndex];

		layer.count = _count;

		layer.anchorPoint = nullptr;
		layer.position = nullptr;
		layer.scale = nullptr;
		layer.rotation_x = nullptr;
		layer.rotation_y = nullptr;
		layer.rotation_z = nullptr;
		layer.opacity = nullptr;
		layer.volume = nullptr;
	
		layer.immutable_mask = 0;

		layer.immutable = _immutable;

		return layer;
	}
    //////////////////////////////////////////////////////////////////////////
    bool MovieFramePack::hasLayer( uint32_t _layerIndex ) const
    {
        if( (_layerIndex - 1) >= m_sizeLayers )
        {
            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	const MovieLayerFrame & MovieFramePack::getLayer( uint32_t _layerIndex ) const
	{
		const MovieLayerFrame & frameLayer = m_layers[_layerIndex - 1];

		return frameLayer;
	}
	//////////////////////////////////////////////////////////////////////////
	MovieLayerTimeRemap & MovieFramePack::mutableLayerTimeRemap( uint32_t _layerIndex )
	{
		MovieLayerTimeRemap & timeremap = m_timeremap[_layerIndex];

		return timeremap;
	}
	//////////////////////////////////////////////////////////////////////////
	MovieLayerShapes & MovieFramePack::mutableLayerShape( uint32_t _layerIndex )
	{
		MovieLayerShapes & shapes = m_shapes[_layerIndex];

		return shapes;
	}
	//////////////////////////////////////////////////////////////////////////
	MovieLayerPolygon & MovieFramePack::mutableLayerPolygon( uint32_t _layerIndex )
	{
		MovieLayerPolygon & polygon = m_polygons[_layerIndex];

		return polygon;
	}
	///////////////////////////////////////////////////////////////////////
	bool MovieFramePack::getLayerFrame( uint32_t _layerIndex, uint32_t _frameIndex, MovieFrameSource & _frame ) const
	{
        if( (_layerIndex - 1) >= m_sizeLayers )
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
	bool MovieFramePack::getLayerFrameInterpolate( uint32_t _layerIndex, uint32_t _frameIndex, float _t, MovieFrameSource & _frame ) const
	{
		if( (_layerIndex - 1) >= m_sizeLayers )
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
    bool MovieFramePack::getLayerTimeRemap( uint32_t _layerIndex, uint32_t _frameIndex, float & _time ) const
    {
		for( uint32_t index = 0; index != m_sizeTimeremap; ++index )
		{
			const MovieLayerTimeRemap & timeremap = m_timeremap[index];

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
	bool MovieFramePack::getLayerShape( uint32_t _layerIndex, uint32_t _frameIndex, const MovieFrameShape ** _shape ) const
	{
		for( uint32_t index = 0; index != m_sizeShapes; ++index )
		{
			const MovieLayerShapes & shapes = m_shapes[index];

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
	bool MovieFramePack::getLayerPolygon( uint32_t _layerIndex, const mt::vec2f ** _polygon, uint8_t & _vertexCount ) const
	{
		for( uint32_t index = 0; index != m_sizePolygons; ++index )
		{
			const MovieLayerPolygon & polygon = m_polygons[index];

			if( polygon.layerId != _layerIndex )
			{
				continue;
			}

			*_polygon = polygon.polygon;
			_vertexCount = polygon.vertexCount;

			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MovieFramePack::isLayerPermanentlyHide( uint32_t _layerIndex ) const
	{
		if( (_layerIndex - 1) >= m_sizeLayers )
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