package org.Mengine.Base;

import androidx.annotation.NonNull;

public class MengineProcedureOpenUrl implements MengineProcedureInterface {
    private static final MengineTag TAG = MengineTag.of("MPOpenUrl");

    private final String m_url;

    public MengineProcedureOpenUrl(@NonNull String url) {
        m_url = url;
    }

    @Override
    public boolean execute(@NonNull MengineActivity activity) {
        MengineLog.logInfo(TAG, "linkingOpenURL url: %s"
            , m_url
        );

        MengineApplication.INSTANCE.setState("open.url", m_url);

        MengineAnalytics.buildEvent("mng_open_url")
            .addParameterString("url", m_url)
            .log();

        if (MengineUtils.openUrl(activity, m_url) == false) {
            MengineAnalytics.buildEvent("mng_open_url_failed")
                .addParameterString("url", m_url)
                .log();

            return false;
        }

        return true;
    }

    static {
        MengineFactoryManager.registerClazz("openUrl", MengineProcedureOpenUrl.class, String.class);
    }
}