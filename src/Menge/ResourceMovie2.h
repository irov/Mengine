#	pragma once

#	include "Kernel/ResourceReference.h"

#	include "movie/movie.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ResourceMovie2
		: public ResourceReference
	{
		DECLARE_VISITABLE( ResourceReference );

	public:
		ResourceMovie2();
		~ResourceMovie2();

	public:
		aeMovieComposition * createComposition( const ConstString & _name );
		void destroyComposition( aeMovieComposition * _composition );

	protected:
		bool _initialize() override;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;

	public:
		ResourceReference * createResourceImage_( const aeMovieResourceImage * _resource );
		ResourceReference * createResourceVideo_( const aeMovieResourceVideo * _resource );

	protected:
		aeMovieInstance m_instance;

		aeMovieData * m_movieData;

        FilePath m_filePath;

		typedef stdex::vector<ResourceReferencePtr> TVectorResources;
		TVectorResources m_resources;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceMovie2> ResourceMovie2Ptr;
}