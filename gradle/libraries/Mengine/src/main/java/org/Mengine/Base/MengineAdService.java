package org.Mengine.Base;

import android.os.Bundle;

import androidx.annotation.NonNull;

import org.json.JSONObject;

import java.util.HashMap;
import java.util.Map;

public class MengineAdService extends MengineService implements MengineAdProviderInterface, MengineListenerRemoteConfig {
    public static final String SERVICE_NAME = "AdService";
    public static final boolean SERVICE_EMBEDDING = true;
    public static final int SAVE_VERSION = 1;

    private final Map<String, MengineAdInterstitialPoint> m_adInterstitialPoints = new HashMap<>();
    private final Map<String, MengineAdRewardedPoint> m_adRewardedPoints = new HashMap<>();
    private final Map<String, MengineAdCooldown> m_adCooldowns = new HashMap<>();
    private final Map<String, MengineAdAttempts> m_adAttempts = new HashMap<>();

    private MengineAdProviderInterface m_adProvider;

    public void setAdProvider(MengineAdProviderInterface adProvider) {
        m_adProvider = adProvider;
    }

    public MengineAdProviderInterface getAdProvider() {
        return m_adProvider;
    }

    public void readyAdProvider() {
        this.activateSemaphore("AdServiceReady");
    }

    private void parseInterstitialPoint(String adPointName, JSONObject adPointConfig) {
        if (m_adInterstitialPoints.containsKey(adPointName) == true) {
            this.logError("ad interstitial point '%s' already exists", adPointName);

            return;
        }

        MengineAdInterstitialPoint adPoint = new MengineAdInterstitialPoint(adPointName, adPointConfig);

        String cooldownGroupName = adPoint.getCooldownGroupName();

        if (cooldownGroupName == null) {
            MengineAdCooldown newCooldown = new MengineAdCooldown();

            adPoint.setCooldown(newCooldown);
        } else {
            if (m_adCooldowns.containsKey(cooldownGroupName) == false) {
                MengineAdCooldown newCooldown = new MengineAdCooldown();

                m_adCooldowns.put(cooldownGroupName, newCooldown);
            }

            MengineAdCooldown cooldown = m_adCooldowns.get(cooldownGroupName);

            adPoint.setCooldown(cooldown);
        }

        if (m_adAttempts.containsKey(adPointName) == false) {
            MengineAdAttempts newAttempts = new MengineAdAttempts();

            m_adAttempts.put(adPointName, newAttempts);
        }

        MengineAdAttempts attempts = m_adAttempts.get(adPointName);

        adPoint.setAttempts(attempts);

        m_adInterstitialPoints.put(adPointName, adPoint);
    }

    private void parseRewardedPoint(String adPointName, JSONObject adPointConfig) {
        if (m_adRewardedPoints.containsKey(adPointName) == true) {
            this.logError("ad rewarded point '%s' already exists", adPointName);

            return;
        }

        MengineAdRewardedPoint adPoint = new MengineAdRewardedPoint(adPointName, adPointConfig);

        m_adRewardedPoints.put(adPointName, adPoint);
    }

    @Override
    public void onMengineRemoteConfigFetch(@NonNull MengineApplication application, @NonNull Map<String, JSONObject> remoteConfig) {
        synchronized (this) {
            m_adInterstitialPoints.clear();
            m_adRewardedPoints.clear();
            m_adCooldowns.clear();

            String ad_interstitial_prefix = "ad_interstitial_";
            String ad_rewarded_prefix = "ad_rewarded_";

            for (Map.Entry<String, JSONObject> entry : remoteConfig.entrySet()) {
                String key = entry.getKey();
                JSONObject adPointConfig = entry.getValue();

                if (key.startsWith(ad_interstitial_prefix) == true) {
                    String adPointName = key.substring(ad_interstitial_prefix.length());

                    this.parseInterstitialPoint(adPointName, adPointConfig);
                } else if (key.startsWith(ad_rewarded_prefix) == true) {
                    String adPointName = key.substring(ad_rewarded_prefix.length());

                    this.parseRewardedPoint(adPointName, adPointConfig);
                }
            }
        }
    }

    @Override
    public void onLoad(@NonNull MengineApplication application, @NonNull Bundle bundle) {
        int version = bundle.getInt("version", 0);

        Bundle attemptsBundle = bundle.getBundle("attempts");

        if (attemptsBundle != null) {
            for (String key : attemptsBundle.keySet()) {
                Bundle attemptsPointBundle = attemptsBundle.getBundle(key);

                MengineAdAttempts attempts = new MengineAdAttempts();

                attempts.onLoad(application, attemptsPointBundle);

                m_adAttempts.put(key, attempts);
            }
        }

        Bundle cooldownsBundle = bundle.getBundle("cooldowns");

        if (cooldownsBundle != null) {
            for (String key : cooldownsBundle.keySet()) {
                Bundle cooldownBundle = cooldownsBundle.getBundle(key);

                MengineAdCooldown cooldown = new MengineAdCooldown();

                cooldown.onLoad(application, cooldownBundle);

                m_adCooldowns.put(key, cooldown);
            }
        }
    }

