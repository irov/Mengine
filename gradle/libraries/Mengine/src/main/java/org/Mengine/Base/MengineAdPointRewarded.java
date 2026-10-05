package org.Mengine.Base;

import androidx.annotation.NonNull;

import org.json.JSONObject;

public class MengineAdPointRewarded extends MengineAdPointBase {
    public static final MengineTag TAG = MengineTag.of("MNGAdPointRewarded");

    private static final int DEFAULT_SHOW_LIMIT = -1;
    private static final long DEFAULT_SHOW_TIMEOUT = 86400;

    protected final int m_showLimit;
    protected final long m_showTimeout;

    protected MengineAdFrequency m_frequency;

    MengineAdPointRewarded(@NonNull String name, @NonNull JSONObject values) {
        super(name, values);

        m_showLimit = this.parseAdPointInteger(values, "trigger_show_limit", false, DEFAULT_SHOW_LIMIT, -1, null);
        m_showTimeout = this.parseAdPointTime(values, "trigger_show_timeout", false, DEFAULT_SHOW_TIMEOUT, 1L, null);
    }

    public void setFrequency(@NonNull MengineAdFrequency frequency) {
        m_frequency = frequency;
    }

    public MengineAdFrequency getFrequency() {
        return m_frequency;
    }

    public boolean canOfferAd(@NonNull MengineApplication application) {
        if (BuildConfig.DEBUG == true) {
            if (MengineUtils.hasOption(application, "adservice.always_rewarded_point") == true) {
                return true;
            }

            if (MengineUtils.hasOption(application, "adservice.always_rewarded_point." + m_name) == true) {
                return true;
            }
        }

        if (m_enabled == false) {
            return false;
        }

        boolean hasQuota = this.hasShowQuota();

        return hasQuota;
    }

    public boolean canYouShowAd(@NonNull MengineApplication application) {
        if (BuildConfig.DEBUG == true) {
            if (MengineUtils.hasOption(application, "adservice.always_rewarded_point") == true) {
                return true;
            }

            if (MengineUtils.hasOption(application, "adservice.always_rewarded_point." + m_name) == true) {
                return true;
            }
        }

        if (m_enabled == false) {
            return false;
        }

        if (this.hasShowQuota() == false) {
            return false;
        }

        m_attempts.incrementAttempts();

        return true;
    }

    public int getShowLimit() {
        return m_showLimit;
    }

    public long getShowTimeout() {
        return m_showTimeout;
    }

    public boolean hasShowQuota() {
        boolean hasQuota = m_frequency.hasShowQuota(m_showLimit, m_showTimeout);

        return hasQuota;
    }

    public void recordShown() {
        m_frequency.recordShown(m_showTimeout);
    }
}
