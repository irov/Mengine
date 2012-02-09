
#	include "TaskLoadResourceImage.h"
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
	TaskLoadResourceImage::TaskLoadResourceImage( const ConstString & _category, const ConstString& _resourceFile, PyObject* _progressCallback )
		: m_category(_category)
		, m_resourceName(_resourceFile)
		, m_callbackComplete(_progressCallback)
	{
		pybind::incref( m_callbackComplete );
	}
	//////////////////////////////////////////////////////////////////////////
	TaskLoadResourceImage::~TaskLoadResourceImage()
	{
		pybind::decref( m_callbackComplete );
	}
	//////////////////////////////////////////////////////////////////////////
	bool TaskLoadResourceImage::_onRun()
	{
		m_resource = dynamic_cast<ResourceImage*>(m_resourceMgr->getResourceReference(m_resourceName));

		if( m_resource == 0 )
		{
			//error
			MENGE_LOG_ERROR( "TaskLoadResourceImage:  Resource Image '%s' was not found"
				, m_resourceName.c_str()
				);

			return false;
		}
		
		size_t countFiles = m_resource->getFilenameCount();
			
		TTextureJobVector::iterator it_jobs = m_textureJobs.begin();
		
		for( size_t i = 0; i < countFiles; ++i )
		{
			const ConstString & filename = m_resource->getFilename( i );

			if( filename == Consts::get()->c_CreateTexture || 
				filename == Consts::get()->c_CreateTarget || 
				m_renderEngine->hasTexture( filename ) == true )
			{
				continue;
			}
			
			TextureJob job;
			job.filename = filename;
			job.file = m_fileEngine->createInputFile(m_category);
						
			const mt::vec2f & texture_size = m_resource->getMaxSize( i );

			job.texture = m_renderEngine->createTexture( job.filename, texture_size.x, texture_size.y, dataInfo->format );
			if( job.texture == NULL )
			{
				job.file->close();
				
				return false;
			}

			job.textureBuffer = job.texture->lock( &(job.textureBufferPitch), false );

			m_textureJobs.push_back(job);
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TaskLoadResourceImage::_onMain()
	{
		for( TTextureJobVector::iterator 
			it = m_textureJobs.begin(), 
			it_end = m_textureJobs.end();
		it != it_end;
		++it )
		{
			TextureJob & job = (*it);

			if( job.file->open( Helper::to_str(filename) ) == false )
			{
				MENGE_LOG_ERROR( "Warning: Image file '%s' was not found"
					, filename.c_str()
					);

				return false;
			}

			job.decoder = m_codecEngine->createDecoderT<ImageDecoderInterface>( Consts::get()->c_Image, job.file );

			if( job.decoder == NULL )
			{
				MENGE_LOG_ERROR( "Warning: Image decoder for file '%s' was not found"
					, filename.c_str() 
					);

				job.file->close();

				return false;
			}
			
			job.texture->loadImageData( job.textureBuffer, job.textureBufferPitch, job.decoder );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadResourceImage::_onComplete()
	{		
		m_resource->incrementReference();

		if( m_callbackComplete != NULL )
		{
			pybind::call( m_callbackComplete, "(O)", pybind::get_bool(true) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void TaskLoadResourceImage::_onJoin()
	{
	}	
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
