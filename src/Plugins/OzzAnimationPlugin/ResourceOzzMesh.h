#pragma once

#include "OzzAnimationInterface.h"

#include "OzzDetail.h"

#include "Kernel/Resource.h"
#include "Kernel/Vector.h"

#include "ozz/animation/runtime/skeleton.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ResourceOzzMesh
        : public Resource
        , public UnknownResourceOzzMeshInterface
    {
        DECLARE_FACTORABLE( ResourceOzzMesh );
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();

    public:
        ResourceOzzMesh();
        ~ResourceOzzMesh() override;

    public:
        ozz::span<const Detail::Mesh> getMeshes() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        ozz::vector<Detail::Mesh> m_meshes;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceOzzMesh> ResourceOzzMeshPtr;
    //////////////////////////////////////////////////////////////////////////
}