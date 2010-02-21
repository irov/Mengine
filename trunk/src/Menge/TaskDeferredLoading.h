/*
 *	TaskDeferredLoading.h
 *
 *	Created by _Berserk_ on 28.4.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	pragma once

#	include "Config/Typedef.h"
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
	class ImageDecoder;
	class FileInput;


	class ResourceManager;
	class RenderEngine;
	class DecoderManager;

	typedef std::vector<ResourceReference*> TResourceVector;

	class TaskDeferredLoading
		: public Task
	{
	public:
		TaskDeferredLoading( const TVectorString& _resourceFiles, PyObject* _progressCallback );
		~TaskDeferredLoading();

	public:
		void preMain() override;
		void main() override;
		void update() override;
		void postMain() override;
		void cancel() override;
		void cleanup() override;
		
		typedef std::map< String, TVectorString > TPackTexturesMap;

	protected:
		float m_oldProgress;
		float m_progress;
		TVectorString m_resourceFiles;
		PyObject* m_progressCallback;

		//TStringVector m_texturesList;
		TPackTexturesMap m_textures;


		TResourceVector m_imageResources;
		TResourceVector m_resources;

		struct TextureJob
		{
			String name;
			FileInput* file;
			ImageDecoder* decoder;
			Texture* texture;
			unsigned char* textureBuffer;
			int textureBufferPitch;
			int state;

			TextureJob::TextureJob()
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
		DecoderManager* m_decoderMgr;
	};
}	// namespace Menge
