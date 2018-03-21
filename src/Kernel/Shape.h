#pragma once

#include "Kernel/Node.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::intrusive_ptr<class Surface> SurfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class Shape
        : public Node
    {
    public:
        Shape();
        ~Shape();
		
    public:
        void setSurface( const SurfacePtr & _surface );
        const SurfacePtr & getSurface() const;

	protected:
		bool _compile() override;
		void _release() override;

    protected:
        void _update( float _current, float _timing ) override;

    protected:
        void _invalidateColor() override;
        void _invalidateWorldMatrix() override;

    protected:
        void invalidateVerticesLocal();        
        void invalidateVerticesWM();
        void invalidateVerticesColor();

	protected:
        SurfacePtr m_surface;

		mutable bool m_invalidateVerticesLocal;
		mutable bool m_invalidateVerticesWM;
		mutable bool m_invalidateVerticesColor;
    };
}