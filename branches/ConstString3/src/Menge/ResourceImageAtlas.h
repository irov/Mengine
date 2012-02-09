#	pragma once

#	include "ResourceImage.h"

#	include <vector>

namespace Menge
{
	//! ResourceImageDefault - ресурс-файл, который содержит список изображений. 

    /*! xml - файл имеет следующую структуру:
	* <Resource Name = "имя_ресурса" Type = "ResourceImageDefault" >
	*	<File Path = "имя_файла0"/>
	*    ...
	*   <File Path = "имя_файлаN"/>
	* </Resource>
	*/

	class ResourceImageAtlas
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageAtlas )

	public:
		ResourceImageAtlas();

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
		void _release() override;

	protected:

		typedef std::vector<ImageFrame> TVectorImageFrame;
		TVectorImageFrame m_vectorImageFrames;

	protected:
		
		struct ImageDesc
		{
			ConstString fileName;
			ConstString codecType;
			mt::vec4f uv;
			mt::vec2f offset;
			mt::vec2f maxSize;
			mt::vec2f size;
			bool isAlpha;
		};

		typedef std::vector<ImageDesc> TVectorImageDesc;
		TVectorImageDesc m_vectorImageDescs;

//	private:
	};
}
