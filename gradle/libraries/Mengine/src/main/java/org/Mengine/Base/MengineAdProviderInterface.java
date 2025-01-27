package org.Mengine.Base;

public interface MengineAdProviderInterface {
    boolean hasBanner();

    void showBanner();
    void hideBanner();

    int getBannerHeight();

    boolean hasInterstitial(String placement);

    boolean canYouShowInterstitial(String placement);
    boolean showInterstitial(String placement, MengineCallback showCallback);

    boolean hasRewarded(String placement);

    boolean canOfferRewarded(String placement);
    boolean canYouShowRewarded(String placement);

    boolean showRewarded(String placement, MengineCallback showCallback);
}