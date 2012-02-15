
#	include "TaskLoadImageResources.h"
#	include "ResourceManager.h"
#	include "ScriptEngine.h"
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
	//////////////////////////////////////////////////////////////////////////
	TaskLoadImageResources::TaskLoadImageResources( const ConstString & _category, TVectorConstString _resourceNames, PyObject* _progressCallback )
		: m_category(_category)
		, m_resourceNames(_resourceNames)
		, m_callbackComplete(_progressCallback)
	{
		pybind::incref( m_callbackComplete );
	}
	//////////////////////////////////////////////////////////////////////////
	TaskLoadImageResources::~TaskLoadImageResources()
	{
		pybind::decref( m_callbackComplete );
	}
	//////////////////////////////////////////////////////////////////////////
	bool TaskLoadImageResources::_onRun()
	{
		for( TVectorConstString::const_iterator 
			it = m_resourceNames.begin(), 
			it_end = m_resourceNames.end();
		it != it_end;
		++it )
		{
			const ConstString & resourceName = *(it);
			ResourceImage * resource = dynamic_cast<ResourceImage*>( ResourceManager::get()->getResourceReference(resourceName) );
				
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
			
			size_t countFiles = resource->getFilenameCount();
			for( size_t i = 0; i < countFiles; ++i )
			{
				const ConstString & filename = resource->getFileName( i );

				if( filename == Consts::get()->c_CreateTexture || 
					filename == Consts::get()->c_CreateTarget || 
					RenderEngine::get()->hasTexture( filename ) == true )
				{
					continue;
				}
		     	
				TextureJob job;
				job.filename = filename;
				job.file = FileEngine::get()->createInputFile(m_category);
				m_textureJobs.push_back(job);
			}
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TaskLoadImageResources::_onMain()
	{
		for( TTextureJobVector::iterator 
			it = m_textureJobs.begin(), 
			it_end = m_textureJobs.end();
		it != it_end;
		++it )
		{
			TextureJob & job = (*it);
			
			if( job.file->open( Helper::to_str(job.filename) ) == false )
			{
				MENGE_LOG_ERROR( "Warning: Image file '%s' was not found"
					, job.filename.c_str()
					);

				return false;
			}

			job.decoder = CodecEngine::get()->createDecoderT<ImageDecoderInterface>( "pngImage", job.file );

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
			job.texture = RenderEngine::get()->createTexture( job.filename, dataInfo->width, dataInfo->height, dataInfo->format );
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
	void TaskLoadImageResources::_onComplete()
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
			//size_t count2 = resource->countReference(); 
			resource->incrementReference();
			//size_t count = resource->countReference();
		}
		
		if( m_callbackComplete != NULL )
		{
			pybind::call( m_callbackComplete, "(O)", pybind::get_bool(true) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadImageResources::_onJoin()
	{
		pybind::call( m_callbackComplete, "(O)", pybind::get_bool(false) );
	}	
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadImageResources::_onCancel()
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
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadImageResources::_onError()
	{
		for( TTextureJobVector::iterator 
			it = m_textureJobs.begin(), 
			it_end = m_textureJobs.end();
		it != it_end;
		++it )
		{
			TextureJob & job = (*it);
			if(job.file != NULL)
			{
				job.file->close();
				job.file = NULL;
			}

			if(job.decoder != NULL)
			{
				job.decoder->destroy();
				job.decoder = NULL;
			}

			if(job.texture != NULL)
			{
				job.texture->unlock();
				RenderEngine::get()->releaseTexture(job.texture);
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
