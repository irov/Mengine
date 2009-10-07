#	include "ShadowCaster2D.h"

#	include "Light2D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "LightSystem.h"

#	include "RenderEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(ShadowCaster2D);
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
			XML_CASE_NODE( "Vertex" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( "Value", &ShadowCaster2D::addVertex );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ShadowCaster2D::_render( unsigned int _debugMask )
	{
#	ifndef MENGE_MASTER_RELEASE
		if( _debugMask & MENGE_DEBUG_SHADOWS )
		{
			for( std::size_t i = 0; i < m_poly.num_points(); i++)
			{
				//Holder<RenderEngine>::hostage()->renderLine(0xFFFFFFFF, m_poly[i], m_poly[(i + 1) % m_poly.num_points() ] );
			}
		}
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	bool ShadowCaster2D::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		Holder<LightSystem>::hostage()->regShadowCaster(this);

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
