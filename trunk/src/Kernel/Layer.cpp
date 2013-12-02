#	include "Kernel/Layer.h"

#	include "Interface/RenderSystemInterface.h"

#	include "Kernel/Scene.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Layer::Layer()
		: m_main(false)
		, m_size(0.f, 0.f)
		, m_scene(nullptr)
	{
		m_layer = this;
	}
	//////////////////////////////////////////////////////////////////////////
	Layer::~Layer()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::setMain( bool _main )
	{
		m_main = _main;

		if( m_scene )
		{
			m_scene->setMainLayer( this );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer::isMain() const
	{
		return m_main;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::setScene( Scene * _scene )
	{
		m_scene = _scene;
	}
	//////////////////////////////////////////////////////////////////////////
	Scene * Layer::getScene()
	{
		return m_scene;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::setSize( const mt::vec2f & _size )
	{
		m_size = _size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Layer::getSize() const
	{
		return m_size;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::setLayer( Layer * _layer )
	{
        (void)_layer;

		Node::setLayer( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::calcScreenPosition( mt::vec2f & _screen, const RenderCameraInterface * _camera, Node* _node ) const
	{
		const mt::vec3f & wp = _node->getWorldPosition();

		const mt::mat4f & vm = _camera->getViewMatrix();

		mt::vec3f sc;
		mt::mul_m4_v3( sc, vm, wp );

		_screen.x = sc.x;
		_screen.y = sc.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::_updateBoundingBox( mt::box2f& _boundingBox )
	{
        (void)_boundingBox;
		// do nothing
	}
}
