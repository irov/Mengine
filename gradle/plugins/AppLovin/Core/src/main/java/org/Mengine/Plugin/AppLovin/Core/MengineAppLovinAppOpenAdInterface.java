package org.Mengine.Plugin.AppLovin.Core;

public interface MengineAppLovinAppOpenAdInterface extends MengineAppLovinAdInterface {
    boolean canYouShowAppOpen(String placement);

    boolean showAppOpen(String placement);
}
