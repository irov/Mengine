#	pragma once
#	include "Config/Typedef.h"
#	include "Interface/RenderSystemInterface.h"

#	include "HGE.h"

#	include "libs/Math/vec2.h"

class HGETexture :
	public Menge::RenderImageInterface
{
public:
	HGETexture( HGE* _hge, bool _freeOnDelete = true );
	HGETexture( HGE* _hge, HTEXTURE _htex, const Menge::String& _name, float _width, float _height, bool _freeOnDelete = false );
	HGETexture( HGE* _hge, const Menge::String& _name, float _width, float _height, bool _freeOnDelete = true );
	~HGETexture();

	void load( const Menge::TextureDesc& _desc );
	void unload();
	void restore( HTEXTURE _htex );
	float getWidth() const override;
	float getHeight() const override;
	void writeToFile( const Menge::String & _filename ) override;
	const Menge::String & getDescription() const override;
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
	float m_width;
	float m_height;
	bool m_freeOnDelete;
	int m_ref;
	mt::vec2f m_uvMask;
	Menge::PixelFormat m_pixelFormat;
	unsigned char* m_lockBuffer;
};