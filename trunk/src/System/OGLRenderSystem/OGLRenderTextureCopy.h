//#	pragma once
//
//#	include "OGLTexture.h"
//
//namespace Menge
//{
//    class OGLRenderTextureCopy
//        : public OGLTexture
//    {
//    public:
//        OGLRenderTextureCopy();
//        ~OGLRenderTextureCopy();
//
//    public:
//        bool create( const Menge::String& _name, std::size_t _width, std::size_t _height, std::size_t _data_width, std::size_t _data_height );
//        bool enable();
//
//    protected:
//        unsigned char* lock( int* _pitch, const Rect& _rect, bool _readOnly = true ) override;
//        void unlock() override;
//
//    protected:
//        void flush();
//
//    protected:
//    };
//}
//
