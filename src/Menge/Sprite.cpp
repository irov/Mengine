#	include "Sprite.h"
#	include "Layer2D.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "Kernel/ResourceImage.h"

#	include "Consts.h"

#	include "Logger/Logger.h"

#	include "math/box2.h"
#	include "math/utils.h"

namespace	Menge
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

		m_material = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::updateMaterial()
	{
        m_invalidateMaterial = false;

		uint32_t texturesNum = 0;
		RenderTextureInterfacePtr textures[2];

		textures[0] = m_resourceImage->getTexture();
		textures[1] = m_resourceImage->getTextureAlpha();
		        
		ConstString stageName;

        if( textures[1] != nullptr )
        {
			if( m_blendAdd == true )
            {
				if( m_disableTextureColor == true )
				{
					texturesNum = 2;

					stageName = CONST_STRING(m_serviceProvider, ExternalAlphaIntensive_OnlyColor);
				}
				else if( m_resourceImage->isAlpha() == true )
				{
					texturesNum = 2;

					stageName = CONST_STRING(m_serviceProvider, ExternalAlphaIntensive);
				}
				else
				{
					texturesNum = 1;

					stageName = CONST_STRING(m_serviceProvider, Add);
				}
            }
			else
			{
				if( m_disableTextureColor == true )
				{
					texturesNum = 2;

					stageName = CONST_STRING(m_serviceProvider, ExternalAlpha_OnlyColor);
				}
				else if( m_resourceImage->isAlpha() == true )
				{
					texturesNum = 2;

					stageName = CONST_STRING(m_serviceProvider, ExternalAlpha);
				}
				else if( m_solid == false )
				{
					texturesNum = 1;

					stageName = CONST_STRING(m_serviceProvider, Blend);
				}
				else
				{
					texturesNum = 1;

					stageName = CONST_STRING(m_serviceProvider, SolidSprite);
				}
			}
        }
		else if( textures[0] != nullptr )
		{
			texturesNum = 1;

			if( m_blendAdd == true )
			{
				if( m_disableTextureColor == true )
				{
					stageName = CONST_STRING(m_serviceProvider, Intensive_OnlyColor);
				}
				else
				{
					stageName = CONST_STRING(m_serviceProvider, Add);
				}				
			}
			else
			{
				if( m_disableTextureColor == true )
				{
					stageName = CONST_STRING(m_serviceProvider, OnlyColor);
				}
				else
				{
					if( m_resourceImage->isAlpha() == true || m_solid == false )
					{
						stageName = CONST_STRING(m_serviceProvider, Blend);
					}
					else
					{
						stageName = CONST_STRING(m_serviceProvider, SolidSprite);
					}
				}
			}
		}
		else
		{
			texturesNum = 0;

			if( m_solid == false )
			{
				stageName = CONST_STRING( m_serviceProvider, Blend );
			}
			else
			{
				stageName = CONST_STRING( m_serviceProvider, SolidSprite );
			}
		}

		bool wrapU = m_resourceImage->isWrapU();
		bool wrapV = m_resourceImage->isWrapV();

		m_material = RENDERMATERIAL_SERVICE(m_serviceProvider)
			->getMaterial( stageName, wrapU, wrapV, PT_TRIANGLELIST, texturesNum, textures );

		if( m_material == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("Sprite::updateMaterial_ %s resource %s m_material is NULL"
				, this->getName().c_str()
				, m_resourceImage->getName().c_str()
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Sprite::_render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera )
	{
		Node::_render( _viewport, _camera );

		const RenderMaterialInterfacePtr & material = this->getMaterial();
		const RenderVertex2D * vertices = this->getVerticesWM();        

		const mt::box2f & bb = this->getBoundingBox();

		RENDER_SERVICE(m_serviceProvider)
			->addRenderQuad( _viewport, _camera, material, vertices, 4, &bb );
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
