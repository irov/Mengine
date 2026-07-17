#include "Arena3DSceneEventReceiver.h"
#include "Arena3DHelper.h"

#include "Arena3DGeneratedCollision.h"

#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdMath.h"

#include "math/uv4.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static constexpr size_t weaponIndex( Arena3D::WeaponType _weapon )
        {
            return static_cast<size_t>(_weapon);
        }
        //////////////////////////////////////////////////////////////////////////
        static size_t explosionIndex( Arena3D::WeaponType _weapon )
        {
            switch( _weapon )
            {
            case Arena3D::WeaponType::RocketLauncher: return 0;
            case Arena3D::WeaponType::Plasmagun: return 1;
            case Arena3D::WeaponType::GrenadeLauncher: return 2;
            default: return 3;
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static float explosionEndScale( Arena3D::WeaponType _weapon )
        {
            static const float scales[] = {4.8f, 1.8f, 4.2f};
            const size_t index = explosionIndex( _weapon );
            return index < 3 ? scales[index] : 0.f;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool impactSurfaceNormal( int32_t _code, mt::vec3f * _normal )
        {
            switch( static_cast<Arena3D::ImpactNormal>(_code) )
            {
            case Arena3D::ImpactNormal::NegativeX:
                *_normal = { -1.f, 0.f, 0.f };
                return true;
            case Arena3D::ImpactNormal::PositiveX:
                *_normal = { 1.f, 0.f, 0.f };
                return true;
            case Arena3D::ImpactNormal::NegativeY:
                *_normal = { 0.f, -1.f, 0.f };
                return true;
            case Arena3D::ImpactNormal::PositiveY:
                *_normal = { 0.f, 1.f, 0.f };
                return true;
            case Arena3D::ImpactNormal::NegativeZ:
                *_normal = { 0.f, 0.f, -1.f };
                return true;
            case Arena3D::ImpactNormal::PositiveZ:
                *_normal = { 0.f, 0.f, 1.f };
                return true;
            case Arena3D::ImpactNormal::Unknown: return false;
            }
            return false;
        }
        //////////////////////////////////////////////////////////////////////////
        static float dot3( const mt::vec3f & _left, const mt::vec3f & _right )
        {
            return _left.x * _right.x + _left.y * _right.y + _left.z * _right.z;
        }
        //////////////////////////////////////////////////////////////////////////
        static const kf_aabb_t * findCollisionBrush( uint32_t _brushId )
        {
            for( size_t index = 0; index != Arena3DGenerated::CollisionBoxCount; ++index )
            {
                const kf_aabb_t & candidate = Arena3DGenerated::CollisionBoxes[index];
                if( candidate.id != _brushId )
                {
                    continue;
                }
                return &candidate;
            }

            return nullptr;
        }
        //////////////////////////////////////////////////////////////////////////
        static ResourceMesh3DPtr createClippedDecalResource( const ConstString & _name, uint32_t _brushId, Arena3D::ImpactNormal _normalCode, const mt::vec3f & _normal, const mt::vec3f & _impactPosition, float _scale )
        {
            if( _scale <= 0.f )
            {
                return nullptr;
            }

            mt::vec3f tangentU;
            mt::vec3f tangentV;
            switch( _normalCode )
            {
            case Arena3D::ImpactNormal::NegativeX:
                tangentU = { 0.f, -1.f, 0.f };
                tangentV = { 0.f, 0.f, 1.f };
                break;
            case Arena3D::ImpactNormal::PositiveX:
                tangentU = { 0.f, 1.f, 0.f };
                tangentV = { 0.f, 0.f, 1.f };
                break;
            case Arena3D::ImpactNormal::NegativeY:
                tangentU = { 1.f, 0.f, 0.f };
                tangentV = { 0.f, 0.f, 1.f };
                break;
            case Arena3D::ImpactNormal::PositiveY:
                tangentU = { 0.f, 0.f, 1.f };
                tangentV = { 1.f, 0.f, 0.f };
                break;
            case Arena3D::ImpactNormal::NegativeZ:
                tangentU = { -1.f, 0.f, 0.f };
                tangentV = { 0.f, 1.f, 0.f };
                break;
            case Arena3D::ImpactNormal::PositiveZ:
                tangentU = { 1.f, 0.f, 0.f };
                tangentV = { 0.f, 1.f, 0.f };
                break;
            case Arena3D::ImpactNormal::Unknown: return nullptr;
            }

            const float halfScale = _scale * 0.5f;
            const float centerU = dot3( _impactPosition, tangentU );
            const float centerV = dot3( _impactPosition, tangentV );
            const float originalMinU = centerU - halfScale;
            const float originalMaxU = centerU + halfScale;
            const float originalMinV = centerV - halfScale;
            const float originalMaxV = centerV + halfScale;
            float clippedMinU = originalMinU;
            float clippedMaxU = originalMaxU;
            float clippedMinV = originalMinV;
            float clippedMaxV = originalMaxV;
            float planeDistance = dot3( _impactPosition, _normal );

            const kf_aabb_t * brush = findCollisionBrush( _brushId );
            if( brush != nullptr )
            {
                const float inverseScale = 1.f / static_cast<float>(KF_FIXED_SCALE);
                const mt::vec3f minimum(
                    static_cast<float>(brush->minimum[0]) * inverseScale,
                    static_cast<float>(brush->minimum[1]) * inverseScale,
                    static_cast<float>(brush->minimum[2]) * inverseScale );
                const mt::vec3f maximum(
                    static_cast<float>(brush->maximum[0]) * inverseScale,
                    static_cast<float>(brush->maximum[1]) * inverseScale,
                    static_cast<float>(brush->maximum[2]) * inverseScale );
                const mt::vec3f center = (minimum + maximum) * 0.5f;
                const mt::vec3f extent = (maximum - minimum) * 0.5f;
                const float projectedCenterU = dot3( center, tangentU );
                const float projectedCenterV = dot3( center, tangentV );
                const float projectedExtentU = Mengine::StdMath::fabs( tangentU.x ) * extent.x + Mengine::StdMath::fabs( tangentU.y ) * extent.y + Mengine::StdMath::fabs( tangentU.z ) * extent.z;
                const float projectedExtentV = Mengine::StdMath::fabs( tangentV.x ) * extent.x + Mengine::StdMath::fabs( tangentV.y ) * extent.y + Mengine::StdMath::fabs( tangentV.z ) * extent.z;

                clippedMinU = Mengine::StdAlgorithm::max( clippedMinU, projectedCenterU - projectedExtentU );
                clippedMaxU = Mengine::StdAlgorithm::min( clippedMaxU, projectedCenterU + projectedExtentU );
                clippedMinV = Mengine::StdAlgorithm::max( clippedMinV, projectedCenterV - projectedExtentV );
                clippedMaxV = Mengine::StdAlgorithm::min( clippedMaxV, projectedCenterV + projectedExtentV );

                mt::vec3f facePoint = _impactPosition;
                switch( _normalCode )
                {
                case Arena3D::ImpactNormal::NegativeX:
                    facePoint.x = minimum.x;
                    break;
                case Arena3D::ImpactNormal::PositiveX:
                    facePoint.x = maximum.x;
                    break;
                case Arena3D::ImpactNormal::NegativeY:
                    facePoint.y = minimum.y;
                    break;
                case Arena3D::ImpactNormal::PositiveY:
                    facePoint.y = maximum.y;
                    break;
                case Arena3D::ImpactNormal::NegativeZ:
                    facePoint.z = minimum.z;
                    break;
                case Arena3D::ImpactNormal::PositiveZ:
                    facePoint.z = maximum.z;
                    break;
                case Arena3D::ImpactNormal::Unknown: break;
                }
                planeDistance = dot3( facePoint, _normal );
            }

            if( clippedMaxU - clippedMinU < 0.01f || clippedMaxV - clippedMinV < 0.01f )
            {
                return nullptr;
            }

            const float u0 = (clippedMinU - originalMinU) / _scale;
            const float u1 = (clippedMaxU - originalMinU) / _scale;
            const float v0 = (clippedMinV - originalMinV) / _scale;
            const float v1 = (clippedMaxV - originalMinV) / _scale;
            const float decalPlane = planeDistance + 0.012f;
            const float tangentCoordinatesU[] = {clippedMinU, clippedMaxU, clippedMaxU, clippedMinU};
            const float tangentCoordinatesV[] = {clippedMinV, clippedMinV, clippedMaxV, clippedMaxV};
            const mt::vec2f textureCoordinates[] = {{u0, 1.f - v0}, {u1, 1.f - v0}, {u1, 1.f - v1}, {u0, 1.f - v1}};

            VectorRenderVertex3D vertices;
            vertices.resize( 4 );
            for( size_t index = 0; index != vertices.size(); ++index )
            {
                RenderVertex3D & vertex = vertices[index];
                vertex.position = tangentU * tangentCoordinatesU[index] + tangentV * tangentCoordinatesV[index] + _normal * decalPlane;
                vertex.normal = _normal;
                vertex.tangent = {tangentU.x, tangentU.y, tangentU.z, 1.f};
                vertex.color = 0xFFFFFFFFu;
                vertex.uv[0] = textureCoordinates[index];
                vertex.uv[1] = textureCoordinates[index];
                vertex._padding = 0;
            }

            VectorRenderIndex32 indices = {0, 1, 2, 0, 2, 3};
            ResourceMesh3DPtr resource = PROTOTYPE_SERVICE()->generatePrototype(
                STRINGIZE_STRING_LOCAL( "Resource" ), STRINGIZE_STRING_LOCAL( "ResourceMesh3D" ), MENGINE_DOCUMENT_FUNCTION );
            MENGINE_ASSERTION_MEMORY_PANIC( resource, "Arena3D failed to allocate clipped decal mesh" );
            resource->setName( _name );
            resource->setVertices( vertices );
            resource->setIndices( indices );
            if( resource->initialize() == false )
            {
                return nullptr;
            }

            return resource;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::spawnExplosionCloud_( const Arena3D::ServerEvent & _event )
    {
        const size_t type = Detail::explosionIndex( _event.weapon );
        if( type >= ExplosionTypeCount )
        {
            return;
        }

        ExplosionCloudVisual * selected = nullptr;
        for( ExplosionCloudVisual & cloud : m_explosionClouds[type] )
        {
            if( cloud.active == false )
            {
                selected = &cloud;
                break;
            }
            if( selected == nullptr || cloud.age > selected->age )
            {
                selected = &cloud;
            }
        }
        if( selected == nullptr )
        {
            return;
        }

        static const float durations[] = {0.46f, 0.24f, 0.52f};
        static const float startScales[] = {0.35f, 0.22f, 0.32f};
        selected->age = 0.f;
        selected->duration = durations[type];
        selected->startScale = startScales[type];
        selected->endScale = Detail::explosionEndScale( _event.weapon );
        selected->active = true;
        selected->node->getTransformation()->setLocalPosition( Helper::kinefixVec3ToVec3f( _event.position ) );
        selected->node->getTransformation()->setLocalScale( {selected->startScale, selected->startScale, selected->startScale} );
        selected->node->getTransformation()->setLocalOrientationY( static_cast<float>(_event.id & 255u) * 0.024543693f );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::spawnImpactFlash_( const Arena3D::ServerEvent & _event )
    {
        ImpactFlashVisual * selected = nullptr;
        for( ImpactFlashVisual & flash : m_impactFlashes )
        {
            if( flash.active == false )
            {
                selected = &flash;
                break;
            }
            if( selected == nullptr || flash.age > selected->age )
            {
                selected = &flash;
            }
        }
        if( selected == nullptr )
        {
            return;
        }

        static const float startScales[] = {0.08f, 0.14f, 0.10f, 0.09f, 0.14f, 0.06f};
        static const float endScales[] = {0.34f, 0.62f, 0.46f, 0.40f, 0.60f, 0.25f};
        static const mt::vec3f colors[] = {
            {1.f, 0.62f, 0.18f}, {1.f, 0.22f, 0.04f}, {0.25f, 0.75f, 1.f},
            {0.05f, 1.f, 0.72f}, {1.f, 0.55f, 0.10f}, {1.f, 0.78f, 0.28f}};
        const size_t weapon = Detail::weaponIndex( _event.weapon );
        mt::vec3f position = Helper::kinefixVec3ToVec3f( _event.position );
        mt::vec3f normal;
        if( _event.type == Arena3D::EventType::Impact && Detail::impactSurfaceNormal( _event.amount, &normal ) == true )
        {
            position += normal * 0.035f;
        }

        selected->age = 0.f;
        selected->duration = 0.18f;
        selected->startScale = startScales[weapon];
        selected->endScale = endScales[weapon];
        selected->active = true;
        selected->node->getTransformation()->setLocalPosition( position );
        selected->node->getTransformation()->setLocalScale( {selected->startScale, selected->startScale, selected->startScale} );
        selected->node->getTransformation()->setLocalOrientation( {
            static_cast<float>((_event.id >> 3) & 31u) * 0.11f,
            static_cast<float>((_event.id >> 8) & 31u) * 0.13f,
            static_cast<float>((_event.id >> 13) & 31u) * 0.17f} );
        const mt::vec3f color = _event.type == Arena3D::EventType::Hit ? mt::vec3f( 1.f, 0.04f, 0.02f ) : colors[weapon];
        selected->node->getRender()->setLocalColorRGBA( color.x, color.y, color.z, 0.92f );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::spawnDecal_( const Arena3D::ServerEvent & _event )
    {
        mt::vec3f normal;
        if( Detail::impactSurfaceNormal( _event.amount, &normal ) == false )
        {
            return;
        }

        DecalVisual * selected = nullptr;
        for( DecalVisual & decal : m_decals )
        {
            if( decal.active == false )
            {
                selected = &decal;
                break;
            }
            if( selected == nullptr || decal.age > selected->age )
            {
                selected = &decal;
            }
        }
        if( selected == nullptr )
        {
            return;
        }

        static const float impactScales[] = {0.22f, 0.f, 0.40f, 0.f, 0.f, 0.16f};
        const float explosionScale = Detail::explosionEndScale( _event.weapon );
        const float scale = explosionScale > 0.f ? explosionScale * 0.5f : impactScales[Detail::weaponIndex( _event.weapon )];
        const size_t slot = static_cast<size_t>(selected - m_decals.data());
        const ConstString decalName = Helper::stringizeStringFormat( "Arena3D_Decal_%u", static_cast<uint32_t>(slot) );
        ResourceMesh3DPtr resource = Detail::createClippedDecalResource( decalName, _event.targetId,
            static_cast<Arena3D::ImpactNormal>(_event.amount), normal, Helper::kinefixVec3ToVec3f( _event.position ), scale );
        if( resource == nullptr )
        {
            return;
        }

        if( selected->node != nullptr )
        {
            selected->node->removeFromParent();
            selected->node = nullptr;
        }
        if( selected->resource != nullptr )
        {
            selected->resource->finalize();
        }

        selected->resource = resource;
        selected->node = this->createMeshNode_( decalName, selected->resource, m_decalTexture,
            STRINGIZE_STRING_LOCAL( "Arena3D_Decal" ), m_worldCamera, m_postProcess.get() );
        selected->age = 0.f;
        selected->duration = 12.f;
        selected->active = true;
        selected->node->getRender()->setLocalColorRGBA( 1.f, 1.f, 1.f, 0.88f );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::rememberGrenadeImpact_( const Arena3D::ServerEvent & _event )
    {
        GrenadeImpactVisual * selected = nullptr;
        for( GrenadeImpactVisual & impact : m_grenadeImpacts )
        {
            if( impact.active == false )
            {
                selected = &impact;
                break;
            }
            if( selected == nullptr || impact.event.tick < selected->event.tick )
            {
                selected = &impact;
            }
        }
        if( selected == nullptr )
        {
            return;
        }

        selected->event = _event;
        selected->active = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::spawnGrenadeExplosionDecal_( const Arena3D::ServerEvent & _event )
    {
        GrenadeImpactVisual * selected = nullptr;
        kf_fixed_t selectedDistance;
        const kf_fixed_t maximumDistanceSquared = kf_fixed_from_int( 16 );
        for( GrenadeImpactVisual & impact : m_grenadeImpacts )
        {
            if( impact.active == false || impact.event.actorId != _event.actorId || impact.event.tick > _event.tick || _event.tick - impact.event.tick > 500u )
            {
                continue;
            }
            const kf_fixed_t distance = kf_vec3_length_squared( kf_vec3_sub( impact.event.position, _event.position ) );
            if( distance > maximumDistanceSquared )
            {
                continue;
            }
            if( selected == nullptr || distance < selectedDistance || (distance == selectedDistance && impact.event.id > selected->event.id) )
            {
                selected = &impact;
                selectedDistance = distance;
            }
        }
        if( selected == nullptr )
        {
            return;
        }

        this->spawnDecal_( selected->event );
        selected->active = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::spawnDeathBurst_( const Arena3D::ServerEvent & _event )
    {
        DeathBurstVisual * selected = nullptr;
        for( DeathBurstVisual & burst : m_deathBursts )
        {
            if( burst.active == false )
            {
                selected = &burst;
                break;
            }
            if( selected == nullptr || burst.age > selected->age )
            {
                selected = &burst;
            }
        }
        if( selected == nullptr )
        {
            return;
        }

        static const mt::vec3f velocities[] = {
            {3.8f, 6.8f, 0.6f}, {-3.4f, 7.5f, 1.5f}, {1.2f, 8.6f, -3.4f},
            {-1.7f, 5.9f, -3.8f}, {4.2f, 5.2f, 2.8f}, {-4.0f, 6.2f, -2.1f}};
        const mt::vec3f origin = Helper::kinefixVec3ToVec3f( _event.position ) + mt::vec3f( 0.f, 0.85f, 0.f );
        const float phase = static_cast<float>(_event.id & 255u) * 0.024543693f;
        const float sinPhase = StdMath::sinf( phase );
        const float cosPhase = StdMath::cosf( phase );
        selected->age = 0.f;
        selected->duration = 1.05f;
        selected->active = true;
        for( size_t index = 0; index != selected->nodes.size(); ++index )
        {
            const mt::vec3f sourceVelocity = velocities[index];
            selected->velocities[index] = {sourceVelocity.x * cosPhase + sourceVelocity.z * sinPhase, sourceVelocity.y, sourceVelocity.z * cosPhase - sourceVelocity.x * sinPhase};
            const float sign = (index & 1u) == 0u ? 1.f : -1.f;
            selected->angularVelocities[index] = {sign * (4.2f + static_cast<float>(index) * 0.4f), -sign * (3.6f + static_cast<float>(index) * 0.3f), sign * 5.1f};
            TransformationInterface * transformation = selected->nodes[index]->getTransformation();
            transformation->setLocalPosition( origin + mt::vec3f( static_cast<float>(index % 3u) * 0.08f - 0.08f, static_cast<float>(index / 3u) * 0.15f, 0.f ) );
            const float scale = 0.22f + static_cast<float>(index % 3u) * 0.035f;
            transformation->setLocalScale( {scale, scale, scale} );
            transformation->setLocalOrientation( {phase + static_cast<float>(index), phase * 0.5f - static_cast<float>(index), phase * 0.25f + static_cast<float>(index) * 0.7f} );
            selected->nodes[index]->getRender()->setLocalColorRGBA( 0.72f, 0.10f + static_cast<float>(index) * 0.018f, 0.045f, 1.f );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::updateExplosionClouds_( float _seconds )
    {
        m_visibleExplosionClouds = 0;
        for( auto & pool : m_explosionClouds )
        {
            for( ExplosionCloudVisual & cloud : pool )
            {
                if( cloud.active == false )
                {
                    continue;
                }
                cloud.age += _seconds;
                if( cloud.age >= cloud.duration )
                {
                    cloud.active = false;
                    cloud.node->getTransformation()->setLocalPosition( {0.f, -10000.f, 0.f} );
                    cloud.node->getTransformation()->setLocalScale( {0.001f, 0.001f, 0.001f} );
                    continue;
                }

                const float linear = cloud.age / cloud.duration;
                const float eased = 1.f - (1.f - linear) * (1.f - linear);
                const float scale = cloud.startScale + (cloud.endScale - cloud.startScale) * eased;
                cloud.node->getTransformation()->setLocalScale( {scale, scale, scale} );
                cloud.node->getTransformation()->setLocalOrientationX( cloud.age * 1.7f );
                cloud.node->getTransformation()->setLocalOrientationZ( cloud.age * -1.2f );
                ++m_visibleExplosionClouds;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::updateImpactEffects_( float _seconds )
    {
        m_visibleImpactFlashes = 0;
        for( ImpactFlashVisual & flash : m_impactFlashes )
        {
            if( flash.active == false )
            {
                continue;
            }
            flash.age += _seconds;
            if( flash.age >= flash.duration )
            {
                flash.active = false;
                flash.node->getTransformation()->setLocalPosition( {0.f, -10000.f, 0.f} );
                flash.node->getTransformation()->setLocalScale( {0.001f, 0.001f, 0.001f} );
                flash.node->getRender()->setLocalAlpha( 0.f );
                continue;
            }

            const float linear = flash.age / flash.duration;
            const float eased = 1.f - (1.f - linear) * (1.f - linear);
            const float scale = flash.startScale + (flash.endScale - flash.startScale) * eased;
            flash.node->getTransformation()->setLocalScale( {scale, scale, scale} );
            flash.node->getTransformation()->setLocalOrientationY( flash.age * 7.f );
            flash.node->getRender()->setLocalAlpha( (1.f - linear) * 0.92f );
            ++m_visibleImpactFlashes;
        }

        m_visibleDecals = 0;
        for( DecalVisual & decal : m_decals )
        {
            if( decal.active == false )
            {
                continue;
            }
            decal.age += _seconds;
            if( decal.age >= decal.duration )
            {
                decal.active = false;
                decal.node->getTransformation()->setLocalPosition( {0.f, -10000.f, 0.f} );
                decal.node->getTransformation()->setLocalScale( {0.001f, 0.001f, 0.001f} );
                decal.node->getRender()->setLocalAlpha( 0.f );
                continue;
            }

            const float fadeStart = decal.duration - 2.f;
            const float alpha = decal.age <= fadeStart ? 0.88f : 0.88f * (decal.duration - decal.age) / (decal.duration - fadeStart);
            decal.node->getRender()->setLocalAlpha( alpha );
            ++m_visibleDecals;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::updateDeathBursts_( float _seconds )
    {
        m_visibleDeathShards = 0;
        for( DeathBurstVisual & burst : m_deathBursts )
        {
            if( burst.active == false )
            {
                continue;
            }
            burst.age += _seconds;
            if( burst.age >= burst.duration )
            {
                burst.active = false;
                for( NodePtr & node : burst.nodes )
                {
                    node->getTransformation()->setLocalPosition( {0.f, -10000.f, 0.f} );
                    node->getTransformation()->setLocalScale( {0.001f, 0.001f, 0.001f} );
                    node->getRender()->setLocalAlpha( 0.f );
                }
                continue;
            }

            const float fade = burst.age < 0.7f ? 1.f : (burst.duration - burst.age) / (burst.duration - 0.7f);
            for( size_t index = 0; index != burst.nodes.size(); ++index )
            {
                TransformationInterface * transformation = burst.nodes[index]->getTransformation();
                transformation->moveLocalPosition( burst.velocities[index] * _seconds );
                burst.velocities[index].y -= 13.f * _seconds;
                transformation->setLocalOrientation( transformation->getLocalOrientation() + burst.angularVelocities[index] * _seconds );
                burst.nodes[index]->getRender()->setLocalAlpha( fade );
                ++m_visibleDeathShards;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::clearImpactEffects_()
    {
        for( ImpactFlashVisual & flash : m_impactFlashes )
        {
            flash.active = false;
            if( flash.node == nullptr )
            {
                continue;
            }
            flash.node->getTransformation()->setLocalPosition( {0.f, -10000.f, 0.f} );
            flash.node->getTransformation()->setLocalScale( {0.001f, 0.001f, 0.001f} );
            flash.node->getRender()->setLocalAlpha( 0.f );
        }
        for( DecalVisual & decal : m_decals )
        {
            decal.active = false;
            if( decal.node == nullptr )
            {
                continue;
            }
            decal.node->getTransformation()->setLocalPosition( {0.f, -10000.f, 0.f} );
            decal.node->getTransformation()->setLocalScale( {0.001f, 0.001f, 0.001f} );
            decal.node->getRender()->setLocalAlpha( 0.f );
        }
        for( GrenadeImpactVisual & impact : m_grenadeImpacts )
        {
            impact = GrenadeImpactVisual{};
        }
        m_visibleImpactFlashes = 0;
        m_visibleDecals = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::clearDeathBursts_()
    {
        for( DeathBurstVisual & burst : m_deathBursts )
        {
            burst.active = false;
            for( NodePtr & node : burst.nodes )
            {
                if( node == nullptr )
                {
                    continue;
                }
                node->getTransformation()->setLocalPosition( {0.f, -10000.f, 0.f} );
                node->getTransformation()->setLocalScale( {0.001f, 0.001f, 0.001f} );
                node->getRender()->setLocalAlpha( 0.f );
            }
        }
        m_visibleDeathShards = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DSceneEventReceiver::playSound_( size_t _index )
    {
        if( _index >= m_sounds.size() || m_sounds[_index] == nullptr )
        {
            return;
        }
        AnimationInterface * animation = m_sounds[_index]->getAnimation();
        if( animation->isPlay() == true )
        {
            animation->stop();
        }
        animation->play( 0.f );
    }
    //////////////////////////////////////////////////////////////////////////
}
