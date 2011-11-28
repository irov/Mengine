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

#	include "Interface/ImageCodecInterface.h"
#	include "LogEngine.h"
#	include "FileEngine.h"
#	include "CodecEngine.h"
#	include "Texture.h"
#	include "Consts.h"

#	include "Utils/Core/File.h"
#	include "Utils/Core/String.h"

namespace Menge
{
	const size_t s_maxLockSize = 1024*768*4;

	//////////////////////////////////////////////////////////////////////////
	class ResourceVisitorGetTexturesList
		: public ResourceVisitor
	{
	public:
		ResourceVisitorGetTexturesList( TVectorConstString& _textures, TVectorResources & _resources, TVectorResources & _imageResources, RenderEngine * _renderEngine )
			: m_textures(_textures)
			, m_resources(_resources)
			, m_imageResources(_imageResources)
			, m_renderEngine(_renderEngine)
		{
		}

		void visit(ResourceImageDefault* _resource)
		{
			m_imageResources.push_back( _resource );
			
			if( _resource->countReference() > 0 )
			{
				return;
			}

			for( size_t i = 0; i < _resource->getFilenameCount(); ++i )
			{
				const ConstString & filename = _resource->getFilename( i ) ;

				if( filename != Consts::get()->c_CreateTexture && 
					filename != Consts::get()->c_CreateTarget && 
					m_renderEngine->hasTexture( filename ) == false )
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
		TVectorConstString & m_textures;
		TVectorResources & m_resources;
		TVectorResources & m_imageResources;

		RenderEngine * m_renderEngine;
	};
	//////////////////////////////////////////////////////////////////////////
	TaskDeferredLoading::TaskDeferredLoading( const ConstString & _category, const TVectorConstString& _resourceFiles, PyObject* _progressCallback )
		: m_category(_category)
		, m_resourceFiles(_resourceFiles)
		, m_progressCallback(_progressCallback)
		, m_oldProgress(0.f)
		, m_progress(0.f)
		, m_progressStep(0.f)
		, m_lockDone(false)
		, m_decodeDone(true)
	{
		pybind::incref( m_progressCallback );

		m_resourceMgr = ResourceManager::get();
		m_renderEngine = RenderEngine::get();
		m_codecEngine = CodecEngine::get();
		m_fileEngine = FileEngine::get();
	}
	//////////////////////////////////////////////////////////////////////////
	TaskDeferredLoading::~TaskDeferredLoading()
	{
		pybind::decref( m_progressCallback );
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskDeferredLoading::preMain()
	{
		int resourcesCount = 0;

		for( TVectorConstString::iterator 
			it = m_resourceFiles.begin(), 
			it_end = m_resourceFiles.end();
		it != it_end;
		++it )
		{
			const ConstString & resourceFile = (*it);

			int numResources = m_resourceMgr->getResourceCount( m_category, resourceFile );
			resourcesCount += numResources;
		}
 
		m_textures.reserve( resourcesCount );
		m_resources.reserve( resourcesCount );
		m_imageResources.reserve( resourcesCount );

		for( TVectorConstString::iterator 
			it = m_resourceFiles.begin(), 
			it_end = m_resourceFiles.end();
			it != it_end;
			++it )
		{
			const ConstString& resourceFile = (*it);

			size_t count = m_resourceMgr->getResourceCount( m_category, resourceFile );
			if( count == 0 )
			{
				continue;
			}

			TVectorConstString & texturesList = m_textures;
			
			ResourceVisitorGetTexturesList visitor( texturesList, m_resources, m_imageResources, m_renderEngine );
			m_resourceMgr->visitResources( m_category, resourceFile, &visitor );
		}

		TVectorConstString& texturesList = m_textures;
		std::sort( m_textures.begin(), m_textures.end() );

		TVectorConstString::iterator it_u_end = std::unique( m_textures.begin(), m_textures.end() );
		m_textures.erase( it_u_end, m_textures.end() );

		int texturesNum = texturesList.size();

		if( texturesNum > 0 )
		{
			m_textureJobs.resize( texturesNum );
			m_progressStep = 1.0f / texturesNum * 0.5f;
		}

		FileEngine* fileEngine = FileEngine::get();

		TTextureJobVector::iterator it_jobs = m_textureJobs.begin();

		for( TVectorConstString::iterator 
			tit = m_textures.begin(), 
			tit_end = m_textures.end();
		tit != tit_end;
		++tit, ++it_jobs )
		{
			TextureJob& job = (*it_jobs);
			job.file = fileEngine->createInputFile( m_category );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskDeferredLoading::main()
	{
		TTextureJobVector::iterator it_jobs = m_textureJobs.begin();
		//for( TStringVector::iterator it = m_texturesList.begin(), it_end = m_texturesList.end();
		//	it != it_end;
		//	++it, ++it_jobs )

		for( TVectorConstString::iterator 
			tit = m_textures.begin(), 
			tit_end = m_textures.end();
		tit != tit_end;
		++tit, ++it_jobs )
		{
			TextureJob & job = (*it_jobs);

			const ConstString & filename = (*tit);

			//job.name = category.str() + filename.str();
			job.name = filename;

			if( job.file->open( Helper::to_str(filename) ) == false )
			{
				MENGE_LOG_ERROR( "Warning: Image file '%s' was not found"
					, filename.c_str()
					);

				job.state = 4;
				m_progress += m_progressStep * 2.0f;
				continue;
			}

			job.decoder = m_codecEngine->createDecoderT<ImageDecoderInterface>( Consts::get()->c_Image, job.file );

			if( job.decoder == NULL )
			{
				MENGE_LOG_ERROR( "Warning: Image decoder for file '%s' was not found"
					, filename.c_str() 
					);

				job.state = 4;
				m_progress += m_progressStep * 2.0f;
				continue;
			}

			job.state = 1;
			m_progress += m_progressStep;
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


		if( m_resources.empty() == false )
		{
			TVectorResources::iterator 
				it = m_resources.begin();

			TVectorResources::size_type size = (m_resources.size() / 10) + 1;

			while( --size )
			{
				(*it)->incrementReference();
				++it;
			}

			m_resources.erase( m_resources.begin(), it );
		}

		if( th_progress > m_oldProgress )
		{
			if( m_progressCallback != NULL
				&& pybind::is_none(m_progressCallback) == false )
			{
				pybind::call( m_progressCallback, "(fO)", th_progress, pybind::get_bool(false) );
			}

			m_oldProgress = th_progress;
		}

		bool lockDone = true;

		size_t bytesLocked = 0;
		for( TTextureJobVector::iterator
			it = m_textureJobs.begin(),
			it_end = m_textureJobs.end();
			it != it_end; 
			++it )
		{
			TextureJob & job = (*it);

			if( job.state == 0 )	// not read yet
			{
				lockDone = false;
				break;
			}
			else if( job.state == 1 )	// need to create texture and lock
			{
				const ImageCodecDataInfo* dataInfo = job.decoder->getCodecDataInfo();
				bytesLocked += dataInfo->size;
				//MENGE_LOG_INFO( "Create and lock texture %s", name.c_str() );
				job.texture = m_renderEngine->createTexture( job.name, dataInfo->width, dataInfo->height, dataInfo->format );
				if( job.texture == NULL )
				{
					job.state = 4;
					continue;
				}

				job.textureBuffer = job.texture->lock( &(job.textureBufferPitch), false );
				job.state = 2;
				if( bytesLocked > s_maxLockSize )
				{
					lockDone = false;
					break;
				}
			}
			else if( job.state == 3 )
			{
				job.decoder->destroy();

				job.file->close();

				job.texture->unlock();
				m_renderEngine->releaseTexture( job.texture );

				job.state = 5;
			}
		}

		if( lockDone == true )
		{
			m_lockDone = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskDeferredLoading::postMain()
	{
		for( TVectorResources::iterator 
			it = m_resources.begin(), it_end = m_resources.end();
			it != it_end;
		++it )
		{
			(*it)->incrementReference();
		}

 		for( TVectorResources::iterator 
 			it = m_imageResources.begin(), it_end = m_imageResources.end();
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
				job.decoder->destroy();

				job.file->close();

				job.texture->unlock();
				m_renderEngine->releaseTexture( job.texture );
			}
		}

		if( m_progressCallback != NULL
			&& pybind::string_check(m_progressCallback) == false )
		{
			pybind::call( m_progressCallback, "(fO)", 1.0f, pybind::get_bool(true) );
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
		for( TTextureJobVector::iterator 
			it = m_textureJobs.begin(), it_end = m_textureJobs.end();
			it != it_end;
		++it )
		{
			TextureJob& job = (*it);
			if( job.state == 1 )
			{
				job.decoder->destroy();
				job.file->close();
			}
			else if( job.state == 3 )
			{
				job.decoder->destroy();
				job.file->close();

				job.texture->unlock();
				m_renderEngine->releaseTexture( job.texture );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
