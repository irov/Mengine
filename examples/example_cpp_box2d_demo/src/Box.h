#pragma once

#include "Plugins/Box2DPlugin/Box2DInterface.h"

#include "Kernel/Factorable.h"

#include "Engine/Vectorizator.h"

#include "Config/Vector.h"

namespace Mengine
{
    class Box
        : public Factorable
    {
    public:
        Box( const Box2DBodyInterfacePtr & _body, const VectorizatorPtr & _vectorizator );
        ~Box() override;

    public:
        void update() const;

    private:
        Box2DBodyInterfacePtr m_body;
        VectorizatorPtr m_vectorizator;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Box> BoxPtr;
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<BoxPtr> VectorBoxPtr;
    //////////////////////////////////////////////////////////////////////////
}