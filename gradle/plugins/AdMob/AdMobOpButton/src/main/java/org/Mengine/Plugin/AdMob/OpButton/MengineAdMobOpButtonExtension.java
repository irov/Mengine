package org.Mengine.Plugin.AdMob.OpButton;

import androidx.annotation.NonNull;

import org.Mengine.Base.MengineAdAttempts;
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

import org.opbutton.sdk.OpButton;
import org.opbutton.sdk.OpButtonLayout;
import org.opbutton.sdk.OpButtonTab;
import org.opbutton.sdk.OpButtonUI;

import java.util.ArrayList;
import java.util.List;

public class MengineAdMobOpButtonExtension implements MenginePluginExtensionInterface {
    private MengineAdService m_adService;

    static {
        MenginePluginExtensionManager.registerExtension("OpButton", MengineAdMobOpButtonExtension.class);
    }

    @Override
    public void onExtensionInitialize(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        if (application.findService("AdMob") == null) {
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
        //Empty
    }

    private void setupUI() {
        if (OpButton.isInitialized() == false) {
            return;
        }

        OpButtonUI ui = OpButton.getUI();
        OpButtonTab tab = ui.addTab("AdMob");
        OpButtonLayout root = tab.vertical();

        this.setupBanner(root);
        this.setupTopper(root);
        this.setupAdPoints(root);
    }

    private void setupBanner(@NonNull OpButtonLayout root) {
        root.addLabel("Banner").color("primary");

        OpButtonLayout statsRow = root.addHorizontal();

        statsRow.addStat("Can Show")
            .valueProvider(() -> m_adService.canYouShowBanner() == true ? "Yes" : "No");

        statsRow.addStat("Size")
            .unit("px")
            .valueProvider(() -> m_adService.getBannerWidth() + "x" + m_adService.getBannerHeight());

        OpButtonLayout btnsRow = root.addHorizontal();

        btnsRow.addButton("Show Banner")
            .variant("contained").color("success")
            .onClick(action -> {
                m_adService.showBanner();
            });

        btnsRow.addButton("Hide Banner")
            .variant("outlined").color("warning")
            .onClick(action -> {
                m_adService.hideBanner();
            });
    }

    private void setupTopper(@NonNull OpButtonLayout root) {
        root.addLabel("Topper").color("primary");

        OpButtonLayout statsRow = root.addHorizontal();

        statsRow.addStat("Can Show")
            .valueProvider(() -> m_adService.canYouShowTopper() == true ? "Yes" : "No");

        statsRow.addStat("Size")
            .unit("px")
            .valueProvider(() -> m_adService.getTopperWidth() + "x" + m_adService.getTopperHeight());

        OpButtonLayout btnsRow = root.addHorizontal();

        btnsRow.addButton("Show Topper")
            .variant("contained").color("success")
            .onClick(action -> {
                m_adService.showTopper();
            });

        btnsRow.addButton("Hide Topper")
            .variant("outlined").color("warning")
            .onClick(action -> {
                m_adService.hideTopper();
            });
    }

    private void setupAdPoints(@NonNull OpButtonLayout root) {
        List<MengineAdPointInterstitial> interstitialPoints = new ArrayList<>();
        List<MengineAdPointRewarded> rewardedPoints = new ArrayList<>();
        List<MengineAdPointAppOpen> appOpenPoints = new ArrayList<>();

        m_adService.forEachAdInterstitialPoint(interstitialPoints::add);
        m_adService.forEachAdRewardedPoint(rewardedPoints::add);
        m_adService.forEachAdAppOpenPoint(appOpenPoints::add);

        if (interstitialPoints.isEmpty() == false) {
            root.addLabel("Interstitial Points").color("primary");

            for (MengineAdPointInterstitial adPoint : interstitialPoints) {
                String pointName = adPoint.getName();
                OpButtonLayout content = root.addCollapsible(pointName);
                this.setupInterstitialPoint(content, adPoint);
            }
        }

        if (rewardedPoints.isEmpty() == false) {
            root.addLabel("Rewarded Points").color("primary");

            for (MengineAdPointRewarded adPoint : rewardedPoints) {
                String pointName = adPoint.getName();
                OpButtonLayout content = root.addCollapsible(pointName);
                this.setupRewardedPoint(content, adPoint);
            }
        }

        if (appOpenPoints.isEmpty() == false) {
            root.addLabel("AppOpen Points").color("primary");

            for (MengineAdPointAppOpen adPoint : appOpenPoints) {
                String pointName = adPoint.getName();
                OpButtonLayout content = root.addCollapsible(pointName);
                this.setupAppOpenPoint(content, adPoint);
            }
        }
    }

    private static String formatTimeMs(long ms) {
        if (ms < 0) {
            return "off";
        }

        long millisecondsPerSecond = 1000;
        long secondsPerMinute = 60;
        long seconds = ms / millisecondsPerSecond;

        if (seconds < secondsPerMinute) {
            String result = seconds + "s";

            return result;
        }

        long minutes = seconds / secondsPerMinute;
        long remainingSeconds = seconds % secondsPerMinute;
        String result = minutes + "m " + remainingSeconds + "s";

        return result;
    }

    private static String formatCooldownRemaining(MengineAdCooldown cooldown, long timeCooldownMs) {
        if (cooldown == null) {
            return "off";
        }

        if (timeCooldownMs <= 0) {
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

        String result = MengineAdMobOpButtonExtension.formatTimeMs(remaining);

        return result;
    }

    private void setupInterstitialPoint(@NonNull OpButtonLayout root, @NonNull MengineAdPointInterstitial adPoint) {
        String pointName = adPoint.getName();

        OpButtonLayout row1 = root.addHorizontal();

        row1.addStat("Enabled")
            .valueProvider(() -> adPoint.isEnabled() == true ? "Yes" : "No");

        row1.addStat("Attempts")
            .valueProvider(() -> {
                MengineAdAttempts attempts = adPoint.getAttempts();

                if (attempts == null) {
                    return 0;
                }

                int attemptsCount = attempts.getAttempts();

                return attemptsCount;
            });

        row1.addStat("Cooldown")
            .valueProvider(() -> {
                MengineAdCooldown cooldown = adPoint.getCooldown();
                long timeCooldownMs = adPoint.getTimeCooldown();

                String result = MengineAdMobOpButtonExtension.formatCooldownRemaining(cooldown, timeCooldownMs);

                return result;
            });

        OpButtonLayout row2 = root.addHorizontal();

        row2.addStat("Act Offset")
            .valueProvider(() -> {
                int value = adPoint.getActionOffset();

                return value;
            });

        row2.addStat("Act Cooldown")
            .valueProvider(() -> {
                int value = adPoint.getActionCooldown();

                return value;
            });

        row2.addStat("Session Offset")
            .valueProvider(() -> {
                long value = adPoint.getSessionOffset();

                return value;
            });

        OpButtonLayout row3 = root.addHorizontal();

        row3.addStat("Time Offset")
            .valueProvider(() -> {
                long timeMs = adPoint.getTimeOffset();

                String result = MengineAdMobOpButtonExtension.formatTimeMs(timeMs);

                return result;
            });

        row3.addStat("Time Cooldown")
            .valueProvider(() -> {
                long timeMs = adPoint.getTimeCooldown();

                String result = MengineAdMobOpButtonExtension.formatTimeMs(timeMs);

                return result;
            });

        row3.addStat("Install Offset")
            .valueProvider(() -> {
                long timeMs = adPoint.getInstallTimeOffset();

                String result = MengineAdMobOpButtonExtension.formatTimeMs(timeMs);

                return result;
            });

        String buttonTitle = "Show [" + pointName + "]";
        root.addButton(buttonTitle)
            .variant("contained").color("primary")
            .onClick(action -> {
                m_adService.showInterstitial(pointName);
            });
    }

    private void setupRewardedPoint(@NonNull OpButtonLayout root, @NonNull MengineAdPointRewarded adPoint) {
        String pointName = adPoint.getName();

        OpButtonLayout row = root.addHorizontal();

        row.addStat("Enabled")
            .valueProvider(() -> adPoint.isEnabled() == true ? "Yes" : "No");

        row.addStat("Attempts")
            .valueProvider(() -> {
                MengineAdAttempts attempts = adPoint.getAttempts();

                if (attempts == null) {
                    return 0;
                }

                int attemptsCount = attempts.getAttempts();

                return attemptsCount;
            });

        String buttonTitle = "Show [" + pointName + "]";
        root.addButton(buttonTitle)
            .variant("contained").color("primary")
            .onClick(action -> {
                m_adService.showRewarded(pointName);
            });

        String rewardedInterstitialTitle = "Show rewarded interstitial [" + pointName + "]";
        root.addButton(rewardedInterstitialTitle)
            .variant("contained").color("secondary")
            .onClick(action -> {
                m_adService.showRewardedInterstitial(pointName);
            });
    }

    private void setupAppOpenPoint(@NonNull OpButtonLayout root, @NonNull MengineAdPointAppOpen adPoint) {
        String pointName = adPoint.getName();

        OpButtonLayout row1 = root.addHorizontal();

        row1.addStat("Enabled")
            .valueProvider(() -> adPoint.isEnabled() == true ? "Yes" : "No");

        row1.addStat("Attempts")
            .valueProvider(() -> {
                MengineAdAttempts attempts = adPoint.getAttempts();

                if (attempts == null) {
                    return 0;
                }

                int attemptsCount = attempts.getAttempts();

                return attemptsCount;
            });

        row1.addStat("Cooldown")
            .valueProvider(() -> {
                MengineAdCooldown cooldown = adPoint.getCooldown();
                long timeCooldownMs = adPoint.getTimeCooldown();

                String result = MengineAdMobOpButtonExtension.formatCooldownRemaining(cooldown, timeCooldownMs);

                return result;
            });

        OpButtonLayout row2 = root.addHorizontal();

        row2.addStat("Act Offset")
            .valueProvider(() -> {
                int value = adPoint.getActionOffset();

                return value;
            });

        row2.addStat("Act Cooldown")
            .valueProvider(() -> {
                int value = adPoint.getActionCooldown();

                return value;
            });

        row2.addStat("Time Sleep")
            .valueProvider(() -> {
                long timeMs = adPoint.getTimeSleep();

                String result = MengineAdMobOpButtonExtension.formatTimeMs(timeMs);

                return result;
            });

        OpButtonLayout row3 = root.addHorizontal();

        row3.addStat("Time Offset")
            .valueProvider(() -> {
                long timeMs = adPoint.getTimeOffset();

                String result = MengineAdMobOpButtonExtension.formatTimeMs(timeMs);

                return result;
            });

        row3.addStat("Time Cooldown")
            .valueProvider(() -> {
                long timeMs = adPoint.getTimeCooldown();

                String result = MengineAdMobOpButtonExtension.formatTimeMs(timeMs);

                return result;
            });

        row3.addStat("Install Offset")
            .valueProvider(() -> {
                long timeMs = adPoint.getInstallTimeOffset();

                String result = MengineAdMobOpButtonExtension.formatTimeMs(timeMs);

                return result;
            });
    }
}
