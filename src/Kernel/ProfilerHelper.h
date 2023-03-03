#pragma once

#include "Config/Config.h"

#ifdef MENGINE_PROFILER_ENABLE
#include "Interface/ProfilerSystemInterface.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class ProfilerFrameScope
        {
        public:
            ProfilerFrameScope( const ProfilerDescriptionInterface * _description )
            {
                if( SERVICE_IS_INITIALIZE( ProfilerSystemInterface ) == true )
                {
                    m_profiler = PROFILER_SYSTEM()
                        ->addFrame( _description );
                }
            }

            ~ProfilerFrameScope()
            {
            }

        protected:
            ProfilerInterfacePtr m_profiler;
        };
        //////////////////////////////////////////////////////////////////////////
        class ProfilerCategoryScope
        {
        public:
            ProfilerCategoryScope( const ProfilerDescriptionInterface * _description )
            {
                if( SERVICE_IS_INITIALIZE( ProfilerSystemInterface ) == true )
                {
                    m_profiler = PROFILER_SYSTEM()
                        ->addCategory( _description );
                }
            }

            ~ProfilerCategoryScope()
            {
            }

        protected:
            ProfilerInterfacePtr m_profiler;
        };
        //////////////////////////////////////////////////////////////////////////
        class ProfilerThreadScope
        {
        public:
            ProfilerThreadScope( const Char * _name )
            {
                if( SERVICE_IS_INITIALIZE( ProfilerSystemInterface ) == true )
                {
                    m_profiler = PROFILER_SYSTEM()\
                        ->addThread( _name );
                }
            }

            ~ProfilerThreadScope()
            {
            }

        protected:
            ProfilerInterfacePtr m_profiler;
        };
    }
}
#endif


#ifdef MENGINE_PROFILER_ENABLE
#ifndef MENGINE_PROFILER_FRAME
#define MENGINE_PROFILER_FRAME(Name)\
    static const Mengine::ProfilerDescriptionInterface * MENGINE_PP_CONCATENATE( __mengine_profiler_description, MENGINE_CODE_LINE ) = nullptr;\
    if( SERVICE_IS_INITIALIZE( Mengine::ProfilerSystemInterface ) == true && MENGINE_PP_CONCATENATE( __mengine_profiler_description, MENGINE_CODE_LINE ) == nullptr )\
    {\
        MENGINE_PP_CONCATENATE( __mengine_profiler_description, MENGINE_CODE_LINE ) = PROFILER_SYSTEM()\
            ->createDescription( MENGINE_CODE_FUNCTION, MENGINE_CODE_FILE, MENGINE_CODE_LINE );\
    }\
    Mengine::Detail::ProfilerFrameScope MENGINE_PP_CONCATENATE( __mengine_profiler_frame_scope, MENGINE_CODE_LINE )(MENGINE_PP_CONCATENATE( __mengine_profiler_description, MENGINE_CODE_LINE ));
#endif
#else
#define MENGINE_PROFILER_FRAME(Name)
#endif

#ifdef MENGINE_PROFILER_ENABLE
#ifndef MENGINE_PROFILER_CATEGORY
#define MENGINE_PROFILER_CATEGORY()\
    static const Mengine::ProfilerDescriptionInterface * MENGINE_PP_CONCATENATE( __mengine_profiler_description, MENGINE_CODE_LINE ) = nullptr;\
    if( SERVICE_IS_INITIALIZE( Mengine::ProfilerSystemInterface ) == true && MENGINE_PP_CONCATENATE( __mengine_profiler_description, MENGINE_CODE_LINE ) == nullptr )\
    {\
        MENGINE_PP_CONCATENATE( __mengine_profiler_description, MENGINE_CODE_LINE ) = PROFILER_SYSTEM()\
            ->createDescription( MENGINE_CODE_FUNCTION, MENGINE_CODE_FILE, MENGINE_CODE_LINE );\
    }\
    Mengine::Detail::ProfilerCategoryScope MENGINE_PP_CONCATENATE( __mengine_profiler_frame_scope, MENGINE_CODE_LINE )(MENGINE_PP_CONCATENATE( __mengine_profiler_description, MENGINE_CODE_LINE ));
#endif
#else
#define MENGINE_PROFILER_CATEGORY()
#endif

#ifdef MENGINE_PROFILER_ENABLE
#ifndef MENGINE_PROFILER_THREAD
#define MENGINE_PROFILER_THREAD(Name)\
    Mengine::Detail::ProfilerThreadScope MENGINE_PP_CONCATENATE( __mengine_profiler_thread, MENGINE_CODE_LINE )( Name );
#endif
#else
#define MENGINE_PROFILER_THREAD(Name)
#endif

#ifdef MENGINE_PROFILER_ENABLE
#ifndef MENGINE_PROFILER_BEGIN_APPLICATION
#define MENGINE_PROFILER_BEGIN_APPLICATION()\
    if( SERVICE_IS_INITIALIZE( ProfilerSystemInterface ) == true )\
    {\
        PROFILER_SYSTEM()\
            ->beginApplication();\
    }
#endif
#else
#define MENGINE_PROFILER_BEGIN_APPLICATION()
#endif

#ifdef MENGINE_PROFILER_ENABLE
#ifndef MENGINE_PROFILER_END_APPLICATION
#define MENGINE_PROFILER_END_APPLICATION()\
    if( SERVICE_IS_INITIALIZE( ProfilerSystemInterface ) == true )\
    {\
        PROFILER_SYSTEM()\
            ->endApplication();\
    }
#endif
#else
#define MENGINE_PROFILER_END_APPLICATION()
#endif
