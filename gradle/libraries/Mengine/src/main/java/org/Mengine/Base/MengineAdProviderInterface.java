package org.Mengine.Base;

public interface MengineAdProviderInterface {
    boolean hasBanner();

    void showBanner();
    void hideBanner();

    int getBannerHeight();

    boolean hasInterstitial();

    boolean canYouShowInterstitial(String placement);
    boolean showInterstitial(String placement);

    boolean hasRewarded();

    boolean canOfferRewarded(String placement);
    boolean canYouShowRewarded(String placement);

    boolean showRewarded(String placement);

    boolean hasAppOpen();

    boolean canYouShowAppOpen(String placement, long timeStop);
    boolean showAppOpen(String placement);
}