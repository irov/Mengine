#	include "ResourceFile.h"

#	include "Interface/FileSystemInterface.h"

#	include "Metacode/Metacode.h"

#	include "Logger/Logger.h"

namespace Menge
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
		const ConstString & category = this->getCategory();

		bool valid = FILE_SERVICE()
			->existFile( category, m_path, nullptr );

		return valid;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceFile::getFilePath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
}
