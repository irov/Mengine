#pragma once

#include "Interface/UnknownInterface.h"
#include "Interface/RenderTextureInterface.h"
#include "Interface/ServiceInterface.h"

#include "Kernel/Vector.h"

#include "Config/Lambda.h"
#include "Config/StdInt.h"

namespace Mosaic
{
    struct Context;
    struct Frame;
    struct RenderMesh;
}

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef uint64_t MosaicTextureHandle;
    //////////////////////////////////////////////////////////////////////////
    typedef uint32_t MosaicProviderId;
    //////////////////////////////////////////////////////////////////////////
    static const MosaicProviderId MOSAIC_INVALID_PROVIDER_ID = 0;
    //////////////////////////////////////////////////////////////////////////
    class UnknownMosaicRenderInterface
        : public UnknownInterface
    {
    public:
        typedef Lambda<void( Mosaic::Context * )> LambdaMosaicProvider;
        virtual void setProvider( const LambdaMosaicProvider & _provider ) = 0;
        virtual const LambdaMosaicProvider & getProvider() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    struct MosaicProviderDesc
    {
        MosaicProviderId id;
        UnknownMosaicRenderInterface::LambdaMosaicProvider provider;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<MosaicProviderDesc> VectorMosaicProviders;
    //////////////////////////////////////////////////////////////////////////
    class MosaicServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "MosaicService" );

    public:
        virtual Mosaic::Context * getContext() const = 0;

    public:
        virtual MosaicTextureHandle getTextureHandle( const RenderTextureInterfacePtr & _texture ) = 0;
        virtual void releaseTextureHandle( MosaicTextureHandle _handle ) = 0;

    public:
        virtual const Mosaic::RenderMesh * prepareRenderMesh( const Mosaic::Frame & _frame ) = 0;

    public:
        virtual MosaicProviderId addProvider( const UnknownMosaicRenderInterface::LambdaMosaicProvider & _provider ) = 0;
        virtual void removeProvider( MosaicProviderId _id ) = 0;
        virtual const VectorMosaicProviders & getProviders() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define MOSAIC_SERVICE()\
    ((Mengine::MosaicServiceInterface *)SERVICE_GET(Mengine::MosaicServiceInterface))
//////////////////////////////////////////////////////////////////////////
