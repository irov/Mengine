#pragma once

#include "OzzAnimationInterface.h"

#include "Kernel/Resource.h"

#include "OzzDetail.h"

#include "Config/Vector.h"

#include "ozz/animation/runtime/skeleton.h"

namespace Mengine
{
    class ResourceOzzMesh
        : public Resource
        , public UnknownResourceOzzMeshInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        ResourceOzzMesh();
        ~ResourceOzzMesh() override;

    public:
        void setFilePath( const FilePath & _filePath ) override;
        const FilePath & getFilePath() const override;

    public:
        const Detail::Mesh & getMesh() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        FilePath m_filePath;

        Detail::Mesh m_mesh;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceOzzMesh, Resource> ResourceOzzMeshPtr;
}