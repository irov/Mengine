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
			bool isAlpha;
			Texture* texture;
			bool wrapX;
			bool wrapY;
		};

	public:
		//! ���������� ���������� �����������
		/*!
		\return ���������� �����������
		*/

		virtual std::size_t getCount() const = 0;

		//! ���������� ������������ ������ frame �����������
		/*!
		\param _frame ������ �����������
		\return ������������ ������
		*/
		virtual const mt::vec2f & getMaxSize( std::size_t _frame ) const = 0;

		//! ���������� ������ frame �����������
		/*!
		\param _frame ������ �����������
		\return ������
		*/
		virtual const mt::vec2f & getSize( std::size_t _frame ) const = 0;

		//! ���������� �������� frame �����������
		/*!
		\param _frame ������ �����������
		\return ��������
		*/
		virtual const mt::vec2f & getOffset( std::size_t _frame ) const = 0;

		//! ���������� ���������� ���������� frame �����������
		/*!
		\param _frame ������ �����������
		\return ���������� ����������
		*/
		virtual const mt::vec4f & getUV( std::size_t _frame ) const = 0;		

		virtual bool isAlpha( std::size_t _frame ) const = 0;

		//! ���������� frame �����������
		/*!
		\param _frame ������ �����������
		\return �����������
		*/
		virtual Texture* getTexture( std::size_t _frame ) = 0;

		//! ���������� �������� ����� �����������
		/*!
		\param _frame ������ �����������
		\return ��� ����� �����������
		*/
		virtual const ConstString & getFilename( std::size_t _frame ) const = 0;

		virtual std::size_t getFilenameCount() const = 0;

		virtual bool getWrapX( std::size_t _frame ) const;
		virtual bool getWrapY( std::size_t _frame ) const;

	public:
		void loader( XmlElement * _xml ) override;

	protected:

		//! ���������� �����������
		/*!
		\param _filename ��� �����������
		\param _buff ������ ������ � ������������
		\return �����������
		*/
		ImageFrame loadImageFrame( const ConstString& _pakName, const ConstString& _filename );
		ImageFrame createImageFrame( const ConstString& _name, const mt::vec2f& _size );
		ImageFrame createRenderTargetFrame( const ConstString& _name, const mt::vec2f& _size );
		void releaseImageFrame( const ImageFrame & _frame );

	protected:
		std::size_t m_filter;
	};
}
