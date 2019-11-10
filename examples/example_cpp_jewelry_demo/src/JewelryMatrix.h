#pragma once

#include "Kernel/Factorable.h"

#include "GOAP/Event.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Jewelry> JewelryPtr;
    //////////////////////////////////////////////////////////////////////////
    class JewelryMatrix
        : public Factorable
    {
    public:
        JewelryMatrix();
        ~JewelryMatrix() override;

    public:
        bool initialize( const GOAP::EventPtr & _event, uint32_t _columnCount, uint32_t _rowCount );
        void finalize();

    public:
        uint32_t getColumnCount() const;
        uint32_t getRowCount() const;

    public:
        void removeJewelry( const JewelryPtr & _jewelry );
        void setJewelry( uint32_t _column, uint32_t _row, const JewelryPtr & _jewelry );
        void moveJewelry( uint32_t _column, uint32_t _row, const JewelryPtr & _jewelry );
        const JewelryPtr & getJewelry( uint32_t _column, uint32_t _row ) const;
        bool existJewelry( uint32_t _column, uint32_t _row ) const;

    protected:
        uint32_t getIndex_( uint32_t _column, uint32_t _row ) const;

    protected:
        uint32_t m_columnCount;
        uint32_t m_rowCount;

        struct Slot
        {
            JewelryPtr jewelry;
        };

        typedef Vector<Slot> VectorSlots;
        VectorSlots m_slots;

        GOAP::EventPtr m_eventSlot;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<JewelryMatrix> JewelryMatrixPtr;
    //////////////////////////////////////////////////////////////////////////
}