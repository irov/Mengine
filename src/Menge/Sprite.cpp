#	include "Sprite.h"
#	include "Layer2D.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "Kernel/ResourceImage.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "math/box2.h"
#	include "math/utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	Sprite::Sprite()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Sprite::~Sprite()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Sprite::_compile()
	{
		if( Shape::_compile() == false )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_release()
	{
		Shape::_release();
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr Sprite::_updateMaterial() const
	{
		RenderMaterialInterfacePtr material = this->makeImageMaterial( m_serviceProvider, m_resourceImage, m_solid );

		if( material == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Sprite::updateMaterial_ %s resource %s m_material is NULL"
				, this->getName().c_str()
				, m_resourceImage->getName().c_str()
				);

			return nullptr;
		}

		return material;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_render( const RenderObjectState * _state )
	{
		Node::_render( _state );

		const RenderMaterialInterfacePtr & material = this->getMaterial();
		const RenderVertex2D * vertices = this->getVerticesWM();

		const mt::box2f & bb = this->getBoundingBox();

		RENDER_SERVICE( m_serviceProvider )
			->addRenderQuad( _state, material, vertices, 4, &bb, false );
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_updateBoundingBox( mt::box2f & _boundingBox ) const
	{
		const RenderVertex2D * vertices = this->getVerticesWM();

		mt::reset( _boundingBox, vertices[0].pos.x, vertices[0].pos.y );

		mt::add_internal_point( _boundingBox, vertices[1].pos.x, vertices[1].pos.y );
		mt::add_internal_point( _boundingBox, vertices[2].pos.x, vertices[2].pos.y );
		mt::add_internal_point( _boundingBox, vertices[3].pos.x, vertices[3].pos.y );
	}
    //////////////////////////////////////////////////////////////////////////
    void Sprite::_invalidateColor()
    {
        Shape::_invalidateColor();

        bool solid = this->isSolidColor();

        if( m_solid != solid )
        {
			m_solid = solid;

            this->invalidateMaterial();
        }
    }
}
