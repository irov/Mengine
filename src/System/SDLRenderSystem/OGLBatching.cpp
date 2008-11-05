#	include "OGLRenderSystem.h"
//#	include "VBO.h"
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::initBatching()
{
	m_curPrimType = 0;
	m_currSrcBlend = 0;
	m_currDstBlend = 0;
	m_curTexture = 0;
	n_prim = 0;

	VertArray = new Menge::TVertex[VertexBufferSize];
	IndexArray = new Menge::uint16[IndexBufferSize];

	unsigned short n = 0;
	int c = 0;

	for(int i = 0; i < IndexBufferSize/6; i++) 
	{
		IndexArray[c++] = n;
		IndexArray[c++] = n + 1;
		IndexArray[c++] = n + 2;
		IndexArray[c++] = n + 2;
		IndexArray[c++] = n + 3;
		IndexArray[c++] = n;
		n += 4;
	}

	int mVertexCount = VertexBufferSize;

	Menge::uint16 * mIndices = new Menge::uint16[IndexBufferSize];

	int mIndexCount = IndexBufferSize;

	n = 0;
	c = 0;

	for(int i = 0; i < IndexBufferSize/6; i++) 
	{
		mIndices[c++] = n;
		mIndices[c++] = n + 1;
		mIndices[c++] = n + 2;
		mIndices[c++] = n + 2;
		mIndices[c++] = n + 3;
		mIndices[c++] = n;
		n += 4;
	}

	_glEnable2D();
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::deleteBatching()
{
//	delete [] IndexArray;
//	delete [] VertArray;
}

//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::_glEnable2D()   
{   
	//в HGE эта часть всегда была в BeginScene.
	/*GLint viewport[4];   

	glGetIntegerv( GL_VIEWPORT, viewport );   

	glMatrixMode( GL_PROJECTION );   
	glLoadIdentity();   

	glOrtho( viewport[0], viewport[0] + viewport[2],   
		viewport[1] + viewport[3], viewport[1], -1, 1 );   

	glMatrixMode( GL_MODELVIEW );   
	glLoadIdentity(); 

	renderBatch();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);  

	glEnable(GL_BLEND);
	glEnable(m_textureType);*/
	/*
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_ALPHA_TEST_REF,)
	glAlphaFunc(GL_ALPHA_TEST_FUNC,GL_GREATER);
	*/
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glEnable( GL_BLEND );
	glEnable( m_textureType );

	std::fill( m_worldMatrix, m_worldMatrix + 16, 0 );
	m_worldMatrix[0] = m_worldMatrix[5] = m_worldMatrix[10] = m_worldMatrix[15] = 1.0f;
	setWorldMatrix( m_worldMatrix );
	setViewMatrix( m_viewMatrix );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( m_viewport[0], /*m_viewport[0] +*/ m_viewport[2], /*m_viewport[1] +*/ m_viewport[3], m_viewport[1], -1, 1 );
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::_glEnable3D()
{
	renderBatch();

	glEnable(m_textureType);
	glEnable(GL_DEPTH_TEST);
	//glFrontFace(GL_CW);
	glDisable(GL_LIGHTING);  
	glDisable(GL_ALPHA_TEST);
	
	//pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

	//pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	//pD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	//pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
	//pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	//m_layer3D = true;

	//	glMatrixMode(GL_MODELVIEW);
	//glLoadMatrixf(m_glWorldViewMat);

	/*glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(m_glProjMat);

	glEnable(m_textureType);
	glEnable(GL_BLEND);

	glEnable( GL_DEPTH_TEST );  
	glDisable( GL_LIGHTING );  */
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::renderBatch()
{
	if(n_prim == 0)
	{
		return;
	}

	switch(m_curPrimType)
	{
	case GL_QUADS:

	//	buffer->renderIndexed();

		glVertexPointer(3, GL_FLOAT, sizeof(Menge::TVertex),  &VertArray[0].pos);
		glEnableClientState(GL_VERTEX_ARRAY);

		glNormalPointer(GL_FLOAT, sizeof(Menge::TVertex), &VertArray[0].n);
		glEnableClientState(GL_NORMAL_ARRAY );

		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Menge::TVertex), &VertArray[0].col);
		glEnableClientState(GL_COLOR_ARRAY);

		glTexCoordPointer(2, GL_FLOAT, sizeof(Menge::TVertex), &VertArray[0].uv);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY );

		glDrawElements(GL_TRIANGLES, n_prim * 6, GL_UNSIGNED_SHORT, IndexArray);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY );
		glDisableClientState(GL_NORMAL_ARRAY );
		glDisableClientState(GL_COLOR_ARRAY);

		break;

	case GL_TRIANGLES:
		glVertexPointer(3, GL_FLOAT, sizeof(Menge::TVertex),  &VertArray[0].pos);
		glEnableClientState(GL_VERTEX_ARRAY);

		glNormalPointer(GL_FLOAT, sizeof(Menge::TVertex), &VertArray[0].n);
		glEnableClientState(GL_NORMAL_ARRAY );

		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Menge::TVertex), &VertArray[0].col);
		glEnableClientState(GL_COLOR_ARRAY);

		glTexCoordPointer(2, GL_FLOAT, sizeof(Menge::TVertex), &VertArray[0].uv);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY );

		glDrawArrays(GL_TRIANGLES, 0, n_prim * 3);
		
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY );
		glDisableClientState(GL_NORMAL_ARRAY );
		glDisableClientState(GL_COLOR_ARRAY);

		break;
	}

	n_prim = 0;
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::Gfx_RenderQuad(const Menge::TVertex * quad, GLint tex, GLint srcBlend, GLint dstBlend)
{
	if( m_curPrimType != GL_QUADS 
		|| n_prim >= VertexBufferSize / 4 
		|| m_curTexture != tex 
		|| m_currSrcBlend != srcBlend 
		|| m_currDstBlend != dstBlend )
	{
		renderBatch();

		glTexParameteri( m_textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( m_textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		m_curPrimType = GL_QUADS;

		if( m_currSrcBlend != srcBlend || m_currDstBlend != dstBlend )
		{
			m_currSrcBlend = srcBlend;
			m_currDstBlend = dstBlend;
			glBlendFunc(srcBlend,dstBlend);
		}

		if( tex != m_curTexture )
		{
			glBindTexture(m_textureType, tex);
			GLenum err = glGetError();
			m_curTexture = tex;
		}
	}

	memcpy( &VertArray[n_prim * 4], quad, sizeof(Menge::TVertex) * 4 );
	n_prim++;
}
//////////////////////////////////////////////////////////////////////////
void OGLRenderSystem::Gfx_RenderTriple(const Menge::TVertex *quad, GLint tex, GLint srcBlend, GLint dstBlend)
{
	if( m_curPrimType != GL_TRIANGLES
		|| n_prim >= VertexBufferSize / 3 
		|| m_curTexture != tex 
		|| m_currSrcBlend != srcBlend 
		|| m_currDstBlend != dstBlend )
	{
		renderBatch();

		glTexParameteri( m_textureType, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameteri( m_textureType, GL_TEXTURE_WRAP_T, GL_REPEAT );
		m_curPrimType=GL_TRIANGLES;

		if( m_currSrcBlend != srcBlend || m_currDstBlend != dstBlend )
		{
			m_currSrcBlend = srcBlend;
			m_currDstBlend = dstBlend;
			glBlendFunc(srcBlend,dstBlend);
		}

		if( tex != m_curTexture )
		{
			glBindTexture(m_textureType, tex);
			GLenum err = glGetError();
			m_curTexture = tex;
		}
	}

	memcpy(&VertArray[n_prim * 3], quad, sizeof(Menge::TVertex) * 3);
	n_prim++;
}
//////////////////////////////////////////////////////////////////////////