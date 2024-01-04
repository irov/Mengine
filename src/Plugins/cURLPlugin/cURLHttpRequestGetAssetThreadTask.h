#pragma once

#include "cURLHttpRequestThreadTask.h"

#include "Interface/OutputStreamInterface.h"
#include "Interface/ContentInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLHttpRequestGetAssetThreadTask
        : public cURLHttpRequestThreadTask
    {
        DECLARE_FACTORABLE( cURLHttpRequestGetAssetThreadTask );

    public:
        cURLHttpRequestGetAssetThreadTask();
        ~cURLHttpRequestGetAssetThreadTask() override;

    public:
        bool initialize( const String & _login, const String & _password, const ContentInterfacePtr & _content, const FilePath & _filePathTemp );

    protected:
        bool _onThreadTaskRun() override;

    protected:
        void _onCURL( CURL * _curl ) override;

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
    typedef IntrusivePtr<cURLHttpRequestGetAssetThreadTask, ThreadTaskInterface> cURLHttpRequestGetAssetThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}