#pragma once

#include "Interface/UnknownInterface.h"
#include "Interface/RenderPipelineInterface.h"
#include "Interface/TransformationInterface.h"

#include "Kernel/Magic.h"
#include "Kernel/Resource.h"
#include "Kernel/Node.h"
#include "Kernel/RenderContext.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#   include "Kernel/Scriptable.h"
#endif

#include "Config/Lambda.h"

#include "math/box2.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DECLARE_MAGIC_NUMBER( MAGIC_AEZ, 'A', 'E', 'Z', '1', 1 );
    //////////////////////////////////////////////////////////////////////////
    class Movie2SubCompositionInterface
        : public Factorable
        , public Identity
        , public Eventable
        , public Animatable
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        , public Scriptable
#endif
    {
    public:
        virtual void setSubCompositionEnable( bool _enable ) = 0;
        virtual bool getSubCompositionEnable() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Movie2SubCompositionInterface> Movie2SubCompositionInterfacePtr;
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
        virtual void removeWorkArea() = 0;

    public:
        virtual bool hasSubComposition( const ConstString & _name ) const = 0;
        virtual const Movie2SubCompositionInterfacePtr & getSubComposition( const ConstString & _name ) const = 0;

        typedef Lambda<void( const Movie2SubCompositionInterfacePtr & )> LambdaSubCompositions;
        virtual void foreachSubComposition( const LambdaSubCompositions & _lambda ) const = 0;

    public:
        virtual bool hasCompositionBounds() const = 0;
        virtual const mt::box2f & getCompositionBounds() const = 0;

    public:
        virtual bool hasMovieLayers( const ConstString & _name ) const = 0;
        virtual void setEnableMovieLayers( const ConstString & _name, bool _enable ) = 0;
        virtual void setExtraOpacityMovieLayers( const ConstString & _name, float _opacity ) = 0;

    public:
        virtual void setExtraTransformation( const ConstString & _name, const TransformationInterfacePtr & _transformation, bool _onlyPosition ) = 0;
        virtual void removeExtraTransformation( const ConstString & _name ) = 0;

    public:
        virtual bool getWorldBoundingBox( mt::box2f * const _bb ) const = 0;

    public:
        typedef Lambda<void( Node * _node, const RenderPipelineInterfacePtr &, const RenderContext * _context )> LambdaMovieRenderSlot;
        virtual void foreachRenderSlots( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, const LambdaMovieRenderSlot & _lambda ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}