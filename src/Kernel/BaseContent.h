#pragma once

#include "Interface/ContentInterface.h"

namespace Mengine
{
    class BaseContent
        : public ContentInterface
    {
    public:
        BaseContent();
        ~BaseContent() override;

    public:
        void setFileGroup( const FileGroupInterfacePtr & _fileGroup ) override;
        const FileGroupInterfacePtr & getFileGroup() const override;

        void setFilePath( const FilePath & _filePath ) override;
        const FilePath & getFilePath() const override;

        void setCodecType( const ConstString & _codecType ) override;
        const ConstString & getCodecType() const override;

        void setConverterType( const ConstString & _converterType ) override;
        const ConstString & getConverterType() const override;

        void setDataflow( const DataflowInterfacePtr & _dataflow ) override;
        const DataflowInterfacePtr & getDataflow() const override;

    public:
        void setValidNoExist( bool _valid ) override;
        bool isValidNoExist() const override;

    public:
        bool existContent() const override;

    protected:
        FileGroupInterfacePtr m_fileGroup;

        FilePath m_filePath;
        ConstString m_codecType;
        ConstString m_converterType;

        DataflowInterfacePtr m_dataflow;

        bool m_validNoExist;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BaseContent> ContentPtr;
    //////////////////////////////////////////////////////////////////////////
}
