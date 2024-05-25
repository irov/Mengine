#pragma once

#include "AppleHttpRequestThreadTask.h"

#include "Interface/OutputStreamInterface.h"
#include "Interface/ContentInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AppleHttpRequestGetAssetThreadTask
        : public AppleHttpRequestThreadTask
    {
        DECLARE_FACTORABLE( AppleHttpRequestGetAssetThreadTask );

    public:
        AppleHttpRequestGetAssetThreadTask();
        ~AppleHttpRequestGetAssetThreadTask() override;

    public:
        bool initialize( const String & _login, const String & _password, const ContentInterfacePtr & _content, const FilePath & _filePathTemp );

    protected:
        bool _onThreadTaskRun() override;

    protected:
        jobject _onHttp( JNIEnv * _jenv, jobject _basses ) override;

    protected:
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        String m_login;
        String m_password;
        ContentInterfacePtr m_content;
        FilePath m_filePathTemp;

        OutputStreamInterfacePtr m_stream;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AppleHttpRequestGetAssetThreadTask, ThreadTaskInterface> AppleHttpRequestGetAssetThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}