#include "ResourceFile.h"

#include "Interface/FileSystemInterface.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceFile::ResourceFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceFile::~ResourceFile()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceFile::_loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceFile * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceFile *>(_meta);

        m_path = metadata->get_File_Path();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceFile::_isValid() const
    {
        const FileGroupInterfacePtr & category = this->getFileGroup();

        bool valid = category->existFile( m_path );

        return valid;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceFile::getFilePath() const
    {
        return m_path;
    }
    //////////////////////////////////////////////////////////////////////////
}
