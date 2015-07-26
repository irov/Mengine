#	include "Materialable.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	namespace Helper
	{
		RenderMaterialInterfacePtr makeImageMaterial( ServiceProviderInterface * _serviceProvider, const ResourceImage * _resourceImage, const ConstString & _materialName, EMaterialBlendMode _blendMode, bool _disableTextureColor, bool _solid )
		{
			uint32_t texturesNum = 0;
			RenderTextureInterfacePtr textures[2];

			textures[0] = _resourceImage->getTexture();
			textures[1] = _resourceImage->getTextureAlpha();

			ConstString materialName;

			if( _materialName.empty() == true )
			{
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
								if( _resourceImage->isAlpha() == true || _solid == false )
								{
									materialId = EM_TEXTURE_BLEND;
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
								materialId = EM_TEXTURE_INTENSIVE;
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
								materialId = EM_TEXTURE_SCREEN;
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
								materialId = EM_TEXTURE_MULTIPLY;
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
							const ColourValue & colour = _resourceImage->getColor();

							if( colour.isSolid() == false || _solid == false )
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

				materialName = RENDERMATERIAL_SERVICE( _serviceProvider )
					->getMaterialName( materialId );
			}
			else
			{
				materialName = _materialName;
			}

			bool wrapU = _resourceImage->isWrapU();
			bool wrapV = _resourceImage->isWrapV();

			RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE( _serviceProvider )
				->getMaterial( materialName, wrapU, wrapV, PT_TRIANGLELIST, texturesNum, textures );

			return material;
		}
		//////////////////////////////////////////////////////////////////////////
		RenderMaterialInterfacePtr makeTextureMaterial( ServiceProviderInterface * _serviceProvider, const RenderTextureInterfacePtr * _textures, size_t _textureCount, bool _wrapU, bool _wrapV, const ConstString & _materialName, EMaterialBlendMode _blendMode, bool _disableTextureColor, bool _solid )
		{
			ConstString materialName;

			if( _materialName.empty() == true )
			{
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
									materialId = EM_TEXTURE_BLEND;
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
								materialId = EM_TEXTURE_INTENSIVE;
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
								materialId = EM_TEXTURE_SCREEN;
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
								materialId = EM_TEXTURE_MULTIPLY;
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

				materialName = RENDERMATERIAL_SERVICE( _serviceProvider )
					->getMaterialName( materialId );
			}
			else
			{
				materialName = _materialName;
			}

			RenderMaterialInterfacePtr material = RENDERMATERIAL_SERVICE( _serviceProvider )
				->getMaterial( materialName, _wrapU, _wrapV, PT_TRIANGLELIST, _textureCount, _textures );

			return material;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Materialable::Materialable()
		: m_blendMode( EMB_NORMAL )
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
	void Materialable::updateMaterial_()
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
	RenderMaterialInterfacePtr Materialable::makeImageMaterial( ServiceProviderInterface * _serviceProvider, const ResourceImage * _resourceImage, bool _solid ) const
	{
		RenderMaterialInterfacePtr material = Helper::makeImageMaterial( _serviceProvider, _resourceImage, m_materialName, m_blendMode, m_disableTextureColor, _solid );

		return material;
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMaterialInterfacePtr Materialable::makeTextureMaterial( ServiceProviderInterface * _serviceProvider, const RenderTextureInterfacePtr * _textures, size_t _textureCount, bool _wrapU, bool _wrapV, bool _solid ) const
	{
		RenderMaterialInterfacePtr material = Helper::makeTextureMaterial( _serviceProvider, _textures, _textureCount, _wrapU, _wrapV, m_materialName, m_blendMode, m_disableTextureColor, _solid );

		return material;
	}
}