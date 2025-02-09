package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

public class MengineAdRewardedPoint extends MengineAdBasePoint {
    public static final String TAG = "MengineAdRewardedPoint";

    public int m_id;
    public boolean m_enabled;

    public int m_actionAttempts = 0;

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

        m_actionAttempts++;

        return true;
    }

    public void showAd() {
        //ToDo: Implement this method
    }

    public void completeAd() {
        //ToDo: Implement this method
    }
}