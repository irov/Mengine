#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

namespace Menge
{
	class Texture;

	//! ResourceImage - интерфейс дл€ ресурсов, которые работают с избражени€ми.
	/*! 
	*
	*/

	class ResourceImage
		: public ResourceReference
	{
	public:
		//!  онструктор.
		/*!
		\param _name им€ ресурса.
		*/
		ResourceImage();

		struct ImageFrame
		{
			mt::vec2f maxSize;
			mt::vec2f size;
			mt::vec2f offset;
			mt::vec4f uv_image;
			mt::vec4f uv;
			mt::vec2f pow_scale;
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
		virtual const mt::vec4f & getUVImage( std::size_t _frame ) const = 0;

		virtual bool isAlpha( std::size_t _frame ) const = 0;

		virtual Texture* getTexture( std::size_t _frame ) const = 0;
		virtual const ConstString & getFilename( std::size_t _frame ) const = 0;
		virtual const ConstString & getCodecType( std::size_t _frame ) const = 0;
		virtual std::size_t getFilenameCount() const = 0;

		virtual bool getWrapX( std::size_t _frame ) const;
		virtual bool getWrapY( std::size_t _frame ) const;

	public:
		void loader( BinParser * _parser ) override;

	protected:

		bool validImageFrame_( const ConstString& _pakName, const ConstString& _filename, const ConstString& _codec ) const;
		bool loadImageFrame_( ImageFrame & _frame, const ConstString& _pakName, const ConstString& _filename, const ConstString& _codec ) const;
		bool loadImageFrameCombineRGBAndAlpha_( ImageFrame& _frame, const ConstString& _pakName, const ConstString& _fileNameRGB, const ConstString& _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha ) const;
		bool createImageFrame_( ImageFrame & _frame, const ConstString& _name, const mt::vec2f& _size ) const;
		void releaseImageFrame_( const ImageFrame & _frame ) const;
		bool prepareImageFrame_( ImageFrame & _frame, Texture * texture ) const;
	public:
		static ConstString s_getImageCodec( const ConstString & _filename );
		
	protected:
		std::size_t m_filter;
	};
}
