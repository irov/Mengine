#pragma once

#include "Interface/FactoryInterface.h"

#include "Box2DInterface.h"

#include "Kernel/ServiceBase.h"
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
        Box2DWorldInterfacePtr createWorld( const mt::vec2f & _gravity, float _scaler, const DocumentInterfacePtr & _doc ) override;
        void destroyWorld( const Box2DWorldInterfacePtr & _world ) override;

    protected:
        FactoryInterfacePtr m_factoryWorlds;
    };
}