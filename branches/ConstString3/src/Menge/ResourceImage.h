#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

namespace Menge
{
	class Texture;

	//! ResourceImage - ��������� ��� ��������, ������� �������� � ������������.
	/*! 
	*
	*/

	class ResourceImage
		: public ResourceReference
	{
	public:
		//! �����������.
		/*!
		\param _name ��� �������.
		*/
		ResourceImage();

		struct ImageFrame
		{
			mt::vec2f maxSize;
			mt::vec2f size;
			mt::vec2f offset;
			mt::vec4f uv;
			mt::vec2f uv_pow;
			bool isAlpha;
			Texture* texture;
			bool wrapX;
			bool wrapY;
		};

	public:
		virtual std::size_t getCount() const = 0;
		virtual const mt::vec2f & getMaxSize( std::size_t _frame ) const = 0;
		virtual const mt::vec2f & getSize( std::size_t _frame ) const = 0;
		virtual const mt::vec2f & getOffset( std::size_t _frame ) const = 0;
		virtual const mt::vec4f & getUV( std::size_t _frame ) const = 0;		

		virtual bool isAlpha( std::size_t _frame ) const = 0;

		virtual Texture* getTexture( std::size_t _frame ) = 0;
		virtual const ConstString & getFilename( std::size_t _frame ) const = 0;
		virtual const ConstString & getCodecType( std::size_t _frame ) const = 0;
		virtual std::size_t getFilenameCount() const = 0;

		virtual bool getWrapX( std::size_t _frame ) const;
		virtual bool getWrapY( std::size_t _frame ) const;

	public:
		void loader( BinParser * _parser ) override;

	protected:

		//! ���������� �����������
		/*!
		\param _filename ��� �����������
		\param _buff ������ ������ � ������������
		\return �����������
		*/
		bool loadImageFrame_( ImageFrame & _frame, const ConstString& _pakName, const ConstString& _filename, const ConstString& _codec ) const;
		bool createImageFrame_( ImageFrame & _frame, const ConstString& _name, const mt::vec2f& _size ) const;
		bool createRenderTargetFrame_( ImageFrame & _frame, const ConstString& _name, const mt::vec2f& _size ) const;
		void releaseImageFrame_( const ImageFrame & _frame ) const;

	public:
		static ConstString s_getImageCodec( const ConstString & _filename );

	protected:
		std::size_t m_filter;
	};
}
