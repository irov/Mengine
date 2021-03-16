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
        DECLARE_FACTORABLE( FileContent );

    public:
        FileContent();
        ~FileContent() override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileContent, ContentInterface> FileContentPtr;
    //////////////////////////////////////////////////////////////////////////
}