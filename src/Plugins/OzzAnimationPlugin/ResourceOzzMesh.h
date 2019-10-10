#pragma once

#include "OzzAnimationInterface.h"

#include "OzzDetail.h"

#include "Kernel/Resource.h"
#include "Kernel/BaseContent.h"

#include "Config/Vector.h"

#include "ozz/animation/runtime/skeleton.h"

namespace Mengine
{
    class ResourceOzzMesh
        : public Resource
        , private BaseContent
        , public UnknownResourceOzzMeshInterface
    {
        DECLARE_VISITABLE( Resource );
        DECLARE_UNKNOWABLE();
        DECLARE_CONTENTABLE();

    public:
        ResourceOzzMesh();
        ~ResourceOzzMesh() override;

    public:
        const Detail::Mesh & getMesh() const;

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        Detail::Mesh m_mesh;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceOzzMesh> ResourceOzzMeshPtr;
    //////////////////////////////////////////////////////////////////////////
}