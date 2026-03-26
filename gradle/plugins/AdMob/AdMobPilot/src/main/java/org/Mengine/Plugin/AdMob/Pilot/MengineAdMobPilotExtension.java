package org.Mengine.Plugin.AdMob.Pilot;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePluginExtensionInterface;
import org.Mengine.Base.MenginePluginExtensionManager;
import org.Mengine.Base.MengineUtils;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobBannerAdInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobInterstitialAdInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobPluginInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobRewardedAdInterface;

import org.pilot.sdk.Pilot;
import org.pilot.sdk.PilotLayout;
import org.pilot.sdk.PilotTab;
import org.pilot.sdk.PilotUI;

public class MengineAdMobPilotExtension implements MenginePluginExtensionInterface {
    private MengineAdMobPluginInterface m_plugin;

    static {
        MenginePluginExtensionManager.registerExtension("Pilot", MengineAdMobPilotExtension.class);
    }

    @Override
    public void onExtensionInitialize(@NonNull MengineApplication application) {
        m_plugin = (MengineAdMobPluginInterface) application.findService("AdMob");

        if (m_plugin == null) {
            return;
        }

        this.setupUI();
    }

    @Override
    public void onExtensionFinalize(@NonNull MengineApplication application) {
    }

    private void setupUI() {
        if (Pilot.isInitialized() == false) {
            return;
        }

        PilotUI ui = Pilot.getUI();
        PilotTab tab = ui.addTab("admob", "AdMob");
        PilotLayout root = tab.vertical();

        MengineAdMobBannerAdInterface bannerAd = m_plugin.getBannerAd();
        MengineAdMobInterstitialAdInterface interstitialAd = m_plugin.getInterstitialAd();
        MengineAdMobRewardedAdInterface rewardedAd = m_plugin.getRewardedAd();

        if (bannerAd != null) {
            this.setupBanner(root, bannerAd);
        }

        if (interstitialAd != null) {
            this.setupInterstitial(root, interstitialAd);
        }

        if (rewardedAd != null) {
            this.setupRewarded(root, rewardedAd);
        }
    }

    private void setupBanner(@NonNull PilotLayout root, @NonNull MengineAdMobBannerAdInterface bannerAd) {
        root.addLabel("admob-banner-header", "Banner").color("primary");

        PilotLayout statsRow = root.addHorizontal();

        statsRow.addStat("admob-banner-canshow", "Can Show")
            .valueProvider(() -> bannerAd.canYouShow() ? "Yes" : "No");

        statsRow.addStat("admob-banner-size", "Size")
            .unit("px")
            .valueProvider(() -> bannerAd.getWidthPx() + "x" + bannerAd.getHeightPx());

        PilotLayout btnsRow = root.addHorizontal();

        btnsRow.addButton("admob-banner-show", "Show Banner")
            .variant("contained").color("success")
            .onClick(action -> {
                MengineUtils.performOnMainThread(() -> {
                    bannerAd.show();
                });
            });

        btnsRow.addButton("admob-banner-hide", "Hide Banner")
            .variant("outlined").color("warning")
            .onClick(action -> {
                MengineUtils.performOnMainThread(() -> {
                    bannerAd.hide();
                });
            });
    }

    private void setupInterstitial(@NonNull PilotLayout root, @NonNull MengineAdMobInterstitialAdInterface interstitialAd) {
        root.addLabel("admob-inter-header", "Interstitial").color("primary");

        PilotLayout statsRow = root.addHorizontal();

        statsRow.addStat("admob-inter-canshow", "Can Show")
            .valueProvider(() -> interstitialAd.canYouShowInterstitial("pilot") ? "Yes" : "No");

        statsRow.addStat("admob-inter-showing", "Showing")
            .valueProvider(() -> interstitialAd.isShowingInterstitial() ? "Yes" : "No");

        root.addButton("admob-inter-show", "Show Interstitial")
            .variant("contained").color("primary")
            .onClick(action -> {
                MengineUtils.performOnMainThread(() -> {
                    m_plugin.showInterstitial("pilot");
                });
            });
    }

    private void setupRewarded(@NonNull PilotLayout root, @NonNull MengineAdMobRewardedAdInterface rewardedAd) {
        root.addLabel("admob-reward-header", "Rewarded").color("primary");

        PilotLayout statsRow = root.addHorizontal();

        statsRow.addStat("admob-reward-canshow", "Can Show")
            .valueProvider(() -> rewardedAd.canYouShowRewarded("pilot") ? "Yes" : "No");

        statsRow.addStat("admob-reward-showing", "Showing")
            .valueProvider(() -> rewardedAd.isShowingRewarded() ? "Yes" : "No");

        root.addButton("admob-reward-show", "Show Rewarded")
            .variant("contained").color("secondary")
            .onClick(action -> {
                MengineUtils.performOnMainThread(() -> {
                    m_plugin.showRewarded("pilot");
                });
            });
    }
}
