
#	include "ThreadTaskLoadImageResources.h"
#	include "ResourceManager.h"
#	include "ScriptEngine.h"
#	include "ResourceImageDefault.h"
#	include "RenderEngine.h"

#	include "Interface/ImageCodecInterface.h"
#	include "LogEngine.h"
#	include "FileEngine.h"
#	include "CodecEngine.h"
#	include "Consts.h"

#	include "Utils/Core/File.h"
#	include "Utils/Core/String.h"

namespace Menge
{	
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskLoadImageResources::ThreadTaskLoadImageResources( const ConstString & _category, TVectorConstString _resourceNames, PyObject* _progressCallback )
		: m_category(_category)
		, m_resourceNames(_resourceNames)
		, m_callbackComplete(_progressCallback)
	{
		pybind::incref( m_callbackComplete );
	}
	//////////////////////////////////////////////////////////////////////////
	ThreadTaskLoadImageResources::~ThreadTaskLoadImageResources()
	{
		pybind::decref( m_callbackComplete );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskLoadImageResources::_onRun()
	{
		
		for( TVectorConstString::const_iterator 
			it = m_resourceNames.begin(), 
			it_end = m_resourceNames.end();
		it != it_end;
		++it )
		{
			const ConstString & resourceName = *(it);
			ResourceImage * resource = dynamic_cast<ResourceImage*>( ResourceManager::get()->getResourceReference(resourceName) );
			
			printf("_________________Thread work %s \n", resourceName.c_str());
			
			if( resource == 0 )
			{
				MENGE_LOG_ERROR( "TaskLoadImageResources:  Resource Image '%s' was not found"
					, resourceName.c_str()
					);

				return false;
			}
			
			ResourceManager::get()->lockResource( resourceName );
			m_resources.push_back(resource);

			size_t countReferences = resource->countReference();
			if( countReferences > 0 )
			{
				continue;
			}
						
			const WString & filename = resource->getFileName();

			if( RenderEngine::get()->hasTexture( filename ) == true )
			{
				continue;
			}

			const ConstString & name = resource->getName();
		     	
			TextureJob job;
			job.name = name;
			job.filename = filename;
			job.file = FileEngine::get()->createInputFile(m_category);
			job.decoder = NULL;
			job.texture = NULL;
			job.textureBuffer = NULL;
			job.textureBufferPitch = 0;
				
			m_textureJobs.push_back(job);
			
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ThreadTaskLoadImageResources::_onMain()
	{
		for( TTextureJobVector::iterator 
			it = m_textureJobs.begin(), 
			it_end = m_textureJobs.end();
		it != it_end;
		++it )
		{
			if( isInterrupt() == true )
			{
				return false;
			}

			TextureJob & job = (*it);
			
			if( job.file->open( job.filename ) == false )
			{
				MENGE_LOG_ERROR( "Warning: Image file '%s' was not found"
					, job.filename.c_str()
					);

				return false;
			}

			job.decoder = CodecEngine::get()->createDecoderT<ImageDecoderInterface>( ConstString("pngImage"), job.file );

			if( job.decoder == NULL )
			{
				MENGE_LOG_ERROR( "Warning: Image decoder for file '%s' was not found"
					, job.filename.c_str() 
					);

				return false;
			}

			const ImageCodecDataInfo* dataInfo = job.decoder->getCodecDataInfo();

			if( dataInfo->format == PF_UNKNOWN )
			{
				MENGE_LOG_ERROR( "Error: Invalid image format '%s'"
					,job.filename.c_str() 
					);

				return false;
			}
			//const mt::vec2f & texture_size = m_resource->getMaxSize( i );
			job.texture = RenderEngine::get()
				->createTexture( dataInfo->width, dataInfo->height, dataInfo->format );
			
			RenderEngine::get()
				->cacheFileTexture( job.filename, job.texture );

			if( job.texture == NULL )
			{
				return false;
			}

			job.textureBuffer = job.texture->lock( &(job.textureBufferPitch), false );

			job.texture->loadImageData( job.textureBuffer, job.textureBufferPitch, job.decoder );

			job.decoder->destroy();
			job.decoder = NULL;

			job.file->close();
			job.file = NULL;

			job.texture->unlock();
			job.texture = NULL;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskLoadImageResources::_onComplete()
	{		
		for( TVectorResourceImages::iterator 
			it = m_resources.begin(), 
			it_end = m_resources.end();
		it != it_end;
		++it )
		{
			ResourceImage * resource = *(it);
			const ConstString& resourceName = resource->getName();
			ResourceManager::get()->unlockResource( resourceName );
			resource->incrementReference();
		}
		
		if( m_callbackComplete != NULL )
		{
			pybind::call( m_callbackComplete, "(O)", pybind::get_bool(true) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskLoadImageResources::_onCancel()
	{
		for( TVectorResourceImages::iterator 
			it = m_resources.begin(), 
			it_end = m_resources.end();
		it != it_end;
		++it )
		{
			ResourceImage * resource = *(it);
			const ConstString& resourceName = resource->getName();
			ResourceManager::get()->unlockResource( resourceName );
		}

		pybind::call( m_callbackComplete, "(O)", pybind::get_bool(false) );
	}
	//////////////////////////////////////////////////////////////////////////
	void ThreadTaskLoadImageResources::_onInterrupt()
	{
		for( TTextureJobVector::iterator 
			it = m_textureJobs.begin(), 
			it_end = m_textureJobs.end();
		it != it_end;
		++it )
		{
			TextureJob & job = (*it);
			if( job.file != NULL )
			{
				job.file->close();
				job.file = NULL;
			}

			if( job.decoder != NULL )
			{
				job.decoder->destroy();
				job.decoder = NULL;
			}

			if( job.texture != NULL )
			{
				job.texture->unlock();
				RenderEngine::get()->releaseTexture( job.texture );

				job.texture = NULL;
			}
		}

		for( TVectorResourceImages::iterator 
			it = m_resources.begin(), 
			it_end = m_resources.end();
		it != it_end;
		++it )
		{
			ResourceImage * resource = *(it);
			const ConstString& resourceName = resource->getName();
			ResourceManager::get()->unlockResource( resourceName );
		}

	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
