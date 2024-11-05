package org.Mengine.Plugin.Advertising;

import android.content.Context;

import com.google.android.gms.ads.identifier.AdvertisingIdClient;
import com.google.android.gms.common.GooglePlayServicesNotAvailableException;
import com.google.android.gms.common.GooglePlayServicesRepairableException;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Plugin.GoogleService.MengineGoogleServicePlugin;

import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class MengineAdvertisingPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "MengineAdvertising";

    private static final String LIMIT_ADVERTISING_ID = "00000000-0000-0000-0000-000000000000";

    private Future<AdvertisingIdClient.Info> m_advertisingFuture;

    @Override
    public boolean onAvailable(MengineApplication application) {
        if (MengineGoogleServicePlugin.isGooglePlayServicesAvailable(application) == false) {
            return false;
        }

        return true;
    }

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        ExecutorService executor = Executors.newSingleThreadExecutor();

        Future<AdvertisingIdClient.Info> future = executor.submit(() -> {
            try {
                Context context = application.getApplicationContext();
                AdvertisingIdClient.Info adInfo = AdvertisingIdClient.getAdvertisingIdInfo(context);

                this.postAdInfo(application, adInfo);

                return adInfo;
            } catch (final IOException e) {
                MengineAdvertisingPlugin.this.logError("[ERROR] invalid get advertising id info IOException: %s"
                    , e.getMessage()
                );
            } catch (final IllegalStateException e) {
                MengineAdvertisingPlugin.this.logError("[ERROR] invalid get advertising id info IllegalStateException: %s"
                    , e.getMessage()
                );
            } catch (final GooglePlayServicesNotAvailableException e) {
                MengineAdvertisingPlugin.this.logError("[ERROR] invalid get advertising id info GooglePlayServicesNotAvailableException: %s"
                    , e.getMessage()
                );
            } catch (final GooglePlayServicesRepairableException e) {
                MengineAdvertisingPlugin.this.logError("[ERROR] invalid get advertising id info GooglePlayServicesRepairableException: %s"
                    , e.getMessage()
                );
            }

            return null;
        });

        m_advertisingFuture = future;
    }

    @Override
    public void onAppTerminate(MengineApplication application) {
        if (m_advertisingFuture == null) {
            return;
        }

        m_advertisingFuture.cancel(true);
        m_advertisingFuture = null;
    }

    private void postAdInfo(MengineApplication application, AdvertisingIdClient.Info adInfo) {
        String advertisingId;
        boolean advertisingLimitTrackingEnabled;

        if (adInfo == null) {
            advertisingId = LIMIT_ADVERTISING_ID;
            advertisingLimitTrackingEnabled = true;
        } else if (adInfo.isLimitAdTrackingEnabled() == true) {
            advertisingId = LIMIT_ADVERTISING_ID;
            advertisingLimitTrackingEnabled = true;
        } else {
            String adInfoAdvertisingId = adInfo.getId();

            if (adInfoAdvertisingId == null || adInfoAdvertisingId.equals(LIMIT_ADVERTISING_ID) == true) {
                advertisingId = LIMIT_ADVERTISING_ID;
                advertisingLimitTrackingEnabled = true;
            } else {
                advertisingId = adInfoAdvertisingId;
                advertisingLimitTrackingEnabled = false;
            }
        }

        this.logMessage("AdvertisingId: %s limit: %s"
            , advertisingId
            , advertisingLimitTrackingEnabled == true ? "true" : "false"
        );

        application.onMengineAdvertisingId(advertisingId, advertisingLimitTrackingEnabled);
    }
}
