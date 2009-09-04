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

	class TaskDeferredLoading
		: public Task
	{
	public:
		TaskDeferredLoading( const TStringVector& _resourceFiles, PyObject* _progressCallback );
		~TaskDeferredLoading();

	public:
		void preMain() override;
		void main() override;
		void update() override;
		void postMain() override;
		void cancel() override;
		void cleanup() override;
		
		typedef std::map< String, TStringVector > TPackTexturesMap;
	protected:
		float m_oldProgress;
		float m_progress;
		TStringVector m_resourceFiles;
		PyObject* m_progressCallback;

		//TStringVector m_texturesList;
		TPackTexturesMap m_textures;

		typedef std::vector<ResourceReference*> TResourceVector;
		TResourceVector m_resources;

		struct TextureJob
		{
			String name;
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
		TTextureJobVector::iterator m_itUpdateJob;
		TStringVector::iterator m_itNames;
		float m_progressStep;
		bool m_lockDone;
		bool m_decodeDone;
	};
}	// namespace Menge
