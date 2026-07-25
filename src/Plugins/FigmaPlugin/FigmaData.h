#pragma once

#include "FigmaDataInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FigmaData
        : public FigmaDataInterface
    {
        DECLARE_FACTORABLE( FigmaData );

    public:
        FigmaData();
        ~FigmaData() override;

    public:
        bool loadDocument( const MemoryInterfacePtr & _memory, const FilePath & _sourceName ) override;
        bool loadUX( const void * _memory, size_t _size ) override;

    public:
        bool acquire() override;
        void release() override;

    public:
        figma_runtime_t * getFigmaRuntime() const override;
        figma_document_t * getFigmaDocument() const override;

    protected:
        MemoryInterfacePtr m_memory;
        figma_runtime_t * m_runtime;
        figma_document_t * m_document;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FigmaData, FigmaDataInterface> FigmaDataPtr;
    //////////////////////////////////////////////////////////////////////////
}
