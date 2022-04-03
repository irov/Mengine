#pragma once

#include "Interface/SettingInterface.h"

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
        bool initialize( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc ) override;
        void finalize() override;

    public:
        const FileGroupInterfacePtr & getFileGroup() const override;
        const FilePath & getFilePath() const override;

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
        FileGroupInterfacePtr m_fileGroup;
        FilePath m_filePath;

        jpp::object m_json;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<JSONSetting, SettingInterface> JSONSettingPtr;
    //////////////////////////////////////////////////////////////////////////
}