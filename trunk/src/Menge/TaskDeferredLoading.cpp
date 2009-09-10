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

#	include "DecoderManager.h"
#	include "Decoder.h"

#	include "ImageDecoder.h"

#	include "Interface/ImageCodecInterface.h"
#	include "LogEngine.h"
#	include "FileEngine.h"
#	include "Texture.h"
#	include "Utils.h"

namespace Menge
{
	const size_t s_maxLockSize = 1024*768*4;

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
					m_textures.push_back( filename );
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
		, m_progressStep( 0.0f )
		, m_lockDone( false )
		, m_decodeDone( true )
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
		ResourceManager* resManager = ResourceManager::hostage();
		RenderEngine* renderEngine = RenderEngine::hostage();
		
		typedef std::map< String, int > TPackResourceMap;
		TPackResourceMap resourcePackMap;
		int allResourcesCount = 0;
		for( TStringVector::iterator it = m_resourceFiles.begin(), it_end = m_resourceFiles.end();
			it != it_end;
			++it )
		{
			String& resourceFile = (*it);
			const String& category = resManager->getCategoryResource( resourceFile );
			if( category == Utils::emptyString() )
			{
				MENGE_LOG_ERROR( "Error: (TaskDeferredLoading::preMain) invalid resource file \"%s\"",
					resourceFile.c_str() );
				continue;
			}
			int numResources = resManager->getResourceCount( resourceFile );
			allResourcesCount += numResources;
			TPackResourceMap::iterator it_find = resourcePackMap.find( category );
			if( it_find == resourcePackMap.end() )
			{
				resourcePackMap.insert( std::make_pair( category, numResources ) );
			}
			else
			{
				it_find->second += numResources;
			}
		}

		for( TPackResourceMap::iterator it = resourcePackMap.begin(), it_end = resourcePackMap.end();
			it != it_end;
			++it )
		{
			std::pair< TPackTexturesMap::iterator, bool > res = 
				m_textures.insert( std::make_pair( it->first, TStringVector() ) );
			res.first->second.reserve( it->second );
		}
		m_resources.reserve( allResourcesCount );
		for( TStringVector::iterator it = m_resourceFiles.begin(), it_end = m_resourceFiles.end();
			it != it_end;
			++it )
		{
			String& resourceFile = (*it);
			size_t count = resManager->getResourceCount( resourceFile );
			if( count == 0 )
			{
				continue;
			}

			const String& category = resManager->getCategoryResource( resourceFile );
			if( category == Utils::emptyString() )
			{
				continue;
			}
			TStringVector& texturesList = m_textures[category];
			//m_texturesList.clear();
			ResourceVisitorGetTexturesList visitor( texturesList, m_resources, renderEngine );
			resManager->visitResources( &visitor, resourceFile );
		}

		int texturesNum = 0;
		for( TPackTexturesMap::iterator it = m_textures.begin(), it_end = m_textures.end();
			it != it_end;
			++it )
		{
			TStringVector& texturesList = it->second;
			std::sort( texturesList.begin(), texturesList.end() );
			TStringVector::iterator it_u_end = std::unique( texturesList.begin(), texturesList.end() );
			texturesList.erase( it_u_end, texturesList.end() );
			texturesNum += texturesList.size();
		}

		if( texturesNum > 0 )
		{
			m_textureJobs.resize( texturesNum );
			m_progressStep = 1.0f / texturesNum * 0.5f;
		}

		FileEngine* fileEngine = FileEngine::hostage();

		TTextureJobVector::iterator it_jobs = m_textureJobs.begin();
		for( TPackTexturesMap::iterator it = m_textures.begin(), it_end = m_textures.end();
			it != it_end;
			++it )
		{
			TStringVector& texturesList = it->second;
			const String& category = it->first;
			for( TStringVector::iterator tit = texturesList.begin(), tit_end = texturesList.end();
				tit != tit_end;
				++tit, ++it_jobs )
			{
				TextureJob& job = (*it_jobs);
				job.file = fileEngine->createFileInput( category );
			}
		}

		m_itUpdateJob = m_textureJobs.begin();
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskDeferredLoading::main()
	{
		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
		DecoderManager* decoderMgr = Holder<DecoderManager>::hostage();

		TTextureJobVector::iterator it_jobs = m_textureJobs.begin();
		//for( TStringVector::iterator it = m_texturesList.begin(), it_end = m_texturesList.end();
		//	it != it_end;
		//	++it, ++it_jobs )
		for( TPackTexturesMap::iterator it = m_textures.begin(), it_end = m_textures.end();
			it != it_end;
			++it )
		{
			TStringVector& texturesList = it->second;
			const String& category = it->first;
			for( TStringVector::iterator tit = texturesList.begin(), tit_end = texturesList.end();
				tit != tit_end;
				++tit, ++it_jobs )
			{
			TextureJob& job = (*it_jobs);
			String& filename = (*tit);
			job.name = category + filename;
			job.decoder = decoderMgr->createDecoderT<ImageDecoder>( filename, "Image", job.file );
			if( job.decoder == NULL )
			{
				MENGE_LOG_ERROR( "Warning: Image decoder for file \"%s\" was not found"
					, filename.c_str() );
				job.state = 4;
				m_progress += m_progressStep * 2.0f;
				continue;
			}

			const ImageCodecDataInfo* dataInfo = static_cast<const ImageCodecDataInfo*>( job.decoder->getCodecDataInfo() );
			if( dataInfo->format == PF_UNKNOWN )
			{
				MENGE_LOG_ERROR( "Error: Invalid image format \"%s\"",
					filename.c_str() );

				decoderMgr->releaseDecoder( job.decoder );
				job.state = 4;
				m_progress += m_progressStep * 2.0f;
				continue;
			}

			job.state = 1;
			m_progress += m_progressStep;
			}
		}

		bool alldone = false;
		while( m_lockDone == false || alldone == false )
		{
			alldone = m_lockDone;	// +1 iteration
			for( TTextureJobVector::iterator it = m_textureJobs.begin(), it_end = m_textureJobs.end();
				it != it_end;
				++it )
			{
				TextureJob& job = (*it);
				if( job.state == 2 )
				{
					job.texture->loadImageData( job.textureBuffer, job.textureBufferPitch, job.decoder );
					job.state = 3;
					m_progress += m_progressStep;
				}
			}
			//m_decodeDone = true;
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
				pybind::call( m_progressCallback, "(fb)", th_progress, false );
			}

			m_oldProgress = th_progress;
		}
		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();

		size_t bytesLocked = 0;
		while( m_itUpdateJob != m_textureJobs.end()/* && m_decodeDone == true*/ )
		{
			TextureJob& job = (*m_itUpdateJob);
			if( job.state == 0 )	// not read yet
			{
				break;
			}
			else if( job.state == 1 )	// need to create texture and lock
			{
				const ImageCodecDataInfo* dataInfo = 
					static_cast<const ImageCodecDataInfo*>( job.decoder->getCodecDataInfo() );
				bytesLocked += dataInfo->size;
				//MENGE_LOG( "Create and lock texture %s", name.c_str() );
				job.texture = renderEngine->createTexture( job.name, dataInfo->width, dataInfo->height, dataInfo->format );
				if( job.texture == NULL )
				{
					job.state = 4;
					continue;
				}

				job.textureBuffer = job.texture->lock( &(job.textureBufferPitch), false );
				job.state = 2;
				if( bytesLocked > s_maxLockSize )
				{
					++m_itUpdateJob;
					break;
				}
			}
			++m_itUpdateJob;
		}
		//m_decodeDone = false;
	
		if( m_itUpdateJob == m_textureJobs.end() )
		{
			m_lockDone = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskDeferredLoading::postMain()
	{
		ResourceManager * resourceMgr = Holder<ResourceManager>::hostage();
		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
		DecoderManager* decoderMgr = Holder<DecoderManager>::hostage();

 		for( TResourceVector::iterator 
 			it = m_resources.begin(), it_end = m_resources.end();
 			it != it_end;
 			++it )
 		{
 			(*it)->incrementReference();
 		}

		for( TTextureJobVector::iterator 
			it = m_textureJobs.begin(), it_end = m_textureJobs.end();
			it != it_end;
			++it )
		{
			TextureJob& job = (*it);
			if( job.state == 3 )
			{
				decoderMgr->releaseDecoder( job.decoder );

				job.texture->unlock();
				renderEngine->releaseTexture( job.texture );
			}
		}

		if( m_progressCallback != NULL
			&& m_progressCallback != Py_None )
		{
			pybind::call( m_progressCallback, "(fb)", 1.0f, true );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskDeferredLoading::cancel()
	{
		for( TTextureJobVector::iterator it = m_textureJobs.begin(), it_end = m_textureJobs.end();
			it != it_end;
			++it )
		{
			TextureJob& job = (*it);
			if( job.state == 2 )
			{
				job.state = 3;
			}
		}
		m_lockDone = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskDeferredLoading::cleanup()
	{
		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
		DecoderManager* decoderMgr = Holder<DecoderManager>::hostage();

		for( TTextureJobVector::iterator 
			it = m_textureJobs.begin(), it_end = m_textureJobs.end();
			it != it_end;
		++it )
		{
			TextureJob& job = (*it);
			if( job.state == 1 )
			{
				decoderMgr->releaseDecoder( job.decoder );
			}
			else if( job.state == 3 )
			{
				decoderMgr->releaseDecoder( job.decoder );

				job.texture->unlock();
				renderEngine->releaseTexture( job.texture );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
