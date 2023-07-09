#pragma once

#include "Interface/FileGroupInterface.h"

namespace Mengine
{
    class BaseFileGroup
        : public FileGroupInterface
    {
    public:
        BaseFileGroup();
        ~BaseFileGroup() override;

    public:
        bool initialize() override final;
        void finalize() override final;

    protected:
        virtual bool _initialize() = 0;
        virtual void _finalize() = 0;

    public:
        void setName( const ConstString & _name ) override final;
        const ConstString & getName() const override final;

    public:
        void setBaseFileGroup( const FileGroupInterfacePtr & _baseFileGroup ) override final;
        const FileGroupInterfacePtr & getBaseFileGroup() const override final;

        void setParentFileGroup( const FileGroupInterfacePtr & _parentFileGroup ) override final;
        const FileGroupInterfacePtr & getParentFileGroup() const override final;

    public:
        void setRelationPath( const FilePath & _relationPath ) override final;
        const FilePath & getRelationPath() const override final;

        void setFolderPath( const FilePath & _folderPath ) override final;
        const FilePath & getFolderPath() const override final;

    protected:
        ConstString m_name;

        FileGroupInterfacePtr m_baseFileGroup;
        FileGroupInterfacePtr m_parentFileGroup;

        FilePath m_relationPath;
        FilePath m_folderPath;
    };
}
