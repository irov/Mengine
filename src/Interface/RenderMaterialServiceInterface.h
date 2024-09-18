#pragma once

#include "Interface/ServiceInterface.h"

#include "Interface/RenderEnumInterface.h"
#include "Interface/MaterialEnumInterface.h"

#include "Interface/RenderTextureInterface.h"
#include "Interface/RenderProgramInterface.h"
#include "Interface/RenderMaterialInterface.h"

#include "Interface/ContentInterface.h"

namespace Mengine
{
    class RenderMaterialServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "RenderMaterialService" )

    public:
        virtual RenderVertexAttributeInterfacePtr createVertexAttribute( const ConstString & _name, uint32_t elementSize, const DocumentInterfacePtr & _doc ) = 0;
        virtual RenderVertexAttributeInterfacePtr removeVertexAttribute( const ConstString & _name ) = 0;

        virtual RenderVertexShaderInterfacePtr createVertexShader( const ConstString & _name, const ContentInterfacePtr & _content, bool _compile, const DocumentInterfacePtr & _doc ) = 0;
        virtual RenderVertexShaderInterfacePtr removeVertexShader( const ConstString & _name ) = 0;

        virtual RenderFragmentShaderInterfacePtr createFragmentShader( const ConstString & _name, const ContentInterfacePtr & _content, bool _compile, const DocumentInterfacePtr & _doc ) = 0;
        virtual RenderFragmentShaderInterfacePtr removeFragmentShader( const ConstString & _name ) = 0;

        virtual RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentInterfacePtr & _doc ) = 0;
        virtual RenderProgramInterfacePtr removeProgram( const ConstString & _name ) = 0;

    public:
        virtual const RenderVertexShaderInterfacePtr & getVertexShader( const ConstString & _name ) const = 0;
        virtual const RenderFragmentShaderInterfacePtr & getFragmentShader( const ConstString & _name ) const = 0;
        virtual const RenderVertexAttributeInterfacePtr & getVertexAttribute( const ConstString & _name ) const = 0;
        virtual const RenderProgramInterfacePtr & getProgram( const ConstString & _name ) const = 0;

    public:
        virtual void setDefaultTextureFilter( ETextureFilter _mipmap, ETextureFilter _magnification, ETextureFilter _minification ) = 0;

    public:
        virtual const RenderMaterialStage * createMaterialStage( const ConstString & _materialName, const RenderMaterialStage & _stage, const DocumentInterfacePtr & _doc ) = 0;
        virtual const RenderMaterialStage * removeMaterialStage( const ConstString & _materialName ) = 0;

        virtual const RenderMaterialStage * getMaterialStage( const ConstString & _materialName ) const = 0;
        virtual const RenderMaterialStage * cacheMaterialStage( const RenderMaterialStage & _other, const DocumentInterfacePtr & _doc ) = 0;
        virtual void uncacheMaterialStage( const RenderMaterialStage * _stage ) = 0;

    public:
        virtual void updateSolidRenderMaterial() = 0;

    public:
        virtual RenderMaterialInterfacePtr getMaterial( const ConstString & _materialName
            , EPrimitiveType _primitiveType
            , const RenderTextureInterfacePtr * _textures
            , uint32_t _textureCount, const DocumentInterfacePtr & _doc ) = 0;

        virtual RenderMaterialInterfacePtr getMaterial2( const ConstString & _materialName
            , const RenderMaterialStage * _stage
            , EPrimitiveType _primitiveType
            , const RenderTextureInterfacePtr * _textures
            , uint32_t _textureCount, const DocumentInterfacePtr & _doc ) = 0;

        virtual RenderMaterialInterfacePtr getMaterial3( EMaterial _materialId
            , EPrimitiveType _primitiveType
            , const RenderTextureInterfacePtr * _textures
            , uint32_t _textureCount, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual const RenderMaterialInterfacePtr & getSolidMaterial( EMaterialBlendMode _blendMode, bool _premultiply ) const = 0;

    public:
        virtual void setDebugLineMaterial( const RenderMaterialInterfacePtr & _debugLineMaterial ) = 0;
        virtual const RenderMaterialInterfacePtr & getDebugLineMaterial() const = 0;

        virtual void setDebugTriangleMaterial( const RenderMaterialInterfacePtr & _debugTriangleMaterial ) = 0;
        virtual const RenderMaterialInterfacePtr & getDebugTriangleMaterial() const = 0;

    public:
        virtual ETextureFilter getDefaultTextureFilterMipmap() const = 0;
        virtual ETextureFilter getDefaultTextureFilterMagnification() const = 0;
        virtual ETextureFilter getDefaultTextureFilterMinification() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define RENDERMATERIAL_SERVICE()\
    ((Mengine::RenderMaterialServiceInterface *)SERVICE_GET(Mengine::RenderMaterialServiceInterface))
//////////////////////////////////////////////////////////////////////////