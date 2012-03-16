#	pragma once

#	include "ResourceReference.h"

#	include "Math/vec4.h"

namespace Menge
{
	class TextureInterface;

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
			TextureInterface* texture;
			bool wrapX;
			bool wrapY;
		};

	public:
		virtual size_t getCount() const = 0;
		virtual const mt::vec2f & getMaxSize( size_t _frame ) const = 0;
		virtual const mt::vec2f & getSize( size_t _frame ) const = 0;
		virtual const mt::vec2f & getOffset( size_t _frame ) const = 0;
		virtual const mt::vec4f & getUV( size_t _frame ) const = 0;
		virtual const mt::vec4f & getUVImage( size_t _frame ) const = 0;

		virtual bool isAlpha( size_t _frame ) const = 0;

		virtual TextureInterface* getTexture( size_t _frame ) const = 0;
		virtual const ConstString & getFileName( size_t _frame ) const = 0;
		virtual const ConstString & getCodecType( size_t _frame ) const = 0;
		virtual size_t getFilenameCount() const = 0;

		virtual bool getWrapX( size_t _frame ) const;
		virtual bool getWrapY( size_t _frame ) const;

	public:
		void loader( BinParser * _parser ) override;

	protected:

		bool validImageFrame_( const ConstString& _pakName, const ConstString& _filename, const ConstString& _codec ) const;
		bool loadImageFrame_( ImageFrame & _frame, const ConstString& _pakName, const ConstString& _filename, const ConstString& _codec ) const;
		bool loadImageFrameCombineRGBAndAlpha_( ImageFrame& _frame, const ConstString& _pakName, const ConstString& _fileNameRGB, const ConstString& _fileNameAlpha, const ConstString & _codecRGB, const ConstString & _codecAlpha ) const;
		bool createImageFrame_( ImageFrame & _frame, const ConstString& _name, const mt::vec2f& _size ) const;
		void releaseImageFrame_( const ImageFrame & _frame ) const;
		bool prepareImageFrame_( ImageFrame & _frame, TextureInterface * texture ) const;
	public:
		static ConstString s_getImageCodec( const ConstString & _filename );
		
	protected:
		size_t m_filter;
	};
}
