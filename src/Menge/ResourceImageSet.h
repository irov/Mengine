#	pragma once

#	include "ResourceImageCell.h"

#	include <vector>

namespace Menge
{
	//! ResourceImageSet - ресурс-файл, который содержит список uv - областей. 

	/*! xml - файл имеет следующую структуру:
	* <Resource Name = "имя_ресурса" Type = "ResourceImageSet" >
	*	<Frame UV = "left;top;right;bottom"/>
	*    ...
	*   <Frame UV = "left;top;right;bottom"/>
	* </Resource>
	*/

	class ResourceImageSet
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageSet )

	public:
		ResourceImageSet( const ResourceFactoryParam & _params );

	public:
		void addFrameUV( const mt::vec4f & _uv );

	public:
		unsigned int getCount() const override;

		const mt::vec2f & getMaxSize( unsigned int _frame ) const override;
		const mt::vec2f & getSize( unsigned int _frame ) const override;
		const mt::vec2f & getOffset( unsigned int _frame ) const override;
		const mt::vec4f & getUV( unsigned int _frame ) const override;		
		bool isAlpha( unsigned int _frame ) const override;

		const RenderImageInterface * getImage( unsigned int _frame ) const override;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;

		ImageFrame m_imageFrame;

		typedef std::vector<mt::vec4f> TVectorUV;
		TVectorUV m_uvs;

		typedef std::vector<mt::vec2f> TVectorSizes;
		TVectorSizes m_sizes;

	private:
		struct ImageDesc
		{
			std::string fileName;
			mt::vec4f uv;
			mt::vec2f offset;
			mt::vec2f maxSize;
		};

		ImageDesc m_imageDesc;
	};
}