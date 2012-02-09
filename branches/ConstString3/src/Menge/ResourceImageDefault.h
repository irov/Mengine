#	pragma once

#	include "ResourceImage.h"

#	include "Config/Typedef.h"

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

	class ResourceImageDefault
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageDefault )

	public:
		ResourceImageDefault();

	public:
		size_t getCount() const override;

		const mt::vec2f & getMaxSize( size_t _frame ) const override;
		const mt::vec2f & getSize( size_t _frame ) const override;
		const mt::vec2f & getOffset( size_t _frame ) const override;
		const mt::vec4f & getUV( size_t _frame ) const override;
		const mt::vec4f & getUVImage( size_t _frame ) const override;

		bool isAlpha( size_t _frame ) const override;
		bool getWrapX( size_t _frame ) const override;
		bool getWrapY( size_t _frame ) const override;

		Texture * getTexture( size_t _frame ) const override;
		void addImagePath( const ConstString& _imagePath, const mt::vec2f & _size );

		const ConstString & getFileName( size_t _frame ) const override;
		const ConstString & getCodecType( size_t _frame ) const override;

		size_t getFilenameCount() const override;

	public:
		inline const ResourceImage::ImageFrame & getImageFrame( size_t _frame ) const;

	protected:
		bool isValid() const override;

	protected:
		void loader( BinParser * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;
        		
	protected:
		//unsigned int m_filter;

		typedef std::vector<ResourceImage::ImageFrame> TVectorImageFrame;
		TVectorImageFrame m_vectorImageFrames;

	private:
		struct ImageDesc
		{
			ConstString fileName;
			ConstString fileNameAlpha;
			ConstString fileNameRGB;
			ConstString codecType;
			ConstString codecTypeAlpha;
			ConstString codecTypeRGB;
			mt::vec4f uv;
			mt::vec2f offset;
			mt::vec2f maxSize;
			mt::vec2f size;
			bool isAlpha;
			bool isCombined;
			bool wrapX;
			bool wrapY;
		};

		typedef std::vector<ImageDesc> TVectorImageDesc;
		TVectorImageDesc m_vectorImageDescs;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const ResourceImage::ImageFrame & ResourceImageDefault::getImageFrame( size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ];
	}
}
