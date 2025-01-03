package org.Mengine.Plugin.GeneralDataProtectionRegulation;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

import java.util.Map;

public class MengineGeneralDataProtectionRegulationPlugin extends MengineService implements MengineListenerApplication {
    public static final String SERVICE_NAME = "GDPR";
    public static final boolean SERVICE_EMBEDDING = true;

    private boolean m_passGDPR;

    @Override
    public void onAppPrepare(@NonNull MengineApplication application, @NonNull Map<String, String> pluginVersions) throws MengineServiceInvalidInitializeException {
        boolean GDPRPass = application.getPreferenceBoolean("gdpr_pass", false);

        m_passGDPR = GDPRPass;

        this.sendEvent(MengineEvent.EVENT_GDPR_PASS, m_passGDPR);
    }

    public void setGDPRPass(boolean passGDPR) {
        if (m_passGDPR == passGDPR) {
            return;
        }

        MengineApplication application = this.getMengineApplication();

        application.setPreferenceBoolean("gdpr_pass", passGDPR);

        m_passGDPR = passGDPR;

        this.sendEvent(MengineEvent.EVENT_GDPR_PASS, m_passGDPR);
    }

    public boolean isGDPRPass() {
        return m_passGDPR;
    }
}
