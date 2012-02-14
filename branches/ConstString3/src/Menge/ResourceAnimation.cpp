#	include "ResourceAnimation.h"
#	include "ResourceImplement.h"

#	include "ResourceImage.h"
#	include "ResourceSequence.h"

#	include "ResourceManager.h"

#	include "BinParser.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceAnimation );
	//////////////////////////////////////////////////////////////////////////
	ResourceAnimation::ResourceAnimation()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::setResourceImageName( const ConstString & _name )
	{
		m_resourceImageName = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceAnimation::getResourceImageName() const
	{
		return m_resourceImageName;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::setResourceSequenceName( const ConstString & _name )
	{
		m_resourceSequenceName = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceAnimation::getResourceSequenceName() const
	{
		return m_resourceSequenceName;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage * ResourceAnimation::getResourceImage() const
	{
		return m_resourceImage;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceSequence * ResourceAnimation::getResourceSequence() const
	{
		return m_resourceSequence;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::ResourceImage_Name, &ResourceAnimation::setResourceImageName );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::ResourceSequence_Name, &ResourceAnimation::setResourceSequenceName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceAnimation::_compile()
	{
		if( m_resourceImageName.empty() == true )
		{
			MENGE_LOG_ERROR( "ResourceAnimation: %s not set resource image"
				, this->getName().c_str()
				);

			return false;
		}

		if( m_resourceSequenceName.empty() == true )
		{
			MENGE_LOG_ERROR( "ResourceAnimation: %s not set resource sequence"
				, this->getName().c_str()
				);

			return false;
		}

		m_resourceImage = ResourceManager::get()
			->getResourceT<ResourceImage>( m_resourceImageName );

		if( m_resourceImage == NULL )
		{
			MENGE_LOG_ERROR( "ResourceAnimation: %s not find resource image %s"
				, this->getName().c_str()
				, m_resourceImageName.c_str()
				);

			return false;
		}

		m_resourceSequence = ResourceManager::get()
			->getResourceT<ResourceSequence>( m_resourceSequenceName );

		if( m_resourceSequence == NULL )
		{
			MENGE_LOG_ERROR( "ResourceAnimation: %s not find resource sequence %s"
				, this->getName().c_str()
				, m_resourceSequenceName.c_str()
				);
		
			m_resourceImage->decrementReference();
			m_resourceImage = 0;

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceAnimation::_release()
	{
		if( m_resourceImage != 0 )
		{
			m_resourceImage->decrementReference();
			m_resourceImage = NULL;
		}

		if( m_resourceSequence != 0 )
		{
			m_resourceSequence->decrementReference();
			m_resourceSequence = NULL;
		}
	}
}
