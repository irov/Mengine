#pragma once

#include "Interface/FileGroupInterface.h"

#include "Kernel/Mixin.h"

#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    class Content
        : public Mixin
    {
    public:
        Content();
        ~Content() override;

    public:
        void setFileGroup( const FileGroupInterfacePtr & _fileGroup );
        const FileGroupInterfacePtr & getFileGroup() const;

        void setFilePath( const FilePath & _filePath );
        const FilePath & getFilePath() const;

        void setCodecType( const ConstString & _codecType );
        const ConstString & getCodecType() const;

        void setConverterType( const ConstString & _converterType );
        const ConstString & getConverterType() const;

        void setDataflowType( const ConstString & _dataflowType );
        const ConstString & getDataflowType() const;

    public:
        void setValidNoExist( bool _valid );
        bool isValidNoExist() const;

    protected:
        FileGroupInterfacePtr m_fileGroup;

        FilePath m_filePath;
        ConstString m_codecType;
        ConstString m_converterType;
        ConstString m_dataflowType;

        bool m_validNoExist;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Content> ContentPtr;
    //////////////////////////////////////////////////////////////////////////
}
