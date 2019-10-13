#pragma once

#include "Tasks/TaskHttpBase.h"

namespace Mengine
{
    class TaskHttpDownloadAsset
        : public TaskHttpBase
    {
    public:
        TaskHttpDownloadAsset( String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, int32_t _timeout, const LambdacURLReceiver & _lambda );
        ~TaskHttpDownloadAsset() override;

    protected:
        bool _onCheck() override;
        bool _onRun() override;

    protected:
        String m_login;
        String m_password;
        FileGroupInterfacePtr m_fileGroup;
        FilePath m_filePath;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskHttpDownloadAsset> TaskHttpDownloadAssetPtr;
    //////////////////////////////////////////////////////////////////////////
}
