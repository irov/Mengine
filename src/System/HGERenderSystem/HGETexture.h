#	pragma once
#	include "Config/Typedef.h"
#	include "Interface/RenderSystemInterface.h"

class HGE;

class HGETexture :
	public RenderImageInterface
{
public:
	HGETexture( HGE* _hge );
	HGETexture( HGE* _hge, const Menge::String& _name, std::size_t _width, std::size_t _height );
	~HGETexture();

	void load( const TextureDesc& _desc );
	void unload();
	float getWidth() const override;
	float getHeight() const override;
	void writeToFile( const char* _filename ) override;
	const char * getDescription() const override;
	unsigned long getHandle() const;
private:
	HGE* m_hge;
	Menge::String m_name;
	unsigned long m_hTexture;
	std::size_t m_width;
	std::size_t m_height;
};