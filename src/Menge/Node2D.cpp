
#	include "Node2D.h"
#	include "Player.h"
#	include "Camera2D.h"
#	include "Layer2D.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Node2D::Node2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Node2D::changePivot()
	{
		Allocator2D::changePivot();

		this->updateBoundingBox();
	
		for( TContainerChildren::iterator
				it = m_children.begin(),
				it_end = m_children.end();
				it != it_end;
			++it)
		{
			static_cast<Node2D*>( (*it) )->changePivot();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Node2D::_changePivot()
	{
		Allocator2D::_changePivot();
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Node2D::getScreenPosition()
	{
		const mt::vec2f & pos = getWorldPosition();

		mt::vec2f screen_pos;

		Camera2D * camera = Holder<Player>::hostage()
			->getRenderCamera2D();

		const Viewport & viewport = camera->getViewport();

		if( m_layer )
		{
			const mt::vec2f & factor = static_cast<Layer2D*>( m_layer )->getParallaxFactor();
			Viewport viewportParallax = viewport;
			viewportParallax.parallax( factor );

			screen_pos = pos - viewportParallax.begin;
		}
		else
		{
			screen_pos = pos - viewport.begin;
		}

		return screen_pos;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge