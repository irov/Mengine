
#	include "ResourceWindow.h"

#	include "ResourceImplement.h"
#	include "Texture.h"
#	include "RenderEngine.h"
#	include "ResourceImage.h"

#	include "BinParser.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT(ResourceWindow);
	//////////////////////////////////////////////////////////////////////////
	ResourceWindow::ResourceWindow()
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			m_images[i].image = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceWindow::~ResourceWindow()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceWindow::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Background_Image, m_images[0].path );
			BIN_CASE_ATTRIBUTE( Protocol::Background_Codec, m_images[0].codec );

			BIN_CASE_ATTRIBUTE( Protocol::LeftTop_Image, m_images[1].path );
			BIN_CASE_ATTRIBUTE( Protocol::LeftTop_Codec, m_images[1].codec );

			BIN_CASE_ATTRIBUTE( Protocol::Top_Image, m_images[2].path );
			BIN_CASE_ATTRIBUTE( Protocol::Top_Codec, m_images[2].codec );

			BIN_CASE_ATTRIBUTE( Protocol::RightTop_Image, m_images[3].path );
			BIN_CASE_ATTRIBUTE( Protocol::RightTop_Codec, m_images[3].codec );

			BIN_CASE_ATTRIBUTE( Protocol::Right_Image, m_images[4].path );
			BIN_CASE_ATTRIBUTE( Protocol::Right_Codec, m_images[4].codec );

			BIN_CASE_ATTRIBUTE( Protocol::RightBottom_Image, m_images[5].path );
			BIN_CASE_ATTRIBUTE( Protocol::RightBottom_Codec, m_images[5].codec );

			BIN_CASE_ATTRIBUTE( Protocol::Bottom_Image, m_images[6].path );
			BIN_CASE_ATTRIBUTE( Protocol::Bottom_Codec, m_images[6].codec );

			BIN_CASE_ATTRIBUTE( Protocol::LeftBottom_Image, m_images[7].path );
			BIN_CASE_ATTRIBUTE( Protocol::LeftBottom_Codec, m_images[7].codec );

			BIN_CASE_ATTRIBUTE( Protocol::Left_Image, m_images[8].path );
			BIN_CASE_ATTRIBUTE( Protocol::Left_Codec, m_images[8].codec );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceWindow::_compile()
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			if( m_images[i].path.empty() == true )
			{
				continue;
			}

			const ConstString & category = this->getCategory();

			if( m_images[i].codec.empty() == true )
			{
				m_images[i].codec
					= ResourceImage::s_getImageCodec( m_images[i].path );
			}

			m_images[i].image = RenderEngine::get()
				->loadTexture( category, m_images[i].path, m_images[i].codec );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceWindow::_release()
	{
		for( int i = 0; i < MAX_WINDOW_ELEMENTS; i++ )
		{
			if( m_images[i].image != NULL )
			{
				RenderEngine::get()
					->releaseTexture( m_images[i].image );

				m_images[i].image = NULL;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceWindow::getImage( int _type )
	{
		return m_images[_type].image;
	}
	//////////////////////////////////////////////////////////////////////////
}
