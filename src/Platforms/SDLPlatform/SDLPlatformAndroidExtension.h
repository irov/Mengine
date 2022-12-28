#pragma once

#include "Interface/AndroidPlatformExtensionInterface.h"

#include "Environment/Android/AndroidIncluder.h"
#include "Environment/Android/AndroidUtils.h"
#include "Environment/Android/AndroidEventationHub.h"

#include "Kernel/Observable.h"

namespace Mengine
{
    class SDLPlatformAndroidExtension
        : public AndroidPlatformExtensionInterface
        , public Observable
    {
    public:
        SDLPlatformAndroidExtension();
        ~SDLPlatformAndroidExtension() override;

    public:
        bool initializePlatformExtension();
        void finalizePlatformExtension();

    public:
        bool openUrlInDefaultBrowser( const Char * _url );
        bool openMail( const Char * _email, const Char * _subject, const Char * _body );

    public:
        JNIEnv * getJENV() const override;

        jclass getJClassActivity() const override;
        jobject getJObjectActivity() const override;

        size_t getAndroidId( Char * _androidId, size_t _capacity ) const override;
        size_t getDeviceName( Char * _deviceName, size_t _capacity ) const override;
        size_t getAndroidPackageName( Char * _packageName, size_t _capacity ) const override;

    public:
        int32_t androidOpenAssetFile( const Char * _path ) override;
        int32_t androidAvailableAssetFile( int32_t _fileId ) override;
        int32_t androidReadAssetFile( int32_t _fileId, int32_t _offset, int32_t _size, void * const _buffer ) override;
        int32_t androidSkipAssetFile( int32_t _fileId, int32_t _offset ) override;
        void androidResetAssetFile( int32_t _fileId ) override;
        void androidCloseAssetFile( int32_t _fileId ) override;

    protected:
        void addAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) override;
        void removeAndroidEventation( const AndroidEventationInterfacePtr & _eventation ) override;

    protected:
        void notifyApplicationRun_();
        void notifyApplicationReady_();
        void notifyApplicationStop_();
        void notifyBootstrapperInitializeBaseServices_();
        void notifyBootstrapperCreateApplication_();

    protected:
        JNIEnv * m_jenv;

        AndroidEventationHubPtr m_androidEventationHub;
    };
}
