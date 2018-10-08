#pragma once

#include "Kernel/Resource.h"

#include "Kernel/FilePath.h"

namespace Mengine
{
    class ResourceFile
        : public Resource
    {
        DECLARE_VISITABLE( Resource );

    public:
        ResourceFile();
        ~ResourceFile() override;

    public:
        bool _loader( const Metabuf::Metadata * _parser ) override;

    public:
        void setFilePath( const FilePath & _filePath );
        const FilePath & getFilePath() const;

    protected:
        FilePath m_filePath;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ResourceFile> ResourceFilePtr;
}
