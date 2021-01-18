#pragma once

#include "Kernel/Node.h"
#include "Kernel/BaseRender.h"
#include "Kernel/BaseTransformation.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MovieSlot
        : public Node
        , protected BaseRender
        , protected BaseTransformation
    {
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        MovieSlot();
        ~MovieSlot() override;

    public:
        void setMovieName( const ConstString & _movieName );

    protected:
        void _destroy() override;

    protected:
        void _setLocalHide( bool _value ) override;

    protected:
        void _setPersonalColor( const Color & _color ) override;
        void _setPersonalAlpha( float _alpha ) override;

    protected:
        void _hierarchyChangeParent( Node * _oldParent, Node * _newParent ) override;

    protected:
        void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const override;

    protected:
        ConstString m_movieName;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<MovieSlot> MovieSlotPtr;
    //////////////////////////////////////////////////////////////////////////
}
