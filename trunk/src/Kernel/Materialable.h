#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "Kernel/ResourceImage.h"

#	include "Core/ConstString.h"

namespace Menge
{
	enum EMaterialBlendMode 
	{	
		EMB_NORMAL,
		EMB_ADD,
		EMB_SCREEN,
		EMB_MULTIPLY
	};

	namespace Helper
	{
		RenderMaterialInterfacePtr makeImageMaterial( ServiceProviderInterface * _serviceProvider, const ResourceImage * _resourceImage, const ConstString & _materialName, EMaterialBlendMode _blend, bool _disableTextureColor, bool _solid );
		RenderMaterialInterfacePtr makeTextureMaterial( ServiceProviderInterface * _serviceProvider, const RenderTextureInterfacePtr * _textures, size_t _textureCount, bool _wrapU, bool _wrapV, const ConstString & _materialName, EMaterialBlendMode _blend, bool _disableTextureColor, bool _solid );
	}

	class Materialable
	{
	public:
		Materialable();
		~Materialable();

	public:
		void setMaterialName( const ConstString & _materialName );
		const ConstString & getMaterialName() const;

	public:
		void setDisableTextureColor( bool _disable );
		bool getDisableTextureColor() const;

	public:
		void setBlendMode( EMaterialBlendMode _blendMode );
		EMaterialBlendMode getBlendMode() const;

	public:
		void releaseMaterial();
		
	protected:
		void updateMaterial_();
		
	protected:
		virtual RenderMaterialInterfacePtr _updateMaterial() const = 0;

	protected:
		RenderMaterialInterfacePtr makeImageMaterial( ServiceProviderInterface * _serviceProvider, const ResourceImage * _resourceImage, bool _solid ) const;
		RenderMaterialInterfacePtr makeTextureMaterial( ServiceProviderInterface * _serviceProvider, const RenderTextureInterfacePtr * _textures, size_t _textureCount, bool _wrapU, bool _wrapV, bool _solid ) const;

	protected:
		inline const RenderMaterialInterfacePtr & getMaterial();

	protected:
		inline void invalidateMaterial();
		inline bool isInvalidateMaterial() const;

	private:
		RenderMaterialInterfacePtr m_material;

		ConstString m_materialName;

		EMaterialBlendMode m_blendMode;

		bool m_disableTextureColor;

		mutable bool m_invalidateMaterial;
	};
	//////////////////////////////////////////////////////////////////////////
	inline void Materialable::invalidateMaterial()
	{
		m_invalidateMaterial = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline bool Materialable::isInvalidateMaterial() const
	{
		return m_invalidateMaterial;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const RenderMaterialInterfacePtr & Materialable::getMaterial()
	{
		if( this->isInvalidateMaterial() == true )
		{
			this->updateMaterial_();
		}

		return m_material;
	}
}