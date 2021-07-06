#pragma once

#include "GOAP/EventInterface.h"

#include "Kernel/Vector.h"
#include "Kernel/Factorable.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Jewelry> JewelryPtr;
    typedef Vector<JewelryPtr> VectorJewelries;
    //////////////////////////////////////////////////////////////////////////
    class JewelryMatrix
        : public Factorable
    {
    public:
        JewelryMatrix();
        ~JewelryMatrix() override;

    public:
        bool initialize( const GOAP::EventInterfacePtr & _event, uint32_t _columnCount, uint32_t _rowCount );
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

    public:
        bool isFall( const JewelryPtr & _jewelry ) const;

        typedef Lambda<bool( const JewelryPtr & )> LambdaJewelry;
        void getNeighbours( uint32_t _column, uint32_t _row, uint32_t _type, VectorJewelries & _jewelries, const LambdaJewelry & _lambda ) const;

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

        GOAP::EventInterfacePtr m_eventSlot;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<JewelryMatrix> JewelryMatrixPtr;
    //////////////////////////////////////////////////////////////////////////
}