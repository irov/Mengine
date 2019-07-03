#	pragma once

#	include "Kernel/ResourceReference.h"
#	include "Kernel/ResourceImage.h"

#	include "stdex/stl_map.h"

#	ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning (disable:4510)
#	pragma warning (disable:4512)
#	pragma warning (disable:4610)
#	endif

#	include "spine/spine.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class ResourceSpine
		: public Resource
	{
		DECLARE_VISITABLE( ResourceReference );

	public:
		ResourceSpine();
		~ResourceSpine();

	public:
		spAtlas * getAtlas() const;
		spSkeletonData * getSkeletonData() const;

	public:
		spAnimation * findSkeletonAnimation( const ConstString & _name ) const;

	public:
		bool _loader( const Metabuf::Metadata * _meta ) override;

	protected:
		bool _compile() override;
		void _release() override;

	public:
		bool _isValid() const override;

	public:
		ResourceImagePtr getResourceImage_( const char * _name ) const;

	public:
		FilePath m_skeletonPath;
		FilePath m_atlasPath;
		
		struct ImageDesc
		{
			ConstString name;
			ResourceImagePtr image;
		};

		typedef stdex::vector<ImageDesc> TVectorImageDesc;

		TVectorImageDesc m_images;
		
		spAtlas * m_atlas;
		spSkeletonData * m_skeletonData;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceSpine> ResourceSpinePtr;
}
