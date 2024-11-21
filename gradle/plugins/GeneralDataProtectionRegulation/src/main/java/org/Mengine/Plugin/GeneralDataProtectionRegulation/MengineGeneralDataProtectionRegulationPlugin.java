package org.Mengine.Plugin.GeneralDataProtectionRegulation;

import android.content.SharedPreferences;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

public class MengineGeneralDataProtectionRegulationPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "MengineGDPR";
    public static final boolean PLUGIN_EMBEDDING = true;

    private boolean m_passGDPR;

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
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
