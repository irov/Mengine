#	pragma once

#	include "ResourceImage.h"

#	include <vector>

namespace Menge
{
	//! ResourceImageCell - ������-����, ������� ������ ����������� �����������. 

	/*! xml - ���� ����� ��������� ���������:
	* <Resource Name = "���_�������" Type = "ResourceImageCell" >
	*	<File Path = "���_�����"/>
	*   <Cell X = "���������� �������� �� �����������"/>
	*   <Cell Y = "���������� �������� �� ���������"/>
	* </Resource>
	*/

	class ResourceImageCell
		: public ResourceImage
	{
		RESOURCE_DECLARE( ResourceImageCell )

	public:
		ResourceImageCell();

	public:
		size_t getCount() const override;

		const mt::vec2f & getMaxSize( size_t _frame ) const override;
		const mt::vec2f & getSize( size_t _frame ) const override;
		const mt::vec2f & getOffset( size_t _frame ) const override;
		const mt::vec4f & getUV( size_t _frame ) const override;
		const mt::vec4f & getUVImage( size_t _frame ) const override;

		size_t getNumX() const;
		size_t getNumY() const;

		bool isAlpha( size_t _frame ) const;

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
		size_t m_numX;
		size_t m_numY;

		typedef std::vector<mt::vec4f> TVectorUV;
		TVectorUV m_uvs;

		ImageFrame m_imageFrame;

	private:
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

		ImageDesc m_imageDesc;
	};
}
