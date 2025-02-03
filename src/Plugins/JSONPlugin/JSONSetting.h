#pragma once

#include "Interface/SettingInterface.h"
#include "Interface/ContentInterface.h"

#include "JSONInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class JSONSetting
        : public SettingInterface
    {
        DECLARE_FACTORABLE( JSONSetting );

    public:
        JSONSetting();
        ~JSONSetting() override;

    public:
        bool initialize( const ConstString & _name, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) override;
        void finalize() override;

    public:
        bool save( const DocumentInterfacePtr & _doc ) const override;

    public:
        const ConstString & getName() const override;
        const ContentInterfacePtr & getContent() const override;

    public:
        void foreachKeys( const LambdaKeys & _lambda ) const override;

    public:
        ESettingType getValueType( const Char * _key ) const override;

    public:
        bool getValueBoolean( const Char * _key, bool _default ) const override;
        int64_t getValueInteger( const Char * _key, int64_t _default ) const override;
        double getValueFloat( const Char * _key, double _default ) const override;
        const Char * getValueString( const Char * _key, const Char * _default ) const override;
        mt::vec2f getValueVec2f( const Char * _key, const mt::vec2f & _default ) const override;
        mt::vec3f getValueVec3f( const Char * _key, const mt::vec3f & _default ) const override;
        Color getValueColor( const Char * _key, const Color & _default ) const override;

    public:
        bool setValueBoolean( const Char * _key, bool _value ) override;
        bool setValueInteger( const Char * _key, int64_t _value ) override;
        bool setValueFloat( const Char * _key, double _value ) override;
        bool setValueString( const Char * _key, const Char * _value ) override;
        bool setValueVec2f( const Char * _key, const mt::vec2f & _value ) override;
        bool setValueVec3f( const Char * _key, const mt::vec3f & _value ) override;
        bool setValueColor( const Char * _key, const Color & _value ) override;

    protected:
        ConstString m_name;
        ContentInterfacePtr m_content;

        jpp::object m_json;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<JSONSetting, SettingInterface> JSONSettingPtr;
    //////////////////////////////////////////////////////////////////////////
}