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
        void setGroupName( const ConstString & _groupName ) override final;
        const ConstString & getGroupName() const override final;

    public:
        void setFileGroup( const FileGroupInterfacePtr & _fileGroup ) override final;
        const FileGroupInterfacePtr & getFileGroup() const override final;

        void setFilePath( const FilePath & _filePath ) override final;
        const FilePath & getFilePath() const override final;

        void getFullPath( Char * const _fullPath ) const override final;

    public:
        void setCodecType( const ConstString & _codecType ) override final;
        const ConstString & getCodecType() const override final;

        void setConverterType( const ConstString & _converterType ) override final;
        const ConstString & getConverterType() const override final;

        void setDataflow( const DataflowInterfacePtr & _dataflow ) override final;
        const DataflowInterfacePtr & getDataflow() const override final;

    public:
        void setValidNoExist( bool _valid ) override final;
        bool isValidNoExist() const override final;

    protected:
        ConstString m_groupName;

        FileGroupInterfacePtr m_fileGroup;
        FilePath m_filePath;

        ConstString m_codecType;
        ConstString m_converterType;

        DataflowInterfacePtr m_dataflow;

        bool m_validNoExist;
    };
}