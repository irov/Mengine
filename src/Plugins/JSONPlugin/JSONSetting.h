#pragma once

#include "Interface/SettingInterface.h"

#include "JSONInterface.h"

namespace Mengine
{
    class JSONSetting
        : public SettingInterface
    {
    public:
        JSONSetting();
        ~JSONSetting() override;

    public:
        bool loadSetting( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Char * _doc ) override;

    public:
        bool getValue( const Char * _key, bool _default ) const override;
        int32_t getValue( const Char * _key, int32_t _default ) const override;
        uint32_t getValue( const Char * _key, uint32_t _default ) const override;
        float getValue( const Char * _key, float _default ) const override;
        const Char * getValue( const Char * _key, const Char * _default ) const override;
        mt::vec2f getValue( const Char * _key, const mt::vec2f & _default ) const override;
        mt::vec3f getValue( const Char * _key, const mt::vec3f & _default ) const override;
        Color getValue( const Char * _key, const Color & _default ) const override;

    protected:
        JSONStorageInterfacePtr m_storage;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<JSONSetting> JSONSettingPtr;
    //////////////////////////////////////////////////////////////////////////
}