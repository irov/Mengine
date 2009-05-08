/*
 *	TaskDeferredLoading.cpp
 *
 *	Created by _Berserk_ on 28.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "TaskDeferredLoading.h"
#	include "ResourceManager.h"
#	include "ScriptEngine.h"
#	include "ResourceVisitor.h"
#	include "ResourceImageDefault.h"
#	include "RenderEngine.h"
#	include "Codec.h"
#	include "Interface/ImageCodecInterface.h"
#	include "LogEngine.h"
#	include "Texture.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ResourceVisitorGetTexturesList
		: public ResourceVisitor
	{
	public:
		ResourceVisitorGetTexturesList( TStringVector& _textures, std::vector<ResourceReference*>& _resources, RenderEngine* _renderEngine )
			: m_textures( _textures )
			, m_resources( _resources )
			, m_renderEngine( _renderEngine )
		{
		}
		void visit(ResourceImageDefault* _resource)
		{
			visit( (ResourceReference*)(_resource) );
			if( _resource->countReference() > 0 )
			{
				return;
			}
			for( size_t i = 0; i < _resource->getFilenameCount(); ++i )
			{
				const String& filename = _resource->getFilename( i ) ;
				if( filename != "CreateTexture"
					&& filename != "CreateTarget" 
					&& m_renderEngine->hasTexture( filename ) == false )
				{
					m_textures.push_back( _resource->getFactoryParams().category + filename );
				}
			}
		}
		void visit(ResourceReference* _resource)
		{
			m_resources.push_back( _resource );
		}

	protected:
		RenderEngine* m_renderEngine;
		TStringVector& m_textures;
		typedef std::vector<ResourceReference*> TResourceVector;
		TResourceVector& m_resources;
	};
	//////////////////////////////////////////////////////////////////////////
	TaskDeferredLoading::TaskDeferredLoading( const TStringVector& _resourceFiles, PyObject* _progressCallback )
		: m_oldProgress( 0.0f )
		, m_progress( 0.0f )
		, m_resourceFiles( _resourceFiles )
		, m_progressCallback( _progressCallback )
	{
		pybind::incref( m_progressCallback );
	}
	//////////////////////////////////////////////////////////////////////////
	TaskDeferredLoading::~TaskDeferredLoading()
	{
		pybind::decref( m_progressCallback );
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskDeferredLoading::preMain()
	{
		ResourceManager* resManager = Holder<ResourceManager>::hostage();
		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
		DecoderManager* decoderMgr = Holder<DecoderManager>::hostage();
		for( TStringVector::iterator it = m_resourceFiles.begin(), it_end = m_resourceFiles.end();
			it != it_end;
			++it )
		{
			String& resourceFile = (*it);
			size_t numResources = resManager->getResourceCount( resourceFile );
			if( numResources == 0 )
			{
				continue;
			}

			m_texturesList.reserve( numResources );
			m_resources.reserve( numResources );

			ResourceVisitorGetTexturesList visitor( m_texturesList, m_resources, renderEngine );
			resManager->visitResources( &visitor, resourceFile );

			m_textureJobs.reserve( m_texturesList.size() );
			for( TStringVector::iterator it = m_texturesList.begin(), it_end = m_texturesList.end();
				it != it_end;
				++it )
			{
				String& filename = (*it);
				ImageDecoderInterface* imageDecoder = decoderMgr->createDecoderT<ImageDecoderInterface>( filename, "Image" );

				if( imageDecoder == 0 )
				{
					MENGE_LOG_ERROR( "Warning: Image decoder for file \"%s\" was not found"
						, filename.c_str() );

					continue;
				}

				const ImageCodecDataInfo* dataInfo = static_cast<const ImageCodecDataInfo*>( imageDecoder->getCodecDataInfo() );
				if( dataInfo->format == PF_UNKNOWN )
				{
					MENGE_LOG_ERROR( "Error: Invalid image format \"%s\"",
						filename.c_str() );

					decoderMgr->releaseDecoder( imageDecoder );

					continue;
				}

				Texture* texture = renderEngine->createTexture( filename, dataInfo->width, dataInfo->height, dataInfo->format );
				if( texture == NULL )
				{
					decoderMgr->releaseDecoder( imageDecoder );
					continue;
				}

				TextureJob job = { texture, imageDecoder };
				m_textureJobs.push_back( job );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskDeferredLoading::main()
	{
		if( m_textureJobs.empty() == true )
		{
			m_progress = 1.0f;
			m_complete = true;
			return;
		}
		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();

		float progressStep = 1.0f / m_texturesList.size();
		for( TTextureJobVector::iterator it = m_textureJobs.begin(), it_end = m_textureJobs.end();
			it != it_end;
			++it )
		{
			TextureJob& job = (*it);
			job.texture->loadImageData( job.decoder );
			m_progress += progressStep;
		}

		m_progress = 1.0f;
		m_complete = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskDeferredLoading::update()
	{
		float th_progress = m_progress;

		if( th_progress > m_oldProgress )
		{
			if( m_progressCallback != NULL
				&& m_progressCallback != Py_None )
			{
				pybind::call( m_progressCallback, "(fb)", th_progress, m_complete );
			}

			m_oldProgress = th_progress;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskDeferredLoading::postMain()
	{
		for( TResourceVector::iterator it = m_resources.begin(), it_end = m_resources.end();
			it != it_end;
			++it )
		{
			(*it)->incrementReference();
		}

		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
		for( TTextureJobVector::iterator it = m_textureJobs.begin(), it_end = m_textureJobs.end();
			it != it_end;
			++it )
		{
			renderEngine->releaseTexture( (*it).texture );
		}

		if( m_progressCallback != NULL
			&& m_progressCallback != Py_None )
		{
			pybind::call( m_progressCallback, "(fb)", 1.0f, m_complete );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
