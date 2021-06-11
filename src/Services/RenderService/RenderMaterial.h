#pragma once

#include "Interface/RenderMaterialInterface.h"

#include "Kernel/IntrusiveLinked.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderMaterial
        : public RenderMaterialInterface
        , public IntrusiveLinked<RenderMaterial>
    {
        DECLARE_FACTORABLE( RenderMaterial );

    public:
        RenderMaterial();
        ~RenderMaterial() override;

    public:
        void initialize( const ConstString & _name, uint32_t _id, uint32_t _hash, EPrimitiveType _primitiveType, uint32_t _textureCount, const RenderTextureInterfacePtr * _textures, const RenderMaterialStage * _stage );

    public:
        const ConstString & getName() const override;
        uint32_t getId() const override;

    public:
        void update( float _time, float _timing ) override;

    public:
        MENGINE_INLINE uint32_t getHash() const;
        MENGINE_INLINE EPrimitiveType getPrimitiveType() const override;
        MENGINE_INLINE uint32_t getTextureCount() const override;
        MENGINE_INLINE const RenderTextureInterfacePtr & getTexture( uint32_t _index ) const override;
        MENGINE_INLINE const RenderMaterialStage * getStage() const override;

    protected:
        ConstString m_name;

        uint32_t m_id;
        uint32_t m_hash;

        EPrimitiveType m_primitiveType;

        uint32_t m_textureCount;
        RenderTextureInterfacePtr m_textures[MENGINE_MAX_TEXTURE_STAGES];

        const RenderMaterialStage * m_stage;
    };
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const ConstString & RenderMaterial::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t RenderMaterial::getId() const
    {
        return m_id;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t RenderMaterial::getHash() const
    {
        return m_hash;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE EPrimitiveType RenderMaterial::getPrimitiveType() const
    {
        return m_primitiveType;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE uint32_t RenderMaterial::getTextureCount() const
    {
        return m_textureCount;
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const RenderTextureInterfacePtr & RenderMaterial::getTexture( uint32_t _index ) const
    {
        return m_textures[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    MENGINE_INLINE const RenderMaterialStage * RenderMaterial::getStage() const
    {
        return m_stage;
    }
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderMaterial, RenderMaterialInterface> RenderMaterialPtr;
    //////////////////////////////////////////////////////////////////////////
}