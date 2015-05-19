#	include "Endless.h" 

#	include "Interface/NodeInterface.h"
#	include "Interface/StringizeInterface.h"

#	include "Kernel/Node.h"

#	include "Logger/Logger.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	static const uint32_t ED_RIGHT = 0;
	static const uint32_t ED_DOWN = 1;
	static const uint32_t ED_LEFT = 2;
	static const uint32_t ED_UP = 3;
	static const uint32_t ED_CLEAR = 16;
	//////////////////////////////////////////////////////////////////////////
	Endless::Endless()
		: m_serviceProvider(nullptr)
		, m_horizontMode(true)
		, m_verticalMode(true)
		, m_elementCountX(0)
		, m_elementCountY(0)
		, m_elementWidth(0.f)
		, m_elementHeight(0.f)
		, m_offset(0.f, 0.f, 0.f)
	{ 
	}
	//////////////////////////////////////////////////////////////////////////
	Endless::~Endless()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Endless::initialize( uint32_t _countX, uint32_t _countY, float _width, float _height, const pybind::object & _cb )
	{
		m_elementCountX = _countX;
		m_elementCountY = _countY;

		m_elementWidth = _width;
		m_elementHeight = _height;

		m_elementCb = _cb;

		for( uint32_t i = 0; i != m_elementCountX; ++i )
		{
			for( uint32_t j = 0; j != m_elementCountY; ++j )
			{
				Node * element = NODE_SERVICE( m_serviceProvider )
					->createNode( STRINGIZE_STRING_LOCAL( m_serviceProvider, "Node" ) );

				float x = float( i ) * m_elementWidth;
				float y = float( j ) * m_elementHeight;

				element->setLocalPosition( mt::vec3f( x, y, 0.f ) );

				m_nodes.push_back( element );

				m_elementCb( true, ED_CLEAR, i, j, element );
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Endless::finalize()
	{
		for( uint32_t i = 0; i != m_elementCountX; ++i )
		{
			for( uint32_t j = 0; j != m_elementCountY; ++j )
			{
				uint32_t index = i + j * m_elementCountX;

				Node * node = m_nodes[index];

				m_elementCb( false, ED_CLEAR, i, j, node );

				node->destroy();
			}
		}

		m_nodes.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void Endless::setHorizontMode( bool _value )
	{
		m_horizontMode = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	void Endless::setVerticalMode( bool _value )
	{ 
		m_verticalMode = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	void Endless::slide( const mt::vec3f & _offset )
	{
		m_offset -= _offset;

		int32_t x_offset = 0;
		int32_t y_offset = 0;

		if( m_horizontMode == true )
		{
			while( m_offset.x > m_elementWidth )
			{
				m_offset.x -= m_elementWidth;

				x_offset += 1;
			}

			while( m_offset.x < -m_elementWidth )
			{
				m_offset.x += m_elementWidth;

				x_offset -= 1;
			}
		}

		if( m_verticalMode == true )
		{
			while( m_offset.y > m_elementHeight )
			{
				m_offset.y -= m_elementHeight;

				y_offset += 1;
			}

			while( m_offset.y < -m_elementHeight )
			{
				m_offset.y += m_elementHeight;

				y_offset -= 1;
			}
		}

		uint32_t x_offset_abs = x_offset > 0 ? x_offset : -x_offset;
		uint32_t y_offset_abs = y_offset > 0 ? y_offset : -y_offset;

		if( x_offset_abs >= m_elementCountX || y_offset_abs >= m_elementCountY )
		{
			for( uint32_t i = 0; i != m_elementCountX; ++i )
			{
				for( uint32_t j = 0; j != m_elementCountY; ++j )
				{
					uint32_t index = i + j * m_elementCountX;

					Node * node = m_nodes[index];

					m_elementCb( false, ED_CLEAR, i, j, node );
				}
			}

			for( uint32_t i = 0; i != m_elementCountX; ++i )
			{
				for( uint32_t j = 0; j != m_elementCountY; ++j )
				{
					uint32_t index = i + j * m_elementCountX;

					Node * node = m_nodes[index];

					m_elementCb( true, ED_CLEAR, i, j, node );
				}
			}
		}
		else
		{
			uint32_t y_from = y_offset >= 0 ? 0 : m_elementCountY - y_offset;
			uint32_t y_to = y_offset >= 0 ? y_offset : m_elementCountY;

			if( x_offset > 0 )
			{
				for( uint32_t i = 0; i != x_offset_abs; ++i )
				{
					this->slideRight_( y_from, y_to );
				}
			}
			else if( x_offset < 0 )
			{
				for( uint32_t i = 0; i != x_offset_abs; ++i )
				{
					this->slideLeft_( y_from, y_to );
				}
			}

			if( y_offset > 0 )
			{
				for( uint32_t i = 0; i != y_offset_abs; ++i )
				{
					this->slideUp_( 0, m_elementCountX );
				}
			}
			else if( y_offset < 0 )
			{
				for( uint32_t i = 0; i != y_offset_abs; ++i )
				{
					this->slideDown_( 0, m_elementCountX );
				}
			}
		}

		for( uint32_t i = 0; i != m_elementCountX; ++i )
		{
			for( uint32_t j = 0; j != m_elementCountY; ++j )
			{
				uint32_t index = i + j * m_elementCountX;

				Node * node = m_nodes[index];

				float x = float( i ) * m_elementWidth - m_offset.x;
				float y = float( j ) * m_elementHeight - m_offset.y;

				node->setLocalPosition( mt::vec3f( x, y, 0.f ) );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Endless::slideRight_( uint32_t _begin, uint32_t _end )
	{ 
		for( uint32_t j = _begin; j != _end; ++j )
		{
			uint32_t indexPop = 0 + j * m_elementCountX;

			Node * nodePop = m_nodes[indexPop];

			m_elementCb( false, ED_RIGHT, 0, j, nodePop );

			for( uint32_t i = 0; i != m_elementCountX - 1; ++i )
			{
				uint32_t index1 = (i + 0) + j * m_elementCountX;
				uint32_t index2 = (i + 1) + j * m_elementCountX;

				m_nodes[index1] = m_nodes[index2];
			}

			uint32_t indexPush = (m_elementCountX - 1) + j * m_elementCountX;

			m_nodes[indexPush] = nodePop;

			m_elementCb( true, ED_RIGHT, m_elementCountX - 1, j, nodePop );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Endless::slideLeft_( uint32_t _begin, uint32_t _end )
	{
		for( uint32_t j = _begin; j != _end; ++j )
		{
			uint32_t indexPop = (m_elementCountX - 1) + j * m_elementCountX;

			Node * nodePop = m_nodes[indexPop];

			m_elementCb( false, ED_LEFT, m_elementCountX - 1, j, nodePop );

			for( uint32_t i = m_elementCountX - 1; i != 0; --i )
			{
				uint32_t index1 = (i + 0) + j * m_elementCountX;
				uint32_t index2 = (i - 1) + j * m_elementCountX;

				m_nodes[index1] = m_nodes[index2];
			}

			uint32_t indexPush = 0 + j * m_elementCountX;

			m_nodes[indexPush] = nodePop;

			m_elementCb( true, ED_LEFT, 0, j, nodePop );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Endless::slideUp_( uint32_t _begin, uint32_t _end )
	{
		for( uint32_t j = _begin; j != _end; ++j )
		{
			uint32_t indexPop = j + (0) * m_elementCountX;

			Node * nodePop = m_nodes[indexPop];

			m_elementCb( false, ED_UP, j, 0, nodePop );

			for( uint32_t i = 0; i != m_elementCountY - 1; ++i )
			{
				uint32_t index1 = j + (i + 0) * m_elementCountX;
				uint32_t index2 = j + (i + 1) * m_elementCountX;

				m_nodes[index1] = m_nodes[index2];
			}

			uint32_t indexPush = j + (m_elementCountY - 1) * m_elementCountX;

			m_nodes[indexPush] = nodePop;

			m_elementCb( true, ED_UP, j, m_elementCountY - 1, nodePop );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Endless::slideDown_( uint32_t _begin, uint32_t _end )
	{
		for( uint32_t j = _begin; j != _end; ++j )
		{
			uint32_t indexPop = j + (m_elementCountY - 1) * m_elementCountX;

			Node * nodePop = m_nodes[indexPop];

			m_elementCb( false, ED_DOWN, m_elementCountY - 1, j, nodePop );

			for( uint32_t i = m_elementCountY - 1; i != 0; --i )
			{
				uint32_t index1 = j + (i + 0) * m_elementCountX;
				uint32_t index2 = j + (i - 1) * m_elementCountX;

				m_nodes[index1] = m_nodes[index2];
			}

			uint32_t indexPush = j + (0) * m_elementCountX;

			m_nodes[indexPush] = nodePop;

			m_elementCb( true, ED_DOWN, 0, j, nodePop );
		}
	}
}
