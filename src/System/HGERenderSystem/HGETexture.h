#	pragma once
#	include "Config/Typedef.h"
#	include "Interface/RenderSystemInterface.h"

#	include "HGE.h"

#	include "libs/Math/vec2.h"

class HGETexture :
	public RenderImageInterface
{
public:
	HGETexture( HGE* _hge, bool _freeOnDelete = true );
	HGETexture( HGE* _hge, HTEXTURE _htex, const Menge::String& _name, std::size_t _width, std::size_t _height, bool _freeOnDelete = false );
	HGETexture( HGE* _hge, const Menge::String& _name, std::size_t _width, std::size_t _height, bool _freeOnDelete = true );
	~HGETexture();

	void load( const TextureDesc& _desc );
	void unload();
	std::size_t getWidth() const override;
	std::size_t getHeight() const override;
	void writeToFile( const char* _filename ) override;
	const char * getDescription() const override;
	unsigned char* lock() override;
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
	unsigned char* m_lockBuffer;
};