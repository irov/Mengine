#	pragma once

/*
 *	TaskDeferredLoading.h
 *
 *	Created by _Berserk_ on 28.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "Core/String.h"

#	include "Task.h"

#	include <map>

extern "C"
{
	struct _object; 
	typedef _object PyObject;
}

namespace Menge
{
	class ResourceReference;
	class Texture;
	class ImageDecoderInterface;
	class FileInputInterface;


	class ResourceManager;
	class RenderEngine;
	class CodecEngine;
	class FileEngine;

	typedef std::vector<ResourceReference*> TResourceVector;

	class TaskDeferredLoading
		: public Task
	{
	public:
		TaskDeferredLoading( const ConstString & _category, const TVectorConstString& _resourceFiles, PyObject* _progressCallback );
		~TaskDeferredLoading();

	public:
		void preMain() override;
		void main() override;
		void update() override;
		void postMain() override;
		void cancel() override;
		void cleanup() override;
		
	protected:
		ConstString m_category;
		TVectorConstString m_resourceFiles;
		PyObject* m_progressCallback;

		float m_oldProgress;
		float m_progress;

		//TStringVector m_texturesList;
		TVectorConstString m_textures;


		TResourceVector m_imageResources;
		TResourceVector m_resources;

		struct TextureJob
		{
			ConstString name;
			String path;
			FileInputInterface* file;
			ImageDecoderInterface * decoder;
			Texture* texture;
			unsigned char* textureBuffer;
			int textureBufferPitch;
			int state;

			TextureJob()
				: state( 0 )
			{
			}
		};

		typedef std::vector<TextureJob> TTextureJobVector;
		TTextureJobVector m_textureJobs;

		float m_progressStep;
		bool m_lockDone;
		bool m_decodeDone;

		ResourceManager * m_resourceMgr;
		RenderEngine* m_renderEngine;
		FileEngine * m_fileEngine;
		CodecEngine * m_codecEngine;
	};
}	// namespace Menge
