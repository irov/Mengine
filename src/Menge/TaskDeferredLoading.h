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

	class TaskDeferredLoading
		: public Task
	{
	public:
		TaskDeferredLoading( const String& _resourceFile, PyObject* _progressCallback );
		~TaskDeferredLoading();

	public:
		void preMain() override;
		void main() override;
		void update() override;
		void postMain() override;
		
	protected:
		float m_oldProgress;
		float m_progress;
		String m_resourceFile;
		PyObject* m_progressCallback;

		TStringVector m_texturesList;
		typedef std::vector<ResourceReference*> TResourceVector;
		TResourceVector m_resources;

		struct TextureJob
		{
			Texture* texture;
			ImageDecoderInterface* decoder;
		};

		typedef std::vector<TextureJob> TTextureJobVector;
		TTextureJobVector m_textureJobs;
	};
}	// namespace Menge
