///*
// *	ResourceTexture.cpp
// *
// *	Created by _Berserk_ on 9.2.2009
// *	Copyright 2009 Menge. All rights reserved.
// *
// */
//
//#	include "ResourceTexture.h"
//#	include "ResourceImplement.h"
//
//#	include "XmlEngine.h"
//#	include "RenderEngine.h"
//
//namespace Menge
//{
//	//////////////////////////////////////////////////////////////////////////
//	RESOURCE_IMPLEMENT( ResourceTexture );
//	//////////////////////////////////////////////////////////////////////////
//	ResourceTexture::ResourceTexture( const ResourceFactoryParam& _params )
//		: ResourceReference( _params )
//		, m_renderTexture( NULL )
//	{
//
//	}
//	//////////////////////////////////////////////////////////////////////////
//	ResourceTexture::~ResourceTexture()
//	{
//
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceTexture::loader( XmlElement * _xml )
//	{
//		ResourceReference::loader( _xml );
//
//		XML_SWITCH_NODE( _xml )
//		{
//			XML_CASE_NODE( "File" )
//			{
//				XML_FOR_EACH_ATTRIBUTES()
//				{
//					XML_CASE_ATTRIBUTE( "Path", m_filename );
//				}
//			}
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool ResourceTexture::_compile()
//	{
//		if( m_filename.empty() == false )
//		{
//			m_renderTexture = Holder<RenderEngine>::hostage()
//								->loadImage( m_params.category + m_filename );
//		}
//
//		return true;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void ResourceTexture::_release()
//	{
//		if( m_renderTexture != NULL )
//		{
//			Holder<RenderEngine>::hostage()
//				->releaseImage( m_renderTexture );
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	RenderImageInterface* ResourceTexture::getRenderTexture()
//	{
//		return m_renderTexture;
//	}
//	//////////////////////////////////////////////////////////////////////////
//}	// namespace Menge
