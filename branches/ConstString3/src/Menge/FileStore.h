#   pragma once

#   include "Config/Typedef.h"

#   include "Loadable.h"

namespace Menge
{
    class FileStore
        : public Loadable
    {
    public:
        void loader( const Metabuf::Metadata * _parser ) override;

    public:
        bool getFileID( size_t _index, FileID & _fileId ) const;

    protected:
        typedef std::vector<FileID> TVectorFileID;
        TVectorFileID m_fileIDs;
    };
}