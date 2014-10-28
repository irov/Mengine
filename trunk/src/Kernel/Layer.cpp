#	include "Kernel/Layer.h"

#	include "Interface/RenderSystemInterface.h"

#	include "Kernel/Scene.h"

#	include "Logger/Logger.h"

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
	bool Layer::_activate()
	{
		if( Node::_activate() == false )
		{
			return false;
		}

		if( m_scene == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Layer::_activate %s not setup scene!"
				, this->getName().c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::setMain( bool _main )
	{
		m_main = _main;

		if( m_scene == nullptr )
		{
			return;
		}
			
		m_scene->setMainLayer( this );
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

		if( m_main == true && m_scene != nullptr )
		{
			m_scene->setMainLayer( this );
		}
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

		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::calcScreenPosition( mt::vec2f & _screen, const RenderCameraInterface * _camera, Node* _node ) const
	{
		const mt::vec3f & wp = _node->getWorldPosition();

		const mt::mat4f & vm = _camera->getCameraViewMatrix();

		mt::vec3f sc;
		mt::mul_m4_v3( sc, vm, wp );

		_screen.x = sc.x;
		_screen.y = sc.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::_addChild( Node * _node )
	{
		_node->setLayer( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::_changeParent( Node * _oldParent, Node * _newParent )
	{
		(void)_oldParent;        

		if( _newParent == nullptr )
		{
			return;
		}

		Scene * scene = _newParent->getScene();

		if( scene == nullptr )
		{
			return;
		}

		this->setScene( scene );
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer::_updateBoundingBox( mt::box2f& _boundingBox ) const
	{
        (void)_boundingBox;
		// do nothing
	}
}
