package org.Mengine.Plugin.AdMob.Pilot;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineAdCooldown;
import org.Mengine.Base.MengineAdPointAppOpen;
import org.Mengine.Base.MengineAdPointInterstitial;
import org.Mengine.Base.MengineAdPointRewarded;
import org.Mengine.Base.MengineAdService;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePluginExtensionInterface;
import org.Mengine.Base.MenginePluginExtensionManager;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobBannerAdInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobInterstitialAdInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobPluginInterface;
import org.Mengine.Plugin.AdMob.Core.MengineAdMobRewardedAdInterface;

import org.pilot.sdk.Pilot;
import org.pilot.sdk.PilotLayout;
import org.pilot.sdk.PilotTab;
import org.pilot.sdk.PilotUI;

import java.util.ArrayList;
import java.util.List;

public class MengineAdMobPilotExtension implements MenginePluginExtensionInterface {
    private MengineAdMobPluginInterface m_plugin;
    private MengineAdService m_adService;

    static {
        MenginePluginExtensionManager.registerExtension("Pilot", MengineAdMobPilotExtension.class);
    }

    @Override
    public void onExtensionInitialize(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        m_plugin = (MengineAdMobPluginInterface) application.findService("AdMob");

        if (m_plugin == null) {
            throw new MengineServiceInvalidInitializeException("AdMob service not found");
        }

        m_adService = application.getService(MengineAdService.class);

        if (m_adService == null) {
            throw new MengineServiceInvalidInitializeException("MengineAdService not found");
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
        PilotTab tab = ui.addTab("AdMob");
        PilotLayout root = tab.vertical();

        MengineAdMobBannerAdInterface bannerAd = m_plugin.getBannerAd();
        MengineAdMobInterstitialAdInterface interstitialAd = m_plugin.getInterstitialAd();
        MengineAdMobRewardedAdInterface rewardedAd = m_plugin.getRewardedAd();

        if (bannerAd != null) {
            this.setupBanner(root, bannerAd);
        }

        this.setupAdPoints(root, interstitialAd, rewardedAd);
    }

    private void setupBanner(@NonNull PilotLayout root, @NonNull MengineAdMobBannerAdInterface bannerAd) {
        root.addLabel("Banner").color("primary");

        PilotLayout statsRow = root.addHorizontal();

        statsRow.addStat("Can Show")
            .valueProvider(() -> bannerAd.canYouShow() ? "Yes" : "No");

        statsRow.addStat("Size")
            .unit("px")
            .valueProvider(() -> bannerAd.getWidthPx() + "x" + bannerAd.getHeightPx());

        PilotLayout btnsRow = root.addHorizontal();

        btnsRow.addButton("Show Banner")
            .variant("contained").color("success")
            .onClick(action -> {
                bannerAd.show();
            });

        btnsRow.addButton("Hide Banner")
            .variant("outlined").color("warning")
            .onClick(action -> {
                bannerAd.hide();
            });
    }

    private void setupAdPoints(@NonNull PilotLayout root, MengineAdMobInterstitialAdInterface interstitialAd, MengineAdMobRewardedAdInterface rewardedAd) {
        List<MengineAdPointInterstitial> interstitialPoints = new ArrayList<>();
        List<MengineAdPointRewarded> rewardedPoints = new ArrayList<>();
        List<MengineAdPointAppOpen> appOpenPoints = new ArrayList<>();

        m_adService.forEachAdInterstitialPoint(interstitialPoints::add);
        m_adService.forEachAdRewardedPoint(rewardedPoints::add);
        m_adService.forEachAdAppOpenPoint(appOpenPoints::add);

        if (interstitialPoints.isEmpty() == false) {
            root.addLabel("Interstitial Points").color("primary");

            for (MengineAdPointInterstitial adPoint : interstitialPoints) {
                PilotLayout content = root.addCollapsible(adPoint.getName());
                this.setupInterstitialPoint(content, adPoint, interstitialAd);
            }
        }

        if (rewardedPoints.isEmpty() == false) {
            root.addLabel("Rewarded Points").color("primary");

            for (MengineAdPointRewarded adPoint : rewardedPoints) {
                PilotLayout content = root.addCollapsible(adPoint.getName());
                this.setupRewardedPoint(content, adPoint, rewardedAd);
            }
        }

        if (appOpenPoints.isEmpty() == false) {
            root.addLabel("AppOpen Points").color("primary");

            for (MengineAdPointAppOpen adPoint : appOpenPoints) {
                PilotLayout content = root.addCollapsible(adPoint.getName());
                this.setupAppOpenPoint(content, adPoint);
            }
        }
    }

    private String formatTimeMs(long ms) {
        if (ms < 0) {
            return "off";
        }

        long seconds = ms / 1000;

        if (seconds < 60) {
            return seconds + "s";
        }

        return (seconds / 60) + "m " + (seconds % 60) + "s";
    }

    private String formatCooldownRemaining(MengineAdCooldown cooldown, long timeCooldownMs) {
        if (cooldown == null || timeCooldownMs <= 0) {
            return "off";
        }

        long lastShown = cooldown.getLastShownTimestamp();

        if (lastShown == 0) {
            return "ready";
        }

        long now = MengineUtils.getTimestamp();
        long elapsed = now - lastShown;
        long remaining = timeCooldownMs - elapsed;

        if (remaining <= 0) {
            return "ready";
        }

        return formatTimeMs(remaining);
    }

    private void setupInterstitialPoint(@NonNull PilotLayout root, @NonNull MengineAdPointInterstitial adPoint, MengineAdMobInterstitialAdInterface interstitialAd) {
        String pointName = adPoint.getName();

        PilotLayout row1 = root.addHorizontal();

        row1.addStat("Enabled")
            .valueProvider(() -> adPoint.isEnabled() ? "Yes" : "No");

        row1.addStat("Attempts")
            .valueProvider(() -> {
                if (adPoint.getAttempts() == null) {
                    return 0;
                }

                return adPoint.getAttempts().getAttempts();
            });

        row1.addStat("Cooldown")
            .valueProvider(() -> formatCooldownRemaining(adPoint.getCooldown(), adPoint.getTimeCooldown()));

        PilotLayout row2 = root.addHorizontal();

        row2.addStat("Act Offset")
            .valueProvider(() -> adPoint.getActionOffset());

        row2.addStat("Act Cooldown")
            .valueProvider(() -> adPoint.getActionCooldown());

        row2.addStat("Session Offset")
            .valueProvider(() -> adPoint.getSessionOffset());

        PilotLayout row3 = root.addHorizontal();

        row3.addStat("Time Offset")
            .valueProvider(() -> formatTimeMs(adPoint.getTimeOffset()));

        row3.addStat("Time Cooldown")
            .valueProvider(() -> formatTimeMs(adPoint.getTimeCooldown()));

        row3.addStat("Install Offset")
            .valueProvider(() -> formatTimeMs(adPoint.getInstallTimeOffset()));

        if (interstitialAd != null) {
            root.addButton("Show [" + pointName + "]")
                .variant("contained").color("primary")
                .onClick(action -> {
                    m_plugin.showInterstitial(pointName);
                });
        }
    }

    private void setupRewardedPoint(@NonNull PilotLayout root, @NonNull MengineAdPointRewarded adPoint, MengineAdMobRewardedAdInterface rewardedAd) {
        String pointName = adPoint.getName();

        PilotLayout row = root.addHorizontal();

        row.addStat("Enabled")
            .valueProvider(() -> adPoint.isEnabled() ? "Yes" : "No");

        row.addStat("Attempts")
            .valueProvider(() -> {
                if (adPoint.getAttempts() == null) {
                    return 0;
                }

                return adPoint.getAttempts().getAttempts();
            });

        if (rewardedAd != null) {
            root.addButton("Show [" + pointName + "]")
                .variant("contained").color("secondary")
                .onClick(action -> {
                    m_plugin.showRewarded(pointName);
                });
        }
    }

    private void setupAppOpenPoint(@NonNull PilotLayout root, @NonNull MengineAdPointAppOpen adPoint) {
        String pointName = adPoint.getName();

        PilotLayout row1 = root.addHorizontal();

        row1.addStat("Enabled")
            .valueProvider(() -> adPoint.isEnabled() ? "Yes" : "No");

        row1.addStat("Attempts")
            .valueProvider(() -> {
                if (adPoint.getAttempts() == null) {
                    return 0;
                }

                return adPoint.getAttempts().getAttempts();
            });

        row1.addStat("Cooldown")
            .valueProvider(() -> formatCooldownRemaining(adPoint.getCooldown(), adPoint.getTimeCooldown()));

        PilotLayout row2 = root.addHorizontal();

        row2.addStat("Act Offset")
            .valueProvider(() -> adPoint.getActionOffset());

        row2.addStat("Act Cooldown")
            .valueProvider(() -> adPoint.getActionCooldown());

        row2.addStat("Time Sleep")
            .valueProvider(() -> formatTimeMs(adPoint.getTimeSleep()));

        PilotLayout row3 = root.addHorizontal();

        row3.addStat("Time Offset")
            .valueProvider(() -> formatTimeMs(adPoint.getTimeOffset()));

        row3.addStat("Time Cooldown")
            .valueProvider(() -> formatTimeMs(adPoint.getTimeCooldown()));

        row3.addStat("Install Offset")
            .valueProvider(() -> formatTimeMs(adPoint.getInstallTimeOffset()));
    }
}
