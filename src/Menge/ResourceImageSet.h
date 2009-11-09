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
		ResourceImageSet();

	public:
		void addFrameUV( const mt::vec4f & _uv );

	public:
		std::size_t getCount() const override;

		const mt::vec2f & getMaxSize( std::size_t _frame ) const override;
		const mt::vec2f & getSize( std::size_t _frame ) const override;
		const mt::vec2f & getOffset( std::size_t _frame ) const override;
		const mt::vec4f & getUV( std::size_t _frame ) const override;		
		bool isAlpha( std::size_t _frame ) const override;

		Texture* getTexture( std::size_t _frame ) override;

		const String & getFilename( std::size_t _frame ) const override;
		std::size_t getFilenameCount() const override;

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
			String fileName;
			mt::vec4f uv;
			mt::vec2f offset;
			mt::vec2f maxSize;
		};

		ImageDesc m_imageDesc;
	};
}
