#	pragma once

#	include "ResourceImage.h"

#	include "Config/Typedef.h"

#	include <vector>

namespace Menge
{
	//! ResourceImageDefault - ������-����, ������� �������� ������ �����������. 

    /*! xml - ���� ����� ��������� ���������:
	* <Resource Name = "���_�������" Type = "ResourceImageDefault" >
	*	<File Path = "���_�����0"/>
	*    ...
	*   <File Path = "���_�����N"/>
	* </Resource>
	*/

	class ResourceImageDefault
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageDefault )

	public:
		ResourceImageDefault();

	public:
		std::size_t getCount() const override;

		const mt::vec2f & getMaxSize( std::size_t _frame ) const override;
		const mt::vec2f & getSize( std::size_t _frame ) const override;
		const mt::vec2f & getOffset( std::size_t _frame ) const override;
		const mt::vec4f & getUV( std::size_t _frame ) const override;
		const mt::vec4f & getUVImage( std::size_t _frame ) const override;

		bool isAlpha( std::size_t _frame ) const override;
		bool getWrapX( std::size_t _frame ) const override;
		bool getWrapY( std::size_t _frame ) const override;

		Texture * getTexture( std::size_t _frame ) const override;
		void addImagePath( const ConstString& _imagePath, const mt::vec2f & _size );

		const ConstString & getFilename( std::size_t _frame ) const override;
		const ConstString & getCodecType( std::size_t _frame ) const override;

		std::size_t getFilenameCount() const override;

	public:
		inline const ResourceImage::ImageFrame & getImageFrame( std::size_t _frame ) const;

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
			int temp;
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
	inline const ResourceImage::ImageFrame & ResourceImageDefault::getImageFrame( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ];
	}
}
