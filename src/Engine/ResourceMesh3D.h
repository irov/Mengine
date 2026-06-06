#pragma once

#include "Kernel/Resource.h"
#include "Kernel/VectorRenderVertex3D.h"
#include "Kernel/VectorRenderIndex32.h"

#include "math/box3.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    class ResourceMesh3D
        : public Resource
    {
        DECLARE_FACTORABLE( ResourceMesh3D );
        DECLARE_VISITABLE( Resource );

    public:
        ResourceMesh3D();
        ~ResourceMesh3D() override;

    public:
        const VectorRenderVertex3D & getVertices() const;
        const VectorRenderIndex32 & getIndices() const;

        const mt::box3f & getBoundingBox() const;

    public:
        void setVertices( const VectorRenderVertex3D & _vertices );
        void setIndices( const VectorRenderIndex32 & _indices );

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void recomputeBoundingBox_();

    protected:
        VectorRenderVertex3D m_vertices;
        VectorRenderIndex32 m_indices;

        mt::box3f m_boundingBox;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceMesh3D> ResourceMesh3DPtr;
    //////////////////////////////////////////////////////////////////////////
}
