#pragma once

#include "GLTFImporterInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class GLTFImporterService
        : public ServiceBase<GLTFImporterServiceInterface>
    {
    public:
        GLTFImporterService();
        ~GLTFImporterService() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool importMesh( const ContentInterfacePtr & _content
            , const mt::mat4f & _preTransform
            , uint32_t _meshIndex
            , const ResourceMesh3DPtr & _resource ) override;

        bool importSkinnedMesh( const ContentInterfacePtr & _content
            , const mt::mat4f & _preTransform
            , uint32_t _meshIndex
            , const ResourceSkinnedMesh3DPtr & _resource ) override;
    };
}
