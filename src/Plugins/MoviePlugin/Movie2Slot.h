#pragma once

#include "Kernel/Node.h"
#include "Kernel/DummyRender.h"
#include "Kernel/BaseTransformation.h"
#include "Kernel/Optional.h"
#include "Kernel/Floats.h"

namespace Mengine
{
    class Movie2Slot
        : public Node
        , protected DummyRender
        , protected BaseTransformation
    {
        DECLARE_FACTORABLE( Movie2Slot );
        DECLARE_VISITABLE( Node );
        DECLARE_RENDERABLE();
        DECLARE_TRANSFORMABLE();

    public:
        Movie2Slot();
        ~Movie2Slot() override;

    public:
        void setMovieName( const ConstString & _movieName );
        const ConstString & getMovieName() const;

    public:
        void setCompositionName( const ConstString & _compositionName );
        const ConstString & getCompositionName() const;

    public:
        void setLayerIndex( uint32_t _layerIndex );
        uint32_t getLayerIndex() const;

    public:
        void setDimension( const Optional<mt::box2f> & _dimension );
        const Optional<mt::box2f> & getDimension() const;

    public:
        void setOptions( const UInt32s & _options );
        const UInt32s & getOptions() const;

    public:
        bool hasOption( const Char * _option4 ) const;

    protected:
        void _destroy() override;

    protected:
        void _setPersonalColor( const Color & _color ) override;
        void _setPersonalAlpha( float _alpha ) override;

    protected:
        void _hierarchyChangeParent( Node * _oldParent, Node * _newParent ) override;

    protected:
        ConstString m_movieName;
        ConstString m_compositionName;
        uint32_t m_layerIndex;

        Optional<mt::box2f> m_dimension;
        UInt32s m_options;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusiveNodePtr<Movie2Slot> Movie2SlotPtr;
    //////////////////////////////////////////////////////////////////////////
}
