#include "Materialable.h"

#include "Interface/RenderMaterialServiceInterface.h"

#include "Logger/Logger.h"
#include "Kernel/ResourceImage.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
        //////////////////////////////////////////////////////////////////////////
        RenderMaterialInterfacePtr makeSolidMaterial( const ConstString & _materialName, EMaterialBlendMode _blendMode, bool _solid )
        {
            if( _materialName.empty() == false )
            {
                RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
                    ->getMaterial( _materialName, PT_TRIANGLELIST, 0, nullptr );

                return material;
            }

            EMaterial materialId = EM_DEBUG;

            switch( _blendMode )
            {
            case EMB_NORMAL:
                {
                    if( _solid == false )
                    {
                        materialId = EM_COLOR_BLEND;
                    }
                    else
                    {
                        materialId = EM_COLOR_SOLID;
                    }
                }break;
            case EMB_ADD:
                {
                    materialId = EM_COLOR_INTENSIVE;
                }break;
            case EMB_SCREEN:
                {
                    materialId = EM_COLOR_SCREEN;
                }break;
            case EMB_MULTIPLY:
                {
                    materialId = EM_COLOR_MULTIPLY;
                }break;
            }

            RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
                ->getMaterial3( materialId, PT_TRIANGLELIST, 0, nullptr );

            return material;
        }
        //////////////////////////////////////////////////////////////////////////
		RenderMaterialInterfacePtr makeImageMaterial( const ResourceImagePtr & _resourceImage, const ConstString & _materialName, EMaterialBlendMode _blendMode, bool _disableTextureColor, bool _solid )
		{
			uint32_t texturesNum = 0;
			RenderTextureInterfacePtr textures[2];
			
			bool premultiply = false;

			if( _resourceImage != nullptr )
			{
				textures[0] = _resourceImage->getTexture();
				textures[1] = _resourceImage->getTextureAlpha();

				premultiply = _resourceImage->isPremultiply();
			}
			else
			{
				textures[0] = nullptr;
				textures[1] = nullptr;
			}

            if( _materialName.empty() == false )
            {
                if( textures[1] != nullptr )
                {
                    texturesNum = 2;
                }
                else if( textures[0] != nullptr )
                {
                    texturesNum = 1;
                }

                RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
                    ->getMaterial( _materialName, PT_TRIANGLELIST, texturesNum, textures );

                return material;
            }

            EMaterial materialId = EM_DEBUG;

            if( textures[1] != nullptr )
            {
                texturesNum = 2;

                switch( _blendMode )
                {
                case EMB_NORMAL:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_BLEND_EXTERNAL_ALPHA_ONLYCOLOR;
                        }
                        else
                        {
                            materialId = EM_TEXTURE_BLEND_EXTERNAL_ALPHA;
                        }
                    }break;
                case EMB_ADD:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_INTENSIVE_EXTERNAL_ALPHA_ONLYCOLOR;
                        }
                        else
                        {
                            materialId = EM_TEXTURE_INTENSIVE_EXTERNAL_ALPHA;
                        }
                    }break;
                case EMB_SCREEN:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_SCREEN_EXTERNAL_ALPHA_ONLYCOLOR;
                        }
                        else
                        {
                            materialId = EM_TEXTURE_SCREEN_EXTERNAL_ALPHA;
                        }
                    }
                case EMB_MULTIPLY:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_MULTIPLY_EXTERNAL_ALPHA_ONLYCOLOR;
                        }
                        else
                        {
                            materialId = EM_TEXTURE_MULTIPLY_EXTERNAL_ALPHA;
                        }
                    }
                default:
                    break;
                }
            }
            else if( textures[0] != nullptr )
            {
                texturesNum = 1;

                switch( _blendMode )
                {
                case EMB_NORMAL:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_BLEND_ONLYCOLOR;
                        }
                        else
                        {
                            bool isAlpha = true;

                            if( _resourceImage != nullptr )
                            {
                                isAlpha = _resourceImage->hasAlpha();
                            }

                            if( isAlpha == true || _solid == false )
                            {
                                if( premultiply == false )
                                {
                                    materialId = EM_TEXTURE_BLEND;
                                }
                                else
                                {
                                    materialId = EM_TEXTURE_BLEND_PREMULTIPLY;
                                }
                            }
                            else
                            {
                                materialId = EM_TEXTURE_SOLID;
                            }
                        }
                    }break;
                case EMB_ADD:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_INTENSIVE_ONLYCOLOR;
                        }
                        else
                        {
                            if( premultiply == false )
                            {
                                materialId = EM_TEXTURE_INTENSIVE;
                            }
                            else
                            {
                                materialId = EM_TEXTURE_INTENSIVE_PREMULTIPLY;
                            }
                        }
                    }break;
                case EMB_SCREEN:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_SCREEN_ONLYCOLOR;
                        }
                        else
                        {
                            if( premultiply == false )
                            {
                                materialId = EM_TEXTURE_SCREEN;
                            }
                            else
                            {
                                materialId = EM_TEXTURE_SCREEN_PREMULTIPLY;
                            }
                        }
                    }break;
                case EMB_MULTIPLY:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_MULTIPLY_ONLYCOLOR;
                        }
                        else
                        {
                            if( premultiply == false )
                            {
                                materialId = EM_TEXTURE_MULTIPLY;
                            }
                            else
                            {
                                materialId = EM_TEXTURE_MULTIPLY_PREMULTIPLY;
                            }
                        }
                    }break;
                }
            }
            else
            {
                texturesNum = 0;

                switch( _blendMode )
                {
                case EMB_NORMAL:
                    {
                        if( _resourceImage != nullptr )
                        {
                            const ColourValue & colour = _resourceImage->getColor();

                            if( colour.isSolid() == false || _solid == false )
                            {
                                materialId = EM_COLOR_BLEND;
                            }
                            else
                            {
                                materialId = EM_COLOR_SOLID;
                            }
                        }
                        else
                        {
                            if( _solid == false )
                            {
                                materialId = EM_COLOR_BLEND;
                            }
                            else
                            {
                                materialId = EM_COLOR_SOLID;
                            }
                        }
                    }break;
                case EMB_ADD:
                    {
                        materialId = EM_COLOR_INTENSIVE;
                    }break;
                case EMB_SCREEN:
                    {
                        materialId = EM_COLOR_SCREEN;
                    }break;
                case EMB_MULTIPLY:
                    {
                        materialId = EM_COLOR_MULTIPLY;
                    }break;
                }
            }

            RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
                ->getMaterial3( materialId, PT_TRIANGLELIST, texturesNum, textures );

			return material;
		}
		//////////////////////////////////////////////////////////////////////////
		RenderMaterialInterfacePtr makeTextureMaterial( const RenderTextureInterfacePtr * _textures, uint32_t _textureCount, const ConstString & _materialName, EMaterialBlendMode _blendMode, bool _premultiply, bool _disableTextureColor, bool _solid )
		{
            if( _materialName.empty() == false )
            {
                RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
                    ->getMaterial( _materialName, PT_TRIANGLELIST, _textureCount, _textures );

                return material;
            }

            EMaterial materialId = EM_DEBUG;

            if( _textureCount == 2 )
            {
                switch( _blendMode )
                {
                case EMB_NORMAL:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_BLEND_EXTERNAL_ALPHA_ONLYCOLOR;
                        }
                        else
                        {
                            materialId = EM_TEXTURE_BLEND_EXTERNAL_ALPHA;
                        }
                    }break;
                case EMB_ADD:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_INTENSIVE_EXTERNAL_ALPHA_ONLYCOLOR;
                        }
                        else
                        {
                            materialId = EM_TEXTURE_INTENSIVE_EXTERNAL_ALPHA;
                        }
                    }break;
                case EMB_SCREEN:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_SCREEN_EXTERNAL_ALPHA_ONLYCOLOR;
                        }
                        else
                        {
                            materialId = EM_TEXTURE_SCREEN_EXTERNAL_ALPHA;
                        }
                    }
                case EMB_MULTIPLY:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_MULTIPLY_EXTERNAL_ALPHA_ONLYCOLOR;
                        }
                        else
                        {
                            materialId = EM_TEXTURE_MULTIPLY_EXTERNAL_ALPHA;
                        }
                    }
                default:
                    break;
                }
            }
            else if( _textureCount == 1 )
            {
                switch( _blendMode )
                {
                case EMB_NORMAL:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_BLEND_ONLYCOLOR;
                        }
                        else
                        {
                            if( _solid == false )
                            {
                                if( _premultiply == false )
                                {
                                    materialId = EM_TEXTURE_BLEND;
                                }
                                else
                                {
                                    materialId = EM_TEXTURE_BLEND_PREMULTIPLY;
                                }
                            }
                            else
                            {
                                materialId = EM_TEXTURE_SOLID;
                            }
                        }
                    }break;
                case EMB_ADD:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_INTENSIVE_ONLYCOLOR;
                        }
                        else
                        {
                            if( _premultiply == false )
                            {
                                materialId = EM_TEXTURE_INTENSIVE;
                            }
                            else
                            {
                                materialId = EM_TEXTURE_INTENSIVE_PREMULTIPLY;
                            }
                        }
                    }break;
                case EMB_SCREEN:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_SCREEN_ONLYCOLOR;
                        }
                        else
                        {
                            if( _premultiply == false )
                            {
                                materialId = EM_TEXTURE_SCREEN;
                            }
                            else
                            {
                                materialId = EM_TEXTURE_SCREEN_PREMULTIPLY;
                            }
                        }
                    }break;
                case EMB_MULTIPLY:
                    {
                        if( _disableTextureColor == true )
                        {
                            materialId = EM_TEXTURE_MULTIPLY_ONLYCOLOR;
                        }
                        else
                        {
                            if( _premultiply == false )
                            {
                                materialId = EM_TEXTURE_MULTIPLY;
                            }
                            else
                            {
                                materialId = EM_TEXTURE_MULTIPLY_PREMULTIPLY;
                            }
                        }
                    }break;
                }
            }
            else
            {
                switch( _blendMode )
                {
                case EMB_NORMAL:
                    {
                        if( _solid == false )
                        {
                            materialId = EM_COLOR_BLEND;
                        }
                        else
                        {
                            materialId = EM_COLOR_SOLID;
                        }
                    }break;
                case EMB_ADD:
                    {
                        materialId = EM_COLOR_INTENSIVE;
                    }break;
                case EMB_SCREEN:
                    {
                        materialId = EM_COLOR_SCREEN;
                    }break;
                case EMB_MULTIPLY:
                    {
                        materialId = EM_COLOR_MULTIPLY;
                    }break;
                }
            }

            RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE()
                ->getMaterial3( materialId, PT_TRIANGLELIST, _textureCount, _textures );

			return material;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Materialable::Materialable()
		: m_blendMode( EMB_NORMAL )
        , m_premultiplyAlpha( false )
		, m_disableTextureColor( false )
		, m_invalidateMaterial( true )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Materialable::~Materialable()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Materialable::setMaterialName( const ConstString & _materialName )
	{
		m_materialName = _materialName;

		this->invalidateMaterial();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & Materialable::getMaterialName() const
	{
		return m_materialName;
	}
	//////////////////////////////////////////////////////////////////////////
	void Materialable::setDisableTextureColor( bool _disable )
	{
		m_disableTextureColor = _disable;

		this->invalidateMaterial();
	}
	//////////////////////////////////////////////////////////////////////////
	bool Materialable::getDisableTextureColor() const
	{
		return m_disableTextureColor;
	}
    //////////////////////////////////////////////////////////////////////////
    void Materialable::setPremultiplyAlpha( bool _premultiplyAlpha )
    {
        m_premultiplyAlpha = _premultiplyAlpha;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Materialable::getPremultiplyAlpha() const
    {
        return m_premultiplyAlpha;
    }
	//////////////////////////////////////////////////////////////////////////
	void Materialable::setBlendMode( EMaterialBlendMode _blendMode )
	{
		m_blendMode = _blendMode;

		this->invalidateMaterial();
	}
	//////////////////////////////////////////////////////////////////////////
	EMaterialBlendMode Materialable::getBlendMode() const
	{
		return m_blendMode;
	}
	//////////////////////////////////////////////////////////////////////////
	void Materialable::updateMaterial_() const
	{
		m_invalidateMaterial = false;

		m_material = this->_updateMaterial();
	}
	//////////////////////////////////////////////////////////////////////////
	void Materialable::releaseMaterial()
	{ 
		m_material = nullptr;

		this->invalidateMaterial();
	}
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr Materialable::makeSolidMaterial( bool _solid ) const
    {
        RenderMaterialInterfacePtr material = Helper::makeSolidMaterial( m_materialName, m_blendMode, _solid );

        return material;
    }
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr Materialable::makeImageMaterial( const ResourceImagePtr & _resourceImage, bool _solid ) const
	{
		RenderMaterialInterfacePtr material = Helper::makeImageMaterial( _resourceImage, m_materialName, m_blendMode, m_disableTextureColor, _solid );

		return material;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr Materialable::makeTextureMaterial( const RenderTextureInterfacePtr * _textures, uint32_t _textureCount, bool _solid ) const
	{
		RenderMaterialInterfacePtr material = Helper::makeTextureMaterial( _textures, _textureCount, m_materialName, m_blendMode, m_premultiplyAlpha, m_disableTextureColor, _solid );

		return material;
	}
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr Materialable::getMaterial3( EMaterial _materialId
        , EPrimitiveType _primitiveType
        , uint32_t _textureCount
        , const RenderTextureInterfacePtr * _textures ) const
    {
        const RenderMaterialInterfacePtr & material = RENDERMATERIAL_SERVICE()
            ->getMaterial3( _materialId, _primitiveType, _textureCount, _textures );

        return material;
    }
}