#include "RenderTexturePrefetchQueue.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/VocabularyHelper.h"

#include "Config/StdAlgorithm.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    constexpr uint32_t TEXTURE_PREFETCH_MAX_PREPARING = 2;
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool isSameTexturePath( const ContentInterfacePtr & _left, const ContentInterfacePtr & _right )
        {
            const FileGroupInterfacePtr & leftFileGroup = _left->getFileGroup();
            const FileGroupInterfacePtr & rightFileGroup = _right->getFileGroup();

            if( leftFileGroup != rightFileGroup )
            {
                return false;
            }

            const FilePath & leftFilePath = _left->getFilePath();
            const FilePath & rightFilePath = _right->getFilePath();
            bool same = leftFilePath == rightFilePath;

            return same;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool isValidTextureContent( const ContentInterfacePtr & _content )
        {
            if( _content == nullptr )
            {
                return false;
            }

            const FileGroupInterfacePtr & fileGroup = _content->getFileGroup();

            if( fileGroup == nullptr )
            {
                return false;
            }

            const FilePath & filePath = _content->getFilePath();

            if( filePath.empty() == true )
            {
                return false;
            }

            const ConstString & codecType = _content->getCodecType();

            if( codecType.empty() == true )
            {
                return false;
            }

            const ConstString & decoderFactory = STRINGIZE_STRING_LOCAL( "DecoderFactory" );
            bool registered = VOCABULARY_HAS( decoderFactory, codecType );

            return registered;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTexturePrefetchQueue::RenderTexturePrefetchQueue()
        : m_stopped( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTexturePrefetchQueue::~RenderTexturePrefetchQueue()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTexturePrefetchInterfacePtr RenderTexturePrefetchQueue::request( const ContentInterfacePtr & _content, uint32_t _codecFlags, const LambdaRenderTexturePrefetch & _callback, const DocumentInterfacePtr & _doc )
    {
        RenderTexturePrefetchJobPtr job;
        bool rejected = m_stopped;

        if( rejected == false )
        {
            rejected = Detail::isValidTextureContent( _content ) == false;
        }

        if( rejected == false )
        {
            this->removeUnusedRequests_();

            const ConstString & codecType = _content->getCodecType();

            for( const RenderTexturePrefetchJobPtr & candidate : m_jobs )
            {
                ERenderTexturePrefetchState state = candidate->getState();

                if( state == ERenderTexturePrefetchState::ERTPS_FAILED || state == ERenderTexturePrefetchState::ERTPS_CANCELLED )
                {
                    continue;
                }

                if( this->isJobUsed_( candidate ) == false )
                {
                    continue;
                }

                const ContentInterfacePtr & candidateContent = candidate->getContent();

                if( Detail::isSameTexturePath( candidateContent, _content ) == false )
                {
                    continue;
                }

                const ConstString & candidateCodecType = candidateContent->getCodecType();

                if( candidate->getCodecFlags() != _codecFlags )
                {
                    rejected = true;

                    break;
                }

                if( candidateCodecType != codecType )
                {
                    rejected = true;

                    break;
                }

                job = candidate;

                break;
            }
        }

        if( job == nullptr )
        {
            job = Helper::makeFactorableUnique<RenderTexturePrefetchJob>( _doc );
            job->initialize( _content, _codecFlags );

            if( rejected == true )
            {
                job->reject();
            }

            if( m_stopped == false )
            {
                m_jobs.emplace_back( job );
            }
        }

        RenderTexturePrefetchPtr request = Helper::makeFactorableUnique<RenderTexturePrefetch>( _doc );
        request->initialize( job, _content, _callback );

        if( m_stopped == true )
        {
            request->finalize();

            return request;
        }

        m_requests.emplace_back( request );

        return request;
    }
    //////////////////////////////////////////////////////////////////////////
    bool RenderTexturePrefetchQueue::isJobUsed_( const RenderTexturePrefetchJobPtr & _job ) const
    {
        if( _job->getrefcount() != 1 )
        {
            return true;
        }

        const RenderTextureInterfacePtr & texture = _job->getTexture();

        if( texture == nullptr )
        {
            return false;
        }

        bool used = texture->getrefcount() != 1;

        return used;
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexturePrefetchQueue::removeUnusedRequests_()
    {
        StdAlgorithm::erase_if( m_requests, []( const RenderTexturePrefetchPtr & _request )
        {
            bool unused = _request->getrefcount() == 1;

            return unused;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexturePrefetchQueue::update()
    {
        if( m_stopped == true )
        {
            return;
        }

        this->removeUnusedRequests_();

        for( const RenderTexturePrefetchJobPtr & job : m_jobs )
        {
            if( this->isJobUsed_( job ) == false )
            {
                job->cancel();
            }

            job->update();
        }

        StdAlgorithm::erase_if( m_jobs, [this]( const RenderTexturePrefetchJobPtr & _job )
        {
            if( this->isJobUsed_( _job ) == true )
            {
                return false;
            }

            bool preparing = _job->isPreparing();

            return preparing == false;
        } );

        uint32_t preparing = 0;

        for( const RenderTexturePrefetchJobPtr & job : m_jobs )
        {
            if( job->isPreparing() == true )
            {
                ++preparing;
            }
        }

        for( const RenderTexturePrefetchJobPtr & job : m_jobs )
        {
            if( preparing >= TEXTURE_PREFETCH_MAX_PREPARING )
            {
                break;
            }

            if( job->getState() != ERenderTexturePrefetchState::ERTPS_QUEUED )
            {
                continue;
            }

            if( job->start() == false )
            {
                continue;
            }

            ++preparing;
        }

        VectorRequests notifications = m_requests;

        for( const RenderTexturePrefetchPtr & request : notifications )
        {
            if( m_stopped == true )
            {
                break;
            }

            request->notify();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexturePrefetchQueue::upload()
    {
        if( m_stopped == true )
        {
            return;
        }

        this->removeUnusedRequests_();

        for( const RenderTexturePrefetchJobPtr & job : m_jobs )
        {
            if( job->getState() != ERenderTexturePrefetchState::ERTPS_READY_FOR_UPLOAD )
            {
                continue;
            }

            if( job->getrefcount() == 1 )
            {
                continue;
            }

            job->upload();

            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void RenderTexturePrefetchQueue::stop()
    {
        if( m_stopped == true )
        {
            return;
        }

        m_stopped = true;

        for( const RenderTexturePrefetchPtr & request : m_requests )
        {
            request->finalize();
        }

        m_requests.clear();

        for( const RenderTexturePrefetchJobPtr & job : m_jobs )
        {
            job->stop();
        }

        m_jobs.clear();
    }
    //////////////////////////////////////////////////////////////////////////
}
