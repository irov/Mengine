#include "TraficJamSceneEventReceiver.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"

#include "Plugins/GOAPPlugin/Tasks/GOAPCook.h"

#include "Engine/SurfaceSolidColor.h"
#include "Engine/ShapeQuadFixed.h"
#include "Engine/SurfaceImage.h"
#include "Engine/ResourceImageDefault.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/Surface.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/StringHelper.h"
#include "Kernel/EntityHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/SchedulerHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Vector.h"

#include "TraficMap.h"

namespace Mengine
{
    const Color bgColor( 128, 128, 128 );
    const mt::vec2f backSize(715, 715);

    const int32_t borderIndent = 25;
    const int32_t playerCarID = 2;

    const int32_t horizontalCar = 0;
    const int32_t verticalCar = 1;

    const Color playerColor( 255, 0, 0 );
    const mt::vec2f playerSize( 110, 225 );

    const Color horizontalColor( 0, 255, 0 );
    const mt::vec2f horizontalSize( 225, 110 );

    const Color verticalColor( 0, 0, 255 );
    const mt::vec2f verticalSize( 110, 225 );

    //////////////////////////////////////////////////////////////////////////
    TraficJamSceneEventReceiver::TraficJamSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TraficJamSceneEventReceiver::~TraficJamSceneEventReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TraficJamSceneEventReceiver::onSceneAppMouseLeave(const EntityBehaviorInterfacePtr& _behavior)
    {
        MENGINE_UNUSED(_behavior);

        LOGGER_MESSAGE("Scene onSceneAppMouseLeave [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TraficJamSceneEventReceiver::onSceneAppMouseEnter(const EntityBehaviorInterfacePtr& _behavior)
    {
        MENGINE_UNUSED(_behavior);

        LOGGER_MESSAGE("Scene onSceneAppMouseEnter [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TraficJamSceneEventReceiver::onSceneAppFocus(const EntityBehaviorInterfacePtr& _behavior, bool _focus)
    {
        MENGINE_UNUSED(_behavior);
        MENGINE_UNUSED(_focus);

        LOGGER_MESSAGE("Scene onSceneAppFocus [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TraficJamSceneEventReceiver::onEntityCreate(const EntityBehaviorInterfacePtr& _behavior, Entity* _entity)
    {
        MENGINE_UNUSED(_behavior);

        m_scene = Helper::staticNodeCast<Scene*>(_entity);

        LOGGER_MESSAGE("Scene onEntityCreate [%s]"
            , m_scene->getName().c_str()
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::onEntityDestroy(const EntityBehaviorInterfacePtr& _behavior)
    {
        MENGINE_UNUSED(_behavior);

        LOGGER_MESSAGE("Scene onEntityDestroy [%s]"
            , m_scene->getName().c_str()
        );

        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TraficJamSceneEventReceiver::onEntityPreparation(const EntityBehaviorInterfacePtr& _behavior)
    {
        MENGINE_UNUSED(_behavior);

        LOGGER_MESSAGE("Scene onEntityPreparation [%s]"
            , m_scene->getName().c_str()
        );

        this->setupGame();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TraficJamSceneEventReceiver::onEntityActivate(const EntityBehaviorInterfacePtr& _behavior)
    {
        MENGINE_UNUSED(_behavior);

        LOGGER_MESSAGE("Scene onEntityActivate [%s]"
            , m_scene->getName().c_str()
        );

        this->runTaskChains();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::onEntityPreparationDeactivate(const EntityBehaviorInterfacePtr& _behavior)
    {
        MENGINE_UNUSED(_behavior);

        LOGGER_MESSAGE("Scene onEntityPreparationDeactivate [%s]"
            , m_scene->getName().c_str()
        );

        this->clearTaskChain();
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::onEntityDeactivate(const EntityBehaviorInterfacePtr& _behavior)
    {
        MENGINE_UNUSED(_behavior);

        LOGGER_MESSAGE("Scene onEntityDeactivate [%s]"
            , m_scene->getName().c_str()
        );

        this->clearGameNode();
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::onEntityCompile(const EntityBehaviorInterfacePtr& _behavior)
    {
        MENGINE_UNUSED(_behavior);

        LOGGER_MESSAGE("Scene onEntityCompile [%s]"
            , m_scene->getName().c_str()
        );
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::onEntityRelease(const EntityBehaviorInterfacePtr& _behavior)
    {
        MENGINE_UNUSED(_behavior);

        LOGGER_MESSAGE("Scene onEntityRelease [%s]"
            , m_scene->getName().c_str()
        );
    }
    /////////////////////////////////////////////////////////////////////////
    ShapeQuadFixedPtr TraficJamSceneEventReceiver::createColor(const ConstString& _name, const SurfaceSolidColorPtr& _resource)
    {
         MENGINE_ASSERTION_MEMORY_PANIC(_resource, "'%s' resource is NULL"
            , _name.c_str()
        );

        ShapeQuadFixedPtr shape = PROTOTYPE_SERVICE()
            ->generatePrototype(STRINGIZE_STRING_LOCAL("Node"), STRINGIZE_STRING_LOCAL("ShapeQuadFixed"), MENGINE_DOCUMENT_FACTORABLE);

        MENGINE_ASSERTION_MEMORY_PANIC(shape);

        shape->setName(_name);
        shape->setSurface(_resource);

        return shape;
    }
    /////////////////////////////////////////////////////////////////////////
    HotSpotPolygonPtr TraficJamSceneEventReceiver::createHotSpot(const ConstString& _name, const mt::vec2f& _size)
    {
        HotSpotPolygonPtr hotspot = PROTOTYPE_SERVICE()
            ->generatePrototype(STRINGIZE_STRING_LOCAL("Node"), STRINGIZE_STRING_LOCAL("HotSpotPolygon"), MENGINE_DOCUMENT_FACTORABLE);

        MENGINE_ASSERTION_MEMORY_PANIC(hotspot);

        hotspot->setName(_name);

        Polygon polygon;

        polygon.append({ 0.f, 0.f });
        polygon.append({ _size.x, 0.f });
        polygon.append({ _size.x, _size.y });
        polygon.append({ 0.f, _size.y });

        hotspot->setPolygon(polygon);

        hotspot->enable();

        return hotspot;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TraficJamSceneEventReceiver::setupGame()
    {
        // create game node
        NodePtr node = PROTOTYPE_GENERATE(STRINGIZE_STRING_LOCAL("Node"), STRINGIZE_STRING_LOCAL("Node"), MENGINE_DOCUMENT_FACTORABLE);

        MENGINE_ASSERTION_MEMORY_PANIC(node);

        node->setName(STRINGIZE_STRING_LOCAL("Node_Game"));

        m_gameNode = node;

        m_scene->addChild(node);

        // setup some settings

        m_semaphoreGameOver = GOAP_SERVICE()
            ->makeSemaphore(0);


        // setup background
        bool setupBackgroundResult = this->setupBackground();

        MENGINE_UNUSED(setupBackgroundResult);

        MENGINE_ASSERTION_FATAL(setupBackgroundResult == true);

        traficMap.initMap();

        bool setupCars = this->setupCars();

        MENGINE_UNUSED(setupCars);

        MENGINE_ASSERTION_FATAL(setupCars == true);

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TraficJamSceneEventReceiver::setupBackground()
    {
        SurfaceSolidColorPtr colorSolid = PROTOTYPE_SERVICE()
            ->generatePrototype(STRINGIZE_STRING_LOCAL("Surface"), STRINGIZE_STRING_LOCAL("SurfaceSolidColor"), MENGINE_DOCUMENT_FACTORABLE);

        MENGINE_ASSERTION_MEMORY_PANIC(colorSolid);

        colorSolid->setSolidColor(bgColor);
        colorSolid->setSolidSize(backSize);

        m_backgroundSize = backSize;

        // setup sprite
        ShapeQuadFixedPtr sprite = this->createColor(STRINGIZE_STRING_LOCAL("BG"), colorSolid);

        MENGINE_ASSERTION_MEMORY_PANIC(sprite);

        m_background = sprite;

        m_gameNode->addChild(sprite);

        // setup position
        const Resolution& resolution = APPLICATION_SERVICE()
            ->getContentResolution();

        float resolutionWidth = (resolution.getWidthF() - m_backgroundSize.x) / 2;
        float resolutionHeight = (resolution.getHeightF() - m_backgroundSize.y) / 2;

        m_gameNode->setLocalPosition({ resolutionWidth, resolutionHeight, 0.f });

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TraficJamSceneEventReceiver::setupCars()
    {
        const RandomizerInterfacePtr& randomizer = PLAYER_SERVICE()
            ->getRandomizer();

        int32_t mapSelector = randomizer->getRandomRangei( 1, 4 );

        switch ( mapSelector )
        {
        case 1: //setupCar(cellX, cellY)
        {
            this->setupCar(3, 0, playerCarID );
            this->setupCar(0, 0, horizontalCar );
            this->setupCar(4, 1, horizontalCar );
            this->setupCar(2, 2, horizontalCar );
            this->setupCar(4, 2, horizontalCar );
            this->setupCar(0, 3, horizontalCar );
            this->setupCar(2, 3, horizontalCar );
            this->setupCar(0, 4, horizontalCar );
            this->setupCar(4, 5, horizontalCar );
            this->setupCar(2, 0, verticalCar );
            this->setupCar(1, 1, verticalCar );
            this->setupCar(5, 3, verticalCar );

            break;
        }
        case 2:
        {
            this->setupCar(3, 0, playerCarID);
            this->setupCar(1, 0, horizontalCar );
            this->setupCar(4, 1, horizontalCar );
            this->setupCar(3, 2, horizontalCar );
            this->setupCar(1, 3, horizontalCar );
            this->setupCar(0, 4, horizontalCar );
            this->setupCar(0, 5, horizontalCar );
            this->setupCar(3, 5, horizontalCar );
            this->setupCar(0, 0, verticalCar );
            this->setupCar(1, 1, verticalCar );
            this->setupCar(2, 1, verticalCar );
            this->setupCar(5, 2, verticalCar );
            this->setupCar(2, 4, verticalCar );
            this->setupCar(5, 4, verticalCar );

            break;
        }
        case 3:
        {
            this->setupCar(3, 2, playerCarID);
            this->setupCar(0, 2, horizontalCar );
            this->setupCar(4, 2, horizontalCar );
            this->setupCar(4, 3, horizontalCar );
            this->setupCar(2, 4, horizontalCar );
            this->setupCar(1, 5, horizontalCar );
            this->setupCar(3, 5, horizontalCar );
            this->setupCar(2, 0, verticalCar );
            this->setupCar(4, 0, verticalCar );
            this->setupCar(2, 2, verticalCar );
            this->setupCar(1, 3, verticalCar );
            this->setupCar(0, 4, verticalCar );
            this->setupCar(5, 4, verticalCar );
            
            break;
        }
        default:
            break;
        }
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    HotSpotPolygonPtr TraficJamSceneEventReceiver::addHotspot(const uint32_t _row, const uint32_t _col, const uint32_t _id)
    {
        mt::vec2f hotspotSize({ cellInPixels, cellInPixels });

        ConstString name = Helper::stringizeStringFormat("HotSpot_%d", _id);

        HotSpotPolygonPtr hotspot = this->createHotSpot(name, hotspotSize);

        hotspot->setLocalPosition({ _row * hotspotSize.x, _col * hotspotSize.y, 0.f });

        return hotspot;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TraficJamSceneEventReceiver::setupCar(const uint32_t _row, const uint32_t _col, const uint32_t _resourceImageId)
    {
        static int carId = 1;

        Car newCar;

        ConstString name = Helper::stringizeStringFormat("Car_%d", carId);

        ShapeQuadFixedPtr sprite;
        
        if (_resourceImageId == playerCarID)
        {
            SurfaceSolidColorPtr colorSolid = PROTOTYPE_SERVICE()
                ->generatePrototype(STRINGIZE_STRING_LOCAL("Surface"), STRINGIZE_STRING_LOCAL("SurfaceSolidColor"), MENGINE_DOCUMENT_FACTORABLE);

            MENGINE_ASSERTION_MEMORY_PANIC(colorSolid);

            colorSolid->setSolidColor( playerColor );
            colorSolid->setSolidSize( playerSize );

            sprite = this->createColor(name, colorSolid);

            MENGINE_ASSERTION_MEMORY_PANIC(sprite);
            //create color and chose right size
            
            traficMap.setCell(_row, _col, ECellType::ECellType_PLAYER_V_UP);
            newCar.hotspotFirst = addHotspot(0, 0, ECellType::ECellType_PLAYER_V_UP);

            traficMap.setCell(_row, _col + 1, ECellType::ECellType_PLAYER_V_DOWN);
            newCar.hotspotSecond = addHotspot(0, 1, ECellType::ECellType_PLAYER_V_DOWN);

            sprite->addChild(newCar.hotspotFirst);
            sprite->addChild(newCar.hotspotSecond);
        }
        else if (_resourceImageId == horizontalCar)
        {
            SurfaceSolidColorPtr colorSolid = PROTOTYPE_SERVICE()
                ->generatePrototype(STRINGIZE_STRING_LOCAL("Surface"), STRINGIZE_STRING_LOCAL("SurfaceSolidColor"), MENGINE_DOCUMENT_FACTORABLE);

            MENGINE_ASSERTION_MEMORY_PANIC(colorSolid);

            colorSolid->setSolidColor( horizontalColor );
            colorSolid->setSolidSize( horizontalSize );

            sprite = this->createColor(name, colorSolid);

            traficMap.setCell(_row, _col, ECellType::ECellType_HORIZONTAL_LEFT);
            newCar.hotspotFirst = addHotspot(0, 0, ECellType::ECellType_HORIZONTAL_LEFT);

            traficMap.setCell(_row + 1, _col, ECellType::ECellType_HORIZONTAL_RIGHT);
            newCar.hotspotSecond = addHotspot(1, 0, ECellType::ECellType_HORIZONTAL_RIGHT);

            sprite->addChild(newCar.hotspotFirst);
            sprite->addChild(newCar.hotspotSecond);
        }
        else if (_resourceImageId == verticalCar)
        {
            SurfaceSolidColorPtr colorSolid = PROTOTYPE_SERVICE()
                ->generatePrototype(STRINGIZE_STRING_LOCAL("Surface"), STRINGIZE_STRING_LOCAL("SurfaceSolidColor"), MENGINE_DOCUMENT_FACTORABLE);

            MENGINE_ASSERTION_MEMORY_PANIC(colorSolid);

            colorSolid->setSolidColor( verticalColor );
            colorSolid->setSolidSize( verticalSize );

            sprite = this->createColor(name, colorSolid);

            traficMap.setCell(_row, _col, ECellType::ECellType_VERTICAL_UP);
            newCar.hotspotFirst = addHotspot(0, 0, ECellType::ECellType_VERTICAL_UP);

            traficMap.setCell(_row, _col + 1, ECellType::ECellType_VERTICAL_DOWN);
            newCar.hotspotSecond = addHotspot(0, 1, ECellType::ECellType_VERTICAL_DOWN);

            sprite->addChild(newCar.hotspotFirst);
            sprite->addChild(newCar.hotspotSecond);
        }

        newCar.sprite = sprite;

        m_gameNode->addChild(sprite);

        const mt::vec3f & spriteLocalPosition = {borderIndent + _row * cellInPixels, borderIndent + _col * cellInPixels, 0.f};

        sprite->setLocalPosition( spriteLocalPosition );

        m_cars.push_back(newCar);

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::moveLeft(ShapeQuadFixedPtr _sprite)
    {
        mt::vec3f pos = _sprite->getLocalPosition();

        int32_t x_pos = (static_cast<int32_t>(pos.x - borderIndent)) / static_cast<int32_t>(cellInPixels);
        int32_t y_pos = (static_cast<int32_t>(pos.y - borderIndent)) / static_cast<int32_t>(cellInPixels);

        ECellType carType = traficMap.getCell(x_pos - 1, y_pos);

        if ( x_pos > 0 && carType == ECellType::ECellType_EMPTY)
        {
            _sprite->setLocalPositionX(pos.x - cellInPixels);
            traficMap.setCell(x_pos - 1, y_pos, ECellType::ECellType_HORIZONTAL_LEFT);
            traficMap.setCell(x_pos, y_pos, ECellType::ECellType_HORIZONTAL_RIGHT);
            traficMap.setCell(x_pos + 1, y_pos, ECellType::ECellType_EMPTY);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::moveRight(ShapeQuadFixedPtr _sprite)
    {
        mt::vec3f pos = _sprite->getLocalPosition();

        int32_t x_pos = (static_cast<int32_t>(pos.x - borderIndent)) / static_cast<int32_t>(cellInPixels);
        int32_t y_pos = (static_cast<int32_t>(pos.y - borderIndent)) / static_cast<int32_t>(cellInPixels);

        ECellType carType = traficMap.getCell( x_pos + 2, y_pos );

        if (x_pos < 4 && carType == ECellType::ECellType_EMPTY)
        {
            _sprite->setLocalPositionX(pos.x + cellInPixels);
            traficMap.setCell(x_pos + 1, y_pos, ECellType::ECellType_HORIZONTAL_LEFT);
            traficMap.setCell(x_pos, y_pos, ECellType::ECellType_EMPTY);
            traficMap.setCell(x_pos + 2, y_pos, ECellType::ECellType_HORIZONTAL_RIGHT);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::moveUp(ShapeQuadFixedPtr _sprite)
    {
        mt::vec3f pos = _sprite->getLocalPosition();

        int32_t x_pos = (static_cast<int32_t>(pos.x - borderIndent)) / static_cast<int32_t>(cellInPixels);
        int32_t y_pos = (static_cast<int32_t>(pos.y - borderIndent)) / static_cast<int32_t>(cellInPixels);

        ECellType carType = traficMap.getCell( x_pos, y_pos - 1 );

        if (y_pos > 0 && carType == ECellType::ECellType_EMPTY)
        {
            _sprite->setLocalPositionY(pos.y - cellInPixels);
            traficMap.setCell(x_pos, y_pos - 1, ECellType::ECellType_VERTICAL_UP);
            traficMap.setCell(x_pos, y_pos, ECellType::ECellType_VERTICAL_DOWN);
            traficMap.setCell(x_pos, y_pos + 1, ECellType::ECellType_EMPTY);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::moveDown(ShapeQuadFixedPtr _sprite)
    {
        mt::vec3f pos = _sprite->getLocalPosition();

        int32_t x_pos = (static_cast<int32_t>(pos.x - borderIndent)) / static_cast<int32_t>(cellInPixels);
        int32_t y_pos = (static_cast<int32_t>(pos.y - borderIndent)) / static_cast<int32_t>(cellInPixels);

        ECellType carType = traficMap.getCell( x_pos, y_pos + 2 );

        if (y_pos < 4 && carType == ECellType::ECellType_EMPTY)
        {
            _sprite->setLocalPositionY(pos.y + cellInPixels);
            traficMap.setCell(x_pos, y_pos, ECellType::ECellType_EMPTY);
            traficMap.setCell(x_pos, y_pos + 1, ECellType::ECellType_VERTICAL_UP);
            traficMap.setCell(x_pos, y_pos + 2, ECellType::ECellType_VERTICAL_DOWN);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::movePlayerUp(ShapeQuadFixedPtr _sprite)
    {
        mt::vec3f pos = _sprite->getLocalPosition();

        int32_t x_pos = (static_cast<int32_t>(pos.x - borderIndent)) / static_cast<int32_t>(cellInPixels);
        int32_t y_pos = (static_cast<int32_t>(pos.y - borderIndent)) / static_cast<int32_t>(cellInPixels);

        ECellType carType = traficMap.getCell( x_pos, y_pos - 1 );

        if (y_pos > 0 && carType == ECellType::ECellType_EMPTY)
        {
            _sprite->setLocalPositionY(pos.y - cellInPixels);
            traficMap.setCell(x_pos, y_pos - 1, ECellType::ECellType_VERTICAL_UP);
            traficMap.setCell(x_pos, y_pos, ECellType::ECellType_VERTICAL_DOWN);
            traficMap.setCell(x_pos, y_pos + 1, ECellType::ECellType_EMPTY);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::movePlayerDown(ShapeQuadFixedPtr _sprite)
    {
        mt::vec3f pos = _sprite->getLocalPosition();

        int32_t x_pos = (static_cast<int32_t>(pos.x - borderIndent)) / static_cast<int32_t>(cellInPixels);
        int32_t y_pos = (static_cast<int32_t>(pos.y - borderIndent)) / static_cast<int>(cellInPixels);

        if (y_pos + 1 > 4)
        {
            m_semaphoreGameOver->setValue(1);
        }
        else if (y_pos < 4 && traficMap.getCell(x_pos, y_pos + 2) == ECellType::ECellType_EMPTY)
        {
            _sprite->setLocalPositionY(pos.y + cellInPixels);
            traficMap.setCell(x_pos, y_pos, ECellType::ECellType_EMPTY);
            traficMap.setCell(x_pos, y_pos + 1, ECellType::ECellType_PLAYER_V_UP);
            traficMap.setCell(x_pos, y_pos + 2, ECellType::ECellType_PLAYER_V_DOWN);
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::runTaskChains()
    {
        GOAP::SourceInterfacePtr source = GOAP_SERVICE()
            ->makeSource();

        Cook::addWhile(source, [this](const GOAP::SourceInterfacePtr& _scope_while)
        {
            Cook::addPrint(_scope_while, "NEW GAME");

            auto && [race_play, race_end] = Cook::addRace<2>(_scope_while);

            for (auto && [pickable_race, element] : Cook::addRaceZip(race_play, m_cars))
            {
                auto && [race_left, race_right] = Cook::addRace<2>(pickable_race);

                Cook::addPickerableMouseButton(race_left, element.hotspotFirst, EMouseCode::MC_LBUTTON, true, true, nullptr);
                Cook::addPickerableMouseButton(race_right, element.hotspotSecond, EMouseCode::MC_LBUTTON, true, true, nullptr);

                const ConstString name = element.hotspotFirst->getName();

                if (name == "HotSpot_1")
                {
                    Cook::addPrint(race_left, "RACE_LEFT - HotSpot_1");
                    Cook::addPrint(race_right, "RACE_RIGHT - HotSpot_1");
                    Cook::addFunction(race_left, this, &TraficJamSceneEventReceiver::moveLeft, element.sprite);
                    Cook::addFunction(race_right, this, &TraficJamSceneEventReceiver::moveRight, element.sprite);
                }
                else if (name == "HotSpot_3")
                {
                    Cook::addPrint(race_left, "RACE_LEFT - HotSpot_3");
                    Cook::addPrint(race_right, "RACE_RIGHT - HotSpot_3");
                    Cook::addFunction(race_left, this, &TraficJamSceneEventReceiver::moveUp, element.sprite);
                    Cook::addFunction(race_right, this, &TraficJamSceneEventReceiver::moveDown, element.sprite);
                }
                else if (name == "HotSpot_5")
                {
                    Cook::addPrint(race_left, "RACE_LEFT - HotSpot_5");
                    Cook::addPrint(race_right, "RACE_RIGHT - HotSpot_5");
                    Cook::addFunction(race_left, this, &TraficJamSceneEventReceiver::movePlayerUp, element.sprite);
                    Cook::addFunction(race_right, this, &TraficJamSceneEventReceiver::movePlayerDown, element.sprite);
                }
            }

            Cook::addSemaphoreEqual(race_end, m_semaphoreGameOver, 1);

            Cook::addPrint(race_end, "YOU WIN!");

            Cook::addFunction(race_end, this, &TraficJamSceneEventReceiver::resetGame);

            return true;

        });

        m_chain = GOAP_SERVICE()
            ->makeChain(source, nullptr, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE);

        m_chain->run();

    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::clearCars()
    {
        for (Car & car : m_cars)
        {
            car.sprite->removeFromParent();
            car.sprite = nullptr;

            car.hotspotFirst->removeFromParent();
            car.hotspotFirst = nullptr;

            car.hotspotSecond->removeFromParent();
            car.hotspotSecond = nullptr;
        }

        m_cars.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::resetGame()
    {
        this->clearCars();
        this->clearBackground();
        this->clearTaskChain();
        traficMap.clearMap();
        
        this->setupGame();
        this->runTaskChains();
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::clearGameNode()
    {
        if (m_gameNode != nullptr)
        {
            m_gameNode->removeFromParent();
            m_gameNode = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::clearBackground()
    {
        if (m_background != nullptr)
        {
            m_background->removeFromParent();
            m_background = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void TraficJamSceneEventReceiver::clearTaskChain()
    {
        if (m_chain != nullptr)
        {
            m_chain->cancel();
            m_chain = nullptr;
        }
    }
};
