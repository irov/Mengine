#pragma once

#include "cURLThreadTask.h"

#include "Interface/OutputStreamInterface.h"
#include "Interface/FileGroupInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLGetAssetThreadTask
        : public cURLThreadTask
    {
    public:
        cURLGetAssetThreadTask();
        ~cURLGetAssetThreadTask() override;

    public:
        bool initialize( const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filepath );

    protected:
        bool _onRun() override;

    protected:
        void _onCURL( CURL * _curl ) override;

    protected:
        void _onComplete( bool _successful ) override;

    protected:
        String m_login;
        String m_password;
        FileGroupInterfacePtr m_fileGroup;
        FilePath m_filePath;

        OutputStreamInterfacePtr m_stream;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLGetAssetThreadTask> cURLGetAssetThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}