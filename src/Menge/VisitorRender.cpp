#	include "VisitorRender.h"

#	include "Renderable.h"
#	include "Sprite.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	VisitorRender::VisitorRender()
	{
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void VisitorRender::call_impl( Renderable *_node )
	{
		_node->render();
		_node->visitChildren( this );
	}
	void VisitorRender::call( Renderable *_node, Node *_base)
	{
		call_impl(_node);
	}
	void VisitorRender::call( Sprite *_node, Node *_base)
	{
		call_impl(_node);
	}

}