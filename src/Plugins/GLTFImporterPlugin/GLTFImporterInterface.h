#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ContentInterface.h"

#include "Engine/ResourceMesh3D.h"
#include "Engine/ResourceSkinnedMesh3D.h"

#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class GLTFImporterServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "GLTFImporterService" );

    public:
        virtual bool importMesh( const ContentInterfacePtr & _content
            , const mt::mat4f & _preTransform
            , uint32_t _meshIndex
            , const ResourceMesh3DPtr & _resource ) = 0;

        virtual bool importSkinnedMesh( const ContentInterfacePtr & _content
            , const mt::mat4f & _preTransform
            , uint32_t _meshIndex
            , const ResourceSkinnedMesh3DPtr & _resource ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define GLTF_IMPORTER_SERVICE()\
    ((Mengine::GLTFImporterServiceInterface *)SERVICE_GET(Mengine::GLTFImporterServiceInterface))
//////////////////////////////////////////////////////////////////////////
