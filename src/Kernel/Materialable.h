#pragma once

#include "Interface/MaterialEnumInterface.h"
#include "Interface/RenderEnumInterface.h"
#include "Interface/RenderMaterialInterface.h"
#include "Interface/RenderTextureInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ResourceImage, class Resource> ResourceImagePtr;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        RenderMaterialInterfacePtr makeSolidMaterial( const ConstString & _materialName, EMaterialBlendMode _blendMode, bool _solid, const Char * _doc );
        RenderMaterialInterfacePtr makeImageMaterial( const ResourceImagePtr & _resourceImage, const ConstString & _materialName, EMaterialBlendMode _blendMode, bool _disableTextureColor, bool _solid, const Char * _doc );
        RenderMaterialInterfacePtr makeTextureMaterial( const RenderTextureInterfacePtr * _textures, uint32_t _textureCount, const ConstString & _materialName, EMaterialBlendMode _blendMode, bool _premultiply, bool _disableTextureColor, bool _solid, const Char * _doc );
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
        void setPremultiplyAlpha( bool _premultiplyAlpha );
        bool getPremultiplyAlpha() const;

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
        RenderMaterialInterfacePtr makeSolidMaterial( bool _solid, const Char * _doc ) const;
        RenderMaterialInterfacePtr makeImageMaterial( const ResourceImagePtr & _resourceImage, bool _solid, const Char * _doc ) const;
        RenderMaterialInterfacePtr makeTextureMaterial( const RenderTextureInterfacePtr * _textures, uint32_t _textureCount, bool _solid, const Char * _doc ) const;

    public:
        inline const RenderMaterialInterfacePtr & getMaterial() const;

    protected:
        inline void invalidateMaterial();
        inline bool isInvalidateMaterial() const;

    protected:
        RenderMaterialInterfacePtr getMaterial3( EMaterial _materialId
            , EPrimitiveType _primitiveType
            , uint32_t _textureCount
            , const RenderTextureInterfacePtr * _textures, const Char * _doc ) const;

        const RenderMaterialInterfacePtr & getSolidMaterial( EMaterialBlendMode _blendMode ) const;

    protected:
        ConstString m_materialName;

        EMaterialBlendMode m_blendMode;

        bool m_premultiplyAlpha;
        bool m_disableTextureColor;

        mutable RenderMaterialInterfacePtr m_material;
        mutable bool m_invalidateMaterial;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Materialable> MaterialablePtr;
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
    inline const RenderMaterialInterfacePtr & Materialable::getMaterial() const
    {
        if( this->isInvalidateMaterial() == true )
        {
            this->updateMaterial_();
        }

        return m_material;
    }
}