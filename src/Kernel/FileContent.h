#pragma once

#include "Interface/FileGroupInterface.h"

#include "Kernel/BaseContent.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FileContent
        : public BaseContent
    {
    public:
        FileContent();
        ~FileContent() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileContent, ContentInterface> FileContentPtr;
    //////////////////////////////////////////////////////////////////////////
}