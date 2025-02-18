package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

public class MengineAdRewardedPoint extends MengineAdBasePoint {
    public static final String TAG = "MengineAdRewardedPoint";

    protected int m_id;
    protected boolean m_enabled;

    MengineAdRewardedPoint(@NonNull String name, @NonNull JSONObject values) {
        super(name);

        m_id = this.parseAdPointInteger(values, "id", false, 1);
        m_enabled = this.parseAdPointBoolean(values, "enable", true, false);
    }

    public boolean canOfferAd(@NonNull MengineApplication application) {
        if (m_enabled == false) {
            return false;
        }

        return true;
    }

    public boolean canYouShowAd(@NonNull MengineApplication application) {
        if (m_enabled == false) {
            return false;
        }

        m_attempts.attempt();

        return true;
    }

    public void showAd() {
        //ToDo: Implement this method
    }
}