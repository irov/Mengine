#pragma once

#include "Interface/ServiceInterface.h"

#include "Interface/RenderEnumInterface.h"
#include "Interface/MaterialEnumInterface.h"

#include "Interface/RenderTextureInterface.h"
#include "Interface/RenderProgramInterface.h"
#include "Interface/RenderMaterialInterface.h"

#include "Interface/FileGroupInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class RenderMaterialServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "RenderMaterialService" )

    public:
        virtual bool loadMaterials( const FileGroupInterfacePtr & _pakName, const FilePath & _fileName ) = 0;
        virtual bool unloadMaterials( const FileGroupInterfacePtr & _pakName, const FilePath & _fileName ) = 0;

    public:
        virtual const RenderVertexShaderInterfacePtr & getVertexShader( const ConstString & _name ) const = 0;
        virtual const RenderFragmentShaderInterfacePtr & getFragmentShader( const ConstString & _name ) const = 0;
        virtual const RenderVertexAttributeInterfacePtr & getVertexAttribute( const ConstString & _name ) const = 0;

        virtual const RenderProgramInterfacePtr & getProgram( const ConstString & _name ) const = 0;

    public:
        virtual void setDefaultTextureFilter( ETextureFilter _mipmap, ETextureFilter _magnification, ETextureFilter _minification ) = 0;

    public:
        virtual const RenderMaterialStage * getMaterialStage( const ConstString & _materialName ) const = 0;

        virtual const RenderMaterialStage * cacheStage( const RenderMaterialStage & _other ) = 0;

    public:
        virtual RenderMaterialInterfacePtr getMaterial( const ConstString & _materialName
            , EPrimitiveType _primitiveType
            , uint32_t _textureCount
            , const RenderTextureInterfacePtr * _textures, const Char * _doc ) = 0;

        virtual RenderMaterialInterfacePtr getMaterial2( const ConstString & _materialName
            , const RenderMaterialStage * _stage
            , EPrimitiveType _primitiveType
            , uint32_t _textureCount
            , const RenderTextureInterfacePtr * _textures, const Char * _doc ) = 0;

        virtual RenderMaterialInterfacePtr getMaterial3( EMaterial _materialId
            , EPrimitiveType _primitiveType
            , uint32_t _textureCount
            , const RenderTextureInterfacePtr * _textures, const Char * _doc ) = 0;

    public:
        virtual const RenderMaterialInterfacePtr & getSolidMaterial( EMaterialBlendMode _blendMode ) const = 0;

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
    //////////////////////////////////////////////////////////////////////////
#define RENDERMATERIAL_SERVICE()\
    ((Mengine::RenderMaterialServiceInterface*)SERVICE_GET(Mengine::RenderMaterialServiceInterface))
}