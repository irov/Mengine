#	include "ResourceModel3D.h"

#	include "Kernel/ResourceImplement.h"

#	include "Interface/ConverterInterface.h"
#	include "Interface/CodecInterface.h"
#	include "Interface/ResourceInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceModel3D );
	//////////////////////////////////////////////////////////////////////////
	ResourceModel3D::ResourceModel3D()
		: m_imageResource(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceModel3D::setDataflow( const ConstString & _dataflow )
	{
		m_dataflow = _dataflow;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceModel3D::getDataflow() const
	{
		return m_dataflow;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceModel3D::setConverter( const ConstString & _converter )
	{
		m_converter = _converter;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceModel3D::getConverter() const
	{
		return m_converter;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceModel3D::_loader( const Metabuf::Metadata * _meta )
	{
		const Metacode::Meta_DataBlock::Meta_ResourceModel3D * metadata 
			= static_cast<const Metacode::Meta_DataBlock::Meta_ResourceModel3D *>(_meta);

		metadata->swap_File_Path( m_path );
		metadata->swap_File_Dataflow( m_dataflow );
		metadata->swap_File_Converter( m_converter );

		metadata->swap_Image_Resource( m_imageResourceName );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceModel3D::_convert()
	{
		if( m_path.empty() == true )
		{
			return false;
		}

		if( m_converter.empty() == false )
		{
			if( CONVERTER_SERVICE(m_serviceProvider)
				->convert( m_converter, m_category, m_path, m_path ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("ResourceModel3D::_convert: '%s' can't convert '%s':'%s'"
					, this->getName().c_str() 
					, m_path.c_str()
					, m_converter.c_str()
					);

				return false;
			}
		}

		if( m_dataflow.empty() == true )
		{
			m_dataflow = CODEC_SERVICE(m_serviceProvider)
				->findCodecType( m_path );
		}

		if( m_dataflow.empty() == true )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceMovie::_convert: '%s' you must determine codec for file '%s'"
				, this->getName().c_str()
				, m_path.c_str()
				);

			return false;
		}

		return true;			 
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceModel3D::_compile()
	{
		if( m_imageResourceName.empty() == true )
		{
			return false;
		}

		const ConstString & category = this->getCategory();

		InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile(category, m_path);

		if( stream == nullptr )
		{
			return false;
		}

		m_model = DATA_SERVICE(m_serviceProvider)
			->dataflowT<Model3DInterfacePtr>( m_dataflow, stream );

		if( m_model == nullptr )
		{
			return false;
		}

		ResourceImage * resource  = RESOURCE_SERVICE(m_serviceProvider)
			->getResourceT<ResourceImage>( m_imageResourceName );

		if( resource == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("ResourceModel3D::_compile: '%s' not found image resource '%s'"
				, this->getName().c_str()
				, m_imageResourceName.c_str() 
				);

			return false;
		}

		m_imageResource = resource;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceModel3D::_release()
	{
		m_model = nullptr;

		if( m_imageResource != nullptr )
		{
			m_imageResource->decrementReference();
			m_imageResource = nullptr;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceModel3D::_isValid() const
	{
		const ConstString & category = this->getCategory();

		InputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
			->openInputFile(category, m_path);

		if( stream == nullptr )
		{
			return false;				 
		}

		Model3DInterfacePtr model = DATA_SERVICE(m_serviceProvider)
			->dataflowT<Model3DInterfacePtr>( m_dataflow, stream );

		if( model == nullptr )
		{
			return false;
		}

		return true;
	}
}