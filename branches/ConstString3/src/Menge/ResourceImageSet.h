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
		size_t getCount() const override;

		const mt::vec2f & getMaxSize( size_t _frame ) const override;
		const mt::vec2f & getSize( size_t _frame ) const override;
		const mt::vec2f & getOffset( size_t _frame ) const override;
		const mt::vec4f & getUV( size_t _frame ) const override;		
		const mt::vec4f & getUVImage( size_t _frame ) const override;

		bool isAlpha( size_t _frame ) const override;

		Texture* getTexture( size_t _frame ) const override;

		const ConstString & getFileName( size_t _frame ) const override;
		const ConstString & getCodecType( size_t _frame ) const override;
		size_t getFilenameCount() const override;

	public:
		void loader( BinParser * _parser ) override;

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
			ConstString fileName;
			ConstString codecType;
			mt::vec4f uv;
			mt::vec2f offset;
			mt::vec2f maxSize;
		};

		ImageDesc m_imageDesc;
	};
}
