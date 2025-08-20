package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

public class MengineAdPointRewarded extends MengineAdPointBase {
    public static final MengineTag TAG = MengineTag.of("MNGAdPointRewarded");

    MengineAdPointRewarded(@NonNull String name, @NonNull JSONObject values) {
        super(name, values);

    }

    public boolean canOfferAd(@NonNull MengineApplication application) {
        if (BuildConfig.DEBUG == true) {
            if (MengineUtils.hasOption(application, "adservice.always_rewarded_point." + m_name) == true) {
                return true;
            }
        }

        if (m_enabled == false) {
            return false;
        }

        return true;
    }

    public boolean canYouShowAd(@NonNull MengineApplication application) {
        if (BuildConfig.DEBUG == true) {
            if (MengineUtils.hasOption(application, "adservice.always_rewarded_point." + m_name) == true) {
                return true;
            }
        }

        if (m_enabled == false) {
            return false;
        }

        m_attempts.incrementAttempts();

        return true;
    }
}