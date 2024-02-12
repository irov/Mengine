package org.Mengine.Plugin.GeneralDataProtectionRegulation;

import android.content.Intent;
import android.content.IntentSender;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

public class MengineGeneralDataProtectionRegulationPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "MengineGDPR";
    public static final boolean PLUGIN_EMBEDDING = true;

    private boolean m_passGDPR = false;

    @Override
    public void onAppPrepare(MengineApplication application) throws MenginePluginInvalidInitializeException {
        SharedPreferences settings = this.getPrivateSharedPreferences();

        boolean GDPRPass = settings.getBoolean("gdpr_pass", false);

        m_passGDPR = GDPRPass;

        this.sendEvent(MengineEvent.EVENT_GDPR_PASS, m_passGDPR);
    }

    public void setGDPRPass(boolean passGDPR) {
        SharedPreferences settings = this.getPrivateSharedPreferences();

        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean("gdpr_pass", passGDPR);
        editor.commit();

        m_passGDPR = passGDPR;

        this.sendEvent(MengineEvent.EVENT_GDPR_PASS, true);
    }

    public boolean isGDPRPass() {
        return m_passGDPR;
    }
}
