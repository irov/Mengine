#pragma once

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

    public:
        bool exist( bool _recursive ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileContent, ContentInterface> FileContentPtr;
    //////////////////////////////////////////////////////////////////////////
}