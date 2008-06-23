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
	void NodeRenderable::render( bool _enableDebug )
	{
		if( isRenderable() == false )
		{
			return;
		}

		if( _renderBegin() )
		{
			_render( _enableDebug );

			struct ForeachRender
				: public NodeForeach
			{
				bool m_debug;
				ForeachRender( bool _enableDebug )
					: m_debug( _enableDebug )
				{
				}
				void apply( Node * children ) override
				{
					children->render( m_debug );
				}
			};

			foreachChildren( ForeachRender( _enableDebug ) );

			_renderEnd();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void NodeRenderable::_render( bool _enableDebug )
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