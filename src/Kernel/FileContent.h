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

    public:
        MemoryInterfacePtr createMemoryFile( bool _stream, bool _share, const DocumentInterfacePtr & _doc ) override;
        MemoryInterfacePtr createMemoryFileString( bool _stream, bool _share, const DocumentInterfacePtr & _doc ) override;

        InputStreamInterfacePtr openInputStreamFile( bool _streaming, bool _share, const DocumentInterfacePtr & _doc ) override;
        void closeInputStreamFile( const InputStreamInterfacePtr & _stream ) override;

        OutputStreamInterfacePtr openOutputStreamFile( bool _withTemp, const DocumentInterfacePtr & _doc ) override;
        void closeOutputStreamFile( const OutputStreamInterfacePtr & _stream ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileContent, ContentInterface> FileContentPtr;
    //////////////////////////////////////////////////////////////////////////
}