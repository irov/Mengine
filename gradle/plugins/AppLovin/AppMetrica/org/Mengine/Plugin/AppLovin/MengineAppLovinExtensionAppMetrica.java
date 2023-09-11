package org.Mengine.Plugin.AppLovin;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginExtension;

import com.applovin.mediation.MaxAdFormat;

import com.applovin.mediation.MaxAd;
import com.yandex.metrica.AdRevenue;
import com.yandex.metrica.AdType;
import com.yandex.metrica.YandexMetrica;

import java.util.Currency;

public class MengineAppLovinExtensionAppMetrica extends MenginePluginExtension implements MengineAppLovinAnalyticsListener {
    static AdType getYandexAdType(MaxAdFormat format) {
        if (format == MaxAdFormat.BANNER) {
            return AdType.BANNER;
        } else if (format == MaxAdFormat.LEADER) {
            return AdType.BANNER;
        } else if (format == MaxAdFormat.INTERSTITIAL) {
            return AdType.INTERSTITIAL;
        } else if (format == MaxAdFormat.REWARDED) {
            return AdType.REWARDED;
        } else if (format == MaxAdFormat.MREC) {
            return AdType.MREC;
        } else if (format == MaxAdFormat.NATIVE) {
            return AdType.NATIVE;
        }

        return AdType.OTHER;
    }

    @Override
    public void onEventRevenuePaid(MengineAppLovinPlugin plugin, MaxAd ad) {
        AdType adType = MengineAppLovinExtensionAppMetrica.getYandexAdType(ad.getFormat());

        AdRevenue adRevenue = AdRevenue.newBuilder(ad.getRevenue(), Currency.getInstance("USD"))
            .withAdNetwork(ad.getNetworkName())
            .withAdPlacementId(ad.getPlacement())
            .withAdType(adType)
            .withAdUnitId(ad.getAdUnitId())
            .withPrecision(ad.getRevenuePrecision())
            .build();

        YandexMetrica.reportAdRevenue(adRevenue);
    }
}
