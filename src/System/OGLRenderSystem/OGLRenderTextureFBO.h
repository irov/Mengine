//#	pragma once
//
//#	include "OGLTexture.h"
//
//namespace Menge
//{
//    class OGLRenderTextureFBO
//        : public OGLTexture
//    {
//    public:
//        OGLRenderTextureFBO();
//        ~OGLRenderTextureFBO();
//
//        bool create( const Menge::String& _name, std::size_t _width, std::size_t _height, std::size_t _data_width, std::size_t _data_height );
//        bool enable();
//
//        void flush();
//
//    protected:
//        unsigned char* lock( int* _pitch, const Rect& _rect, bool _readOnly = true ) override;
//        void unlock() override;
//
//    protected:
//        GLuint m_framebufferID;
//        GLuint m_renderbufferID;
//    };
//}
//
