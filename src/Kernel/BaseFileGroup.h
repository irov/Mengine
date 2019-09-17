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
        void setName( const ConstString & _name ) override;
        const ConstString & getName() const override;

    public:
        void setBaseFileGroup( const FileGroupInterfacePtr & _baseFileGroup ) override;
        const FileGroupInterfacePtr & getBaseFileGroup() const override;

        void setParentFileGroup( const FileGroupInterfacePtr & _parentFileGroup ) override;
        const FileGroupInterfacePtr & getParentFileGroup() const override;

    public:
        void setRelationPath( const FilePath & _relationPath ) override;
        const FilePath & getRelationPath() const override;

        void setFolderPath( const FilePath & _folderPath ) override;
        const FilePath & getFolderPath() const override;

    protected:
        ConstString m_name;

        FileGroupInterfacePtr m_baseFileGroup;
        FileGroupInterfacePtr m_parentFileGroup;

        FilePath m_relationPath;
        FilePath m_folderPath;
    };
}
