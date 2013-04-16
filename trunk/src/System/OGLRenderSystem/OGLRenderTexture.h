///*
//*  OGLRenderTexture.h
//*  Mac
//*
//*  Created by Berserk on 13.11.08.
//*  Copyright 2008 __Menge__. All rights reserved.
//*
//*/
//
//#	pragma once
//
//#	include "OGLTexture.h"
//
//namespace Menge
//{
//    class OGLRenderTexture
//        : public OGLTexture
//    {
//    public:
//        OGLRenderTexture();
//        ~OGLRenderTexture();
//
//    public:
//        bool create( const Menge::String& _name, std::size_t _width, std::size_t _height, std::size_t _data_width, std::size_t _data_height );
//        bool enable();
//
//    protected:
//        unsigned char* lock( int* _pitch, const Rect& _rect, bool _readOnly = true ) override;
//        void unlock() override;
//
//    public:
//        void flush();
//
//    protected:
//#ifdef MENGE_PLATFORM_MACOSX
//        AGLPbuffer m_aglPbuffer;
//        AGLContext m_aglContext;
//#endif
//    };
//}
