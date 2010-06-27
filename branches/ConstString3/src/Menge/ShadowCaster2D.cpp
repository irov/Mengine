#	include "ShadowCaster2D.h"

#	include "Light2D.h"
#	include "Camera2D.h"

#	include "XmlEngine.h"
#	include "BinParser.h"

#	include "LightSystem.h"

#	include "RenderEngine.h"
#	include "Application.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	ShadowCaster2D::ShadowCaster2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ShadowCaster2D::loader( XmlElement * _xml )
	{
		Node::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Vertex", "Value", &ShadowCaster2D::addVertex );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ShadowCaster2D::parser( BinParser * _parser )
	{
		Node::parser(_parser);

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Vertex_Value, &ShadowCaster2D::addVertex );
		}
	}
#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	void ShadowCaster2D::_debugRender( Camera2D * _camera, unsigned int _debugMask )
	{
		unsigned int debugMask = Application::get()->getDebugMask();

		if( debugMask & MENGE_DEBUG_SHADOWS )
		{
			for( std::size_t i = 0; i < m_poly.num_points(); i++)
			{
				//Holder<RenderEngine>::get()->renderLine(0xFFFFFFFF, m_poly[i], m_poly[(i + 1) % m_poly.num_points() ] );
			}
		}
	}
#	endif
	//////////////////////////////////////////////////////////////////////////
	bool ShadowCaster2D::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		Holder<LightSystem>::get()->regShadowCaster(this);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ShadowCaster2D::_deactivate()
	{
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ShadowCaster2D::_compile()
	{
		if( Node::_compile() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ShadowCaster2D::_release()
	{
		Node::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	void ShadowCaster2D::_update( float _timing )
	{
		Node::_update(_timing);
	}
	//////////////////////////////////////////////////////////////////////////
	void ShadowCaster2D::addVertex(const mt::vec2f & _vertex)
	{
		m_poly.add_point(_vertex);
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ShadowCaster2D::getVertex( std::size_t i) const
	{
		return m_poly[i % m_poly.num_points()];
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f ShadowCaster2D::getEdge( std::size_t i) const
	{
		std::size_t next = (i + 1) % m_poly.num_points();
		return m_poly[next] - m_poly[i];
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ShadowCaster2D::size() const
	{
		return m_poly.num_points();
	}
	//////////////////////////////////////////////////////////////////////////
}
