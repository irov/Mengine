#	include "NodeRenderable.h"

#	include "NodeForeach.h"

#	include "XmlEngine.h"

namespace Menge
{
	NodeRenderable::NodeRenderable()
		: m_hide( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeRenderable::hide( bool _value )
	{
		m_hide = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeRenderable::isHide() const
	{
		return m_hide;
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeRenderable::isRenderable()
	{
		if( isEnable() == false )
		{
			return false;
		}

		if( isActivate() == false )
		{
			return false;
		}

		if( m_hide == true )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeRenderable::render()
	{
		if( isRenderable() == false )
		{
			return;
		}

		if( _renderBegin() )
		{
			_render();

			struct ForeachRender
				: public NodeForeach
			{
				void apply( Node * children ) override
				{
					children->render();
				}
			};

			foreachChildren( ForeachRender() );

			_renderEnd();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeRenderable::_render()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool NodeRenderable::_renderBegin()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeRenderable::_renderEnd()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeRenderable::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Hide")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE("Value", m_hide);
				}
			}
		}
	}
}