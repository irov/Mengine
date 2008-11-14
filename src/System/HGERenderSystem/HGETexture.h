#	pragma once
#	include "Config/Typedef.h"
#	include "Interface/RenderSystemInterface.h"

#	include "HGE.h"

#	include "libs/Math/vec2.h"

class HGETexture 
	: public Menge::RenderImageInterface
{
public:
	HGETexture( HGE* _hge, bool _freeOnDelete = true );
	HGETexture( HGE* _hge, HTEXTURE _htex, const Menge::String& _name, std::size_t _width, std::size_t _height, bool _freeOnDelete = false );
	HGETexture( HGE* _hge, const Menge::String& _name,
		std::size_t _width, std::size_t _height,
		std::size_t _data_width, std::size_t _data_height,
		Menge::PixelFormat _format, bool _freeOnDelete = true );
	~HGETexture();

	void load( const Menge::String& _name, std::size_t _width, std::size_t _height, const Menge::TextureDesc& _desc );
	void unload();
	void restore( HTEXTURE _htex );
	std::size_t getWidth() const override;
	std::size_t getHeight() const override;
	const Menge::String & getDescription() const override;
	unsigned char* lock( int* _pitch, bool _readOnly ) override;
	void unlock() override;
	Menge::PixelFormat getPixelFormat();

	HTEXTURE getHandle() const;

	int incRef() { return ++m_ref; }
	int decRef() { return --m_ref; }

	const mt::vec2f& getUVMask() const;
private:
	HGE* m_hge;
	Menge::String m_name;
	HTEXTURE m_hTexture;
	std::size_t m_width;
	std::size_t m_height;
	bool m_freeOnDelete;
	int m_ref;
	mt::vec2f m_uvMask;
	Menge::PixelFormat m_pixelFormat;
};