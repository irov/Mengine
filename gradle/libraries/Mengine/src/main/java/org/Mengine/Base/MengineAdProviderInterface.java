package org.Mengine.Base;

public interface MengineAdProviderInterface {
    boolean hasBanner();
    boolean isBannerLoaded();

    boolean canYouShowBanner();
    void showBanner();
    void hideBanner();

    int getBannerWidth();
    int getBannerHeight();

    boolean hasTopper();
    boolean isTopperLoaded();

    boolean canYouShowTopper();
    void showTopper();
    void hideTopper();

    int getTopperWidth();
    int getTopperHeight();

    boolean hasInterstitial();

    boolean canYouShowInterstitial(String placement);
    boolean showInterstitial(String placement);
    boolean isShowingInterstitial();

    boolean hasRewarded();

    boolean canOfferRewarded(String placement);
    boolean canYouShowRewarded(String placement);
    boolean showRewarded(String placement);
    boolean isShowingRewarded();

    boolean hasRewardedInterstitial();

    boolean canYouShowRewardedInterstitial(String placement);
    boolean showRewardedInterstitial(String placement);
    boolean isShowingRewardedInterstitial();

    boolean hasAppOpen();

    boolean canYouShowAppOpen(String placement, long timeStop);
    boolean showAppOpen(String placement);

    boolean hasMREC();

    boolean canYouShowMREC();
    void showMREC(int leftMargin, int topMargin);
    void hideMREC();

    int getMRECLeftMargin();
    int getMRECTopMargin();
    int getMRECWidth();
    int getMRECHeight();

    boolean hasNative();

    boolean canYouShowNative();
    void showNative();
    void hideNative();

    int getNativeLeftMargin();
    int getNativeTopMargin();
    int getNativeWidth();
    int getNativeHeight();
}