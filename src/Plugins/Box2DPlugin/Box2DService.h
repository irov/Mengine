#pragma once

#include "Box2DInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"
#include "Kernel/ThreadTask.h"

namespace Mengine
{
    class Box2DService
        : public ServiceBase<Box2DServiceInterface>
    {
    public:
        Box2DService();
        ~Box2DService();

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        Box2DWorldInterfacePtr createWorld( const mt::vec2f & _gravity, const Char * _doc ) override;
        void destroyWorld( const Box2DWorldInterfacePtr & _world ) override;

    protected:
        FactoryPtr m_factoryWorlds;
    };
}