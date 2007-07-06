#	include "RenderSystem3dInterface.h"
#	include <d3d9.h>

class	d3d9Texture : public Texture
{
public:
	d3d9Texture();
	~d3d9Texture();
	IDirect3DTexture9*	_get() const;
private:
	IDirect3DTexture9*	m_texture;
};