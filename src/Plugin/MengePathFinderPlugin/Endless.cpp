#	include "Endless.h" 

#	include "Interface/NodeInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Kernel/Node.h"
#	include "Kernel/Parallax.h"

#	include "Logger/Logger.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	static const uint32_t ED_RIGHT = 0;
	static const uint32_t ED_LEFT = 2;
	static const uint32_t ED_ALL = 16;
	//////////////////////////////////////////////////////////////////////////
	Endless::Endless()
		: m_elementCount(0)
		, m_elementWidth(0.f)
		, m_offset(0.f)
		, m_enumeratorElementId(0)
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	Endless::~Endless()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Endless::initialize( uint32_t _count, float _width, const mt::vec3f & _parallax, const pybind::object & _cb )
	{
		m_elementCount = _count;
		m_elementWidth = _width;

		m_elementCb = _cb;

		if( m_elementWidth < mt::m_eps )
		{
			return false;
		}

		if( m_elementCb.is_callable() == false )
		{
			return false;
		}

		for( uint32_t i = 0; i != m_elementCount; ++i )
		{
			Parallax * parallax = NODE_SERVICE() 
				->createNodeT<Parallax *>( STRINGIZE_STRING_LOCAL( "Parallax" ) );

			parallax->setImmortal( true );
			
			float x = float( i ) * m_elementWidth;
			
			parallax->setLocalPosition( mt::vec3f( x, 0.f, 0.f ) );
			parallax->setParallaxFactor( _parallax );

			Element el;
			el.node = parallax;
			el.id = ++m_enumeratorElementId;

			m_elements.push_back( el );

			m_elementCb.call( true, ED_ALL, i, el.node, el.id );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Endless::finalize()
	{
		for( uint32_t i = 0; i != m_elementCount; ++i )
		{
			uint32_t index = i;

			Element & el = m_elements[index];

			m_elementCb.call( false, ED_ALL, i, el.node, el.id );

			el.node->setImmortal( false );
			el.node->destroy();
		}

		m_elements.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Endless::slide( float _offset )
	{
		m_offset -= _offset;

		if( m_elementCount == 0 )
		{
			return;
		}

		int32_t x_offset = 0;

		while( m_offset > m_elementWidth )
		{
			m_offset -= m_elementWidth;

			x_offset += 1;
		}

		while( m_offset < -m_elementWidth )
		{
			m_offset += m_elementWidth;

			x_offset -= 1;
		}

		uint32_t x_offset_abs = x_offset > 0 ? x_offset : -x_offset;
		
		if( x_offset > 0 )
		{
			for( uint32_t i = 0; i != x_offset_abs; ++i )
			{
				this->slideRight_();
			}
		}
		else if( x_offset < 0 )
		{
			for( uint32_t i = 0; i != x_offset_abs; ++i )
			{
				this->slideLeft_();
			}
		}

		for( uint32_t index = 0; index != m_elementCount; ++index )
		{
			Element & el = m_elements[index];

			float x = float( index ) * m_elementWidth - m_offset;

			el.node->setLocalPositionX( x );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Endless::slideRight_()
	{ 
		uint32_t indexPop = 0;

		Element elPop = m_elements[indexPop];

		m_elementCb.call( false, ED_RIGHT, indexPop, elPop.node, elPop.id );

		for( uint32_t i = 0; i != m_elementCount - 1; ++i )
		{
			uint32_t index1 = (i + 0);
			uint32_t index2 = (i + 1);

			m_elements[index1] = m_elements[index2];
		}

		uint32_t indexPush = m_elementCount - 1;

		m_elements[indexPush] = elPop;

		m_elementCb.call( true, ED_RIGHT, indexPush, elPop.node, elPop.id );
	}
	//////////////////////////////////////////////////////////////////////////
	void Endless::slideLeft_()
	{
		uint32_t indexPop = (m_elementCount - 1);

		Element elPop = m_elements[indexPop];

		m_elementCb.call( false, ED_LEFT, m_elementCount - 1, elPop.node, elPop.id );

		for( uint32_t i = m_elementCount - 1; i != 0; --i )
		{
			uint32_t index1 = (i + 0);
			uint32_t index2 = (i - 1);

			m_elements[index1] = m_elements[index2];
		}

		uint32_t indexPush = 0;

		m_elements[indexPush] = elPop;

		m_elementCb.call( true, ED_LEFT, indexPush, elPop.node, elPop.id );
	}
}