    @Override
    public Bundle onSave(@NonNull MengineApplication application) {
        Bundle bundle = new Bundle();

        bundle.putInt("version", SAVE_VERSION);

        Bundle attemptsBundle = new Bundle();

        for (Map.Entry<String, MengineAdAttempts> entry : m_adAttempts.entrySet()) {
            String attemptsPointName = entry.getKey();
            MengineAdAttempts attempts = entry.getValue();

            Bundle attemptsPointBundle = attempts.onSave(application);

            attemptsBundle.putBundle(attemptsPointName, attemptsPointBundle);
        }

        bundle.putBundle("attempts", attemptsBundle);

        Bundle cooldownsBundle = new Bundle();

        for (Map.Entry<String, MengineAdCooldown> entry : m_adCooldowns.entrySet()) {
            String cooldownGroupName = entry.getKey();
            MengineAdCooldown cooldown = entry.getValue();

            Bundle cooldownBundle = cooldown.onSave(application);

            cooldownsBundle.putBundle(cooldownGroupName, cooldownBundle);
        }

        bundle.putBundle("cooldowns", cooldownsBundle);

        return bundle;
    }

    @Override
    public boolean hasBanner() {
        return m_adProvider.hasBanner();
    }

    @Override
    public void showBanner() {
        m_adProvider.showBanner();
    }

    @Override
    public void hideBanner() {
        m_adProvider.hideBanner();
    }

    @Override
    public int getBannerHeight() {
        int height = m_adProvider.getBannerHeight();

        return height;
    }

    @Override
    public boolean hasInterstitial() {
        if (m_adProvider.hasInterstitial() == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canYouShowInterstitial(String placement) {
        if (m_adInterstitialPoints.containsKey(placement) == false) {
            this.logError("ad interstitial point '%s' not found", placement);

            return false;
        }

        if (m_adProvider.canYouShowInterstitial(placement) == false) {
            return false;
        }

        MengineApplication application = this.getMengineApplication();

        MengineAdInterstitialPoint adPoint = m_adInterstitialPoints.get(placement);

        if (adPoint.canYouShowAd(application) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean showInterstitial(String placement) {
        if (m_adInterstitialPoints.containsKey(placement) == false) {
            this.logError("ad interstitial point '%s' not found", placement);

            return false;
        }

        MengineAdInterstitialPoint adPoint = m_adInterstitialPoints.get(placement);

        if (m_adProvider.showInterstitial(placement) == false) {
            return false;
        }

        adPoint.showAd();

        return true;
    }

    @Override
    public boolean hasRewarded() {
        if (m_adProvider.hasRewarded() == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canOfferRewarded(String placement) {
        if (m_adRewardedPoints.containsKey(placement) == false) {
            this.logError("ad rewarded point '%s' not found", placement);

            return false;
        }

        if (m_adProvider.canOfferRewarded(placement) == false) {
            return false;
        }

        MengineApplication application = this.getMengineApplication();

        if (BuildConfig.DEBUG == true) {
            if (application.hasOption("adservice.always_rewarded_point." + placement) == true) {
                return true;
            }
        }

        MengineAdRewardedPoint adPoint = m_adRewardedPoints.get(placement);

        if (adPoint.canOfferAd(application) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean canYouShowRewarded(String placement) {
        if (m_adRewardedPoints.containsKey(placement) == false) {
            this.logError("ad rewarded point '%s' not found", placement);

            return false;
        }

        if (m_adProvider.canYouShowRewarded(placement) == false) {
            return false;
        }

        MengineApplication application = this.getMengineApplication();

        if (BuildConfig.DEBUG == true) {
            if (application.hasOption("adservice.always_rewarded_point." + placement) == true) {
                return true;
            }
        }

        MengineAdRewardedPoint adPoint = m_adRewardedPoints.get(placement);

        if (adPoint.canYouShowAd(application) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean showRewarded(String placement) {
        if (m_adRewardedPoints.containsKey(placement) == false) {
            this.logError("ad rewarded point '%s' not found", placement);

            return false;
        }

        MengineAdRewardedPoint adPoint = m_adRewardedPoints.get(placement);

        if (m_adProvider.showRewarded(placement) == false) {
            return false;
        }

        adPoint.showAd();

        return true;
    }
}