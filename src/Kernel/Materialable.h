#pragma once

#include "Interface/MaterialEnumInterface.h"
#include "Interface/RenderEnumInterface.h"
#include "Interface/RenderMaterialInterface.h"
#include "Interface/RenderTextureInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/IntrusivePtrView.h"
#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtrView<class ResourceImage, class Resource> ResourceImagePtrView;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        RenderMaterialInterfacePtr makeSolidMaterial( const ConstString & _materialName, EMaterialBlendMode _blendMode, bool _solid, const DocumentInterfacePtr & _doc );
        RenderMaterialInterfacePtr makeImageMaterial( const ResourceImagePtrView & _resourceImage, const ConstString & _materialName, EMaterialBlendMode _blendMode, bool _disableTextureColor, bool _solid, const DocumentInterfacePtr & _doc );
        RenderMaterialInterfacePtr makeTextureMaterial( const ConstString & _materialName, uint32_t _textureCount, const RenderTextureInterfacePtr * _textures, EMaterialBlendMode _blendMode, bool _premultiply, bool _disableTextureColor, bool _solid, const DocumentInterfacePtr & _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    class Materialable
        : public Mixin
    {
    public:
        Materialable();
        ~Materialable() override;

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
        void updateMaterial_() const;

    protected:
        virtual RenderMaterialInterfacePtr _updateMaterial() const = 0;

    protected:
        RenderMaterialInterfacePtr makeSolidMaterial( bool _solid, const DocumentInterfacePtr & _doc ) const;
        RenderMaterialInterfacePtr makeImageMaterial( const ResourceImagePtrView & _resourceImage, bool _solid, const DocumentInterfacePtr & _doc ) const;
        RenderMaterialInterfacePtr makeTextureMaterial( uint32_t _textureCount, const RenderTextureInterfacePtr * _textures, bool _premultiply, bool _solid, const DocumentInterfacePtr & _doc ) const;

    public:
        MENGINE_INLINE const RenderMaterialInterfacePtr & getMaterial() const;

    protected:
        MENGINE_INLINE void invalidateMaterial() const;
        MENGINE_INLINE bool isInvalidateMaterial() const;

    protected:
        virtual void _invalidateMaterial() const;

    protected:
        RenderMaterialInterfacePtr getMaterial3( EMaterial _materialId
            , EPrimitiveType _primitiveType
            , uint32_t _textureCount
            , const RenderTextureInterfacePtr * _textures, const DocumentInterfacePtr & _doc ) const;

        const RenderMaterialInterfacePtr & getSolidMaterial( EMaterialBlendMode _blendMode, bool _premultiply ) const;

    protected:
        ConstString m_materialName;

        EMaterialBlendMode m_blendMode;

        bool m_disableTextureColor;

        mutable RenderMaterialInterfacePtr m_material;
        mutable bool m_invalidateMaterial;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Materialable> MaterialablePtr;
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE void Materialable::invalidateMaterial() const
    {
        m_invalidateMaterial = true;

        this->_invalidateMaterial();
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE bool Materialable::isInvalidateMaterial() const
    {
        return m_invalidateMaterial;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const RenderMaterialInterfacePtr & Materialable::getMaterial() const
    {
        if( this->isInvalidateMaterial() == true )
        {
            this->updateMaterial_();
        }

        return m_material;
    }
    //////////////////////////////////////////////////////////////////////////
}