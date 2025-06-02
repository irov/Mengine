package org.Mengine.Plugin.AppLovin.Core;

import org.Mengine.Base.MengineServiceInterface;

import java.util.List;

public interface MengineAppLovinPluginInterface extends MengineServiceInterface {
    List<MengineAppLovinMediationInterface> getMediations();
    MengineAppLovinNonetBannersInterface getNonetBanners();
}