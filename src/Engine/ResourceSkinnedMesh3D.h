#pragma once

#include "Kernel/Resource.h"
#include "Kernel/VectorRenderVertexSkinned3D.h"
#include "Kernel/VectorRenderIndex32.h"

#include "math/box3.h"
#include "math/mat4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    class ResourceSkinnedMesh3D
        : public Resource
    {
        DECLARE_FACTORABLE( ResourceSkinnedMesh3D );
        DECLARE_VISITABLE( Resource );

    public:
        ResourceSkinnedMesh3D();
        ~ResourceSkinnedMesh3D() override;

    public:
        const VectorRenderVertexSkinned3D & getVertices() const;
        const VectorRenderIndex32 & getIndices() const;
        const Vector<mt::mat4f> & getInverseBindMatrices() const;

        const mt::box3f & getBoundingBox() const;

        uint32_t getBoneCount() const;

    public:
        void setVertices( const VectorRenderVertexSkinned3D & _vertices );
        void setIndices( const VectorRenderIndex32 & _indices );
        void setInverseBindMatrices( const Vector<mt::mat4f> & _matrices );

    protected:
        bool _compile() override;
        void _release() override;

    protected:
        void recomputeBoundingBox_();

    protected:
        VectorRenderVertexSkinned3D m_vertices;
        VectorRenderIndex32 m_indices;
        Vector<mt::mat4f> m_inverseBindMatrices;

        mt::box3f m_boundingBox;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveResourcePtr<ResourceSkinnedMesh3D> ResourceSkinnedMesh3DPtr;
    //////////////////////////////////////////////////////////////////////////
}
