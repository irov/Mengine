#pragma once

#include "SpineInterface.h"

#include "Kernel/Resource.h"
#include "Kernel/Content.h"
#include "Kernel/ResourceImage.h"

#include "spine/spine.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class ResourceSpineAtlas
		: public Resource
        , public Content
	{
        DECLARE_VISITABLE( Resource );
        DECLARE_CONTENTABLE();

	public:
        ResourceSpineAtlas();
        ~ResourceSpineAtlas() override;

    public:
        void addResourceImageDesc( const ConstString & _name, const ConstString & _resourceImageName );

    protected:
        const ResourceImagePtr & getResourceImage_( const Char * _name ) const;

	public:
		spAtlas * getAtlas() const;

	protected:
		bool _compile() override;
		void _release() override;

    protected:
        struct ImageDesc
        {
            ConstString name;
            ConstString resourceImageName;
            ResourceImagePtr resourceImage;
        };

        typedef Vector<ImageDesc> VectorImageDesc;
        VectorImageDesc m_images;

		spAtlas * m_atlas;        

        friend const ResourceImagePtr & _spAtlasPage_createTexture2( const ResourceSpineAtlas *, const char * );
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceSpineAtlas> ResourceSpineAtlasPtr;
    //////////////////////////////////////////////////////////////////////////
}
