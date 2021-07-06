#pragma once

#include "Kernel/Factorable.h"

#include "Kernel/Node.h"
#include "Kernel/Pickerable.h"

#include "GOAP/SourceInterface.h"

#include "JewelryMatrix.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EJewelryState
    {
        EJS_NONE = 0,
        EJS_BLOCK = 1 << 0,
        EJS_PICK = 1 << 1,
        EJS_DEAD = 1 << 2,
        EJS_MOVE = 1 << 3,
    };
    //////////////////////////////////////////////////////////////////////////
    enum EJewelrySuper
    {
        EJSUPER_NORMAL = 0,
        EJSUPER_BOMB = 1 << 0,
    };
    //////////////////////////////////////////////////////////////////////////
    class Jewelry
        : public Factorable
    { 
        DECLARE_FACTORABLE( Jewelry );

    public:
        Jewelry();
        ~Jewelry() override;

    public:
        void setColumn( uint32_t _column );
        uint32_t getColumn() const;

        void setRow( uint32_t _row );
        uint32_t getRow() const;

    public:
        bool isBomb() const;

    public:
        bool initialize( EJewelrySuper _super, uint32_t _type, const JewelryMatrixPtr & _matrix, uint32_t _column, uint32_t _row, float _size, float _stride );
        void finalize();

    public:
        void makeNodeActive_();
        void makeNodeBlock_();
        void makeNodeBomb_();
        void makePickerable_();

    public:
        void move( const GOAP::SourceInterfacePtr & _source, uint32_t _row, float _time );
        void stop();
        void pickHand( const GOAP::SourceInterfacePtr & _source );
        void unpickHand( const GOAP::SourceInterfacePtr & _source );
        void block( const GOAP::SourceInterfacePtr & _source );
        void dead( const GOAP::SourceInterfacePtr & _source );
        void bomb( const GOAP::SourceInterfacePtr & _source );
        void explosive( const GOAP::SourceInterfacePtr & _source );

    public:
        bool isDead() const;
        bool isBlock() const;
        bool isMove() const;

    public:        
        uint32_t getType() const;
        EJewelrySuper getSuper() const;

    public:
        const NodePtr & getNode() const;
        const NodePtr & getNodeActive() const;
        const NodePtr & getNodeBomb() const;
        const PickerablePtr & getPickerable() const;

    public:
        JewelryMatrixPtr m_matrix;

        float m_size;
        float m_stride;

        uint32_t m_type;
        EJewelrySuper m_super;

        uint32_t m_column;
        uint32_t m_row;

        NodePtr m_node;
        NodePtr m_nodeActive;
        NodePtr m_nodeBlock;
        NodePtr m_nodeBomb;

        PickerablePtr m_pickerable;

        uint32_t m_state;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Jewelry> JewelryPtr;
    //////////////////////////////////////////////////////////////////////////
}