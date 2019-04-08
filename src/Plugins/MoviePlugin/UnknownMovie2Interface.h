#pragma once

#include "Interface/UnknownInterface.h"

#include "Kernel/Resource.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownMovie2Interface
        : public UnknownInterface
    {
    public:
        virtual void setResourceMovie2( const ResourcePtr & _resource ) = 0;
        virtual const ResourcePtr & getResourceMovie2() const = 0;

    public:
        virtual bool setCompositionName( const ConstString & _compositionName ) = 0;
        virtual const ConstString & getCompositionName() const = 0;

    public:
        virtual void setTextAliasEnvironment( const ConstString & _aliasEnvironment ) = 0;
        virtual const ConstString & getTextAliasEnvironment() const = 0;

    public:
        virtual bool setWorkAreaFromEvent( const ConstString & _eventName ) = 0;
        virtual bool removeWorkArea() = 0;

    public:
        virtual bool hasCompositionBounds() const = 0;
        virtual const mt::box2f & getCompositionBounds() const = 0;

    public:
        virtual bool hasMovieLayers( const ConstString & _name ) const = 0;
        virtual void setEnableMovieLayers( const ConstString & _name, bool _enable ) = 0;

    public:
        virtual bool getWorldBoundingBox( mt::box2f * _bb ) const = 0;

    public:
        typedef Lambda<void( Node * _node, const RenderContext * _context )> LambdaMovieRenderSlot;
        virtual void foreachRenderSlots( const RenderContext * _context, const LambdaMovieRenderSlot & _lambda ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownMovie2Interface> UnknownMovie2InterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}