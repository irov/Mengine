#	pragma once
#	include "Config/Typedef.h"
#	include "Interface/RenderSystemInterface.h"

#	include "HGE.h"

class HGETexture :
	public RenderImageInterface
{
public:
	HGETexture( HGE* _hge, bool _freeOnDelete = true );
	HGETexture( HGE* _hge, HTEXTURE _htex, const Menge::String& _name, bool _freeOnDelete = false );
	HGETexture( HGE* _hge, const Menge::String& _name, std::size_t _width, std::size_t _height, bool _freeOnDelete = true );
	~HGETexture();

	void load( const TextureDesc& _desc );
	void unload();
	float getWidth() const override;
	float getHeight() const override;
	void writeToFile( const char* _filename ) override;
	const char * getDescription() const override;
	unsigned long getHandle() const;

	int incRef() { return ++m_ref; }
	int decRef() { return --m_ref; }

private:
	HGE* m_hge;
	Menge::String m_name;
	unsigned long m_hTexture;
	std::size_t m_width;
	std::size_t m_height;
	bool m_freeOnDelete;
	int m_ref;
};