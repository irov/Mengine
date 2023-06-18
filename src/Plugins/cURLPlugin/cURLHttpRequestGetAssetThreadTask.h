#pragma once

#include "cURLHttpRequestThreadTask.h"

#include "Interface/OutputStreamInterface.h"
#include "Interface/FileGroupInterface.h"

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
        bool initialize( const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filepath, const FilePath & _filePathTemp );

    protected:
        bool _onThreadTaskRun() override;

    protected:
        void _onCURL( CURL * _curl ) override;

    protected:
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        String m_login;
        String m_password;
        FileGroupInterfacePtr m_fileGroup;
        FilePath m_filePath;
        FilePath m_filePathTemp;

        OutputStreamInterfacePtr m_stream;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHttpRequestGetAssetThreadTask, ThreadTaskInterface> cURLHttpRequestGetAssetThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}