#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/IntrusivePtr.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Visitor.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PrefetcherObserverInterface
        : public Factorable
    {
    public:
        virtual void onPrefetchPreparation() = 0;
        virtual bool onPrefetchRun() = 0;
        virtual void onPrefetchCancel() = 0;
        virtual void onPrefetchComplete( bool _successful ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PrefetcherObserverInterface> PrefetcherObserverInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class ImageDecoderInterface> ImageDecoderInterfacePtr;
    typedef IntrusivePtr<class SoundDecoderInterface> SoundDecoderInterfacePtr;
    typedef IntrusivePtr<class DataInterface> DataInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class PrefetcherServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "PrefetcherService" )

    public:
        virtual void update() = 0;

    public:
        virtual bool prefetchImageDecoder( const FileGroupInterfacePtr& _pakName, const FilePath & _fileName, const ConstString & _codec, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool getImageDecoder( const FileGroupInterfacePtr& _pakName, const FilePath & _fileName, ImageDecoderInterfacePtr & _decoder ) const = 0;

    public:
        virtual bool prefetchSoundDecoder( const FileGroupInterfacePtr & _pakName, const FilePath & _fileName, const ConstString & _codec, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool getSoundDecoder( const FileGroupInterfacePtr& _pakName, const FilePath & _fileName, SoundDecoderInterfacePtr & _decoder ) const = 0;

    public:
        virtual bool prefetchData( const FileGroupInterfacePtr& _pakName, const FilePath & _fileName, const ConstString & _dataflowType, const PrefetcherObserverInterfacePtr & _observer ) = 0;
        virtual bool getData( const FileGroupInterfacePtr& _pakName, const FilePath & _fileName, DataInterfacePtr & _data ) const = 0;

    public:
        virtual void unfetch( const FileGroupInterfacePtr& _pakName, const FilePath& _fileName ) = 0;

    public:
        virtual void visitPrefetches( const VisitorPtr & _visitor ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#define PREFETCHER_SERVICE()\
	((Mengine::PrefetcherServiceInterface*)SERVICE_GET(Mengine::PrefetcherServiceInterface))
    //////////////////////////////////////////////////////////////////////////
}

