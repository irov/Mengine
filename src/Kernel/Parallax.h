//#pragma once
//
//#include "Kernel/Node.h"
//
//#include "Kernel/RenderCameraProxy.h"
//
//namespace Mengine
//{
//    class Parallax
//        : public Node
//    {
//        DECLARE_VISITABLE( Node );
//
//    public:
//        Parallax();
//        ~Parallax() override;
//
//    public:
//        void setParallaxFactor( const mt::vec3f & _parallax );
//        const mt::vec3f & getParallaxFactor() const;
//
//    protected:
//        void render( const RenderContext * _state ) override;
//
//    protected:
//        mt::vec3f m_parallaxFactor;
//
//        RenderCameraProxy m_parallaxCamera;
//    };
//    //////////////////////////////////////////////////////////////////////////
//    typedef IntrusivePtr<Parallax> ParallaxPtr;
//    //////////////////////////////////////////////////////////////////////////
//}
