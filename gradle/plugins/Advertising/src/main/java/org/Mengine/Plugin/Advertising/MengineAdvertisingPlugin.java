package org.Mengine.Plugin.Advertising;

import android.content.Context;

import com.google.android.gms.ads.identifier.AdvertisingIdClient;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GoogleApiAvailability;
import com.google.android.gms.common.GooglePlayServicesNotAvailableException;
import com.google.android.gms.common.GooglePlayServicesRepairableException;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;

import java.io.IOException;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class MengineAdvertisingPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "Advertising";

    private static final String LIMIT_ADVERTISING_ID = "00000000-0000-0000-0000-000000000000";

    private Future<AdvertisingIdClient.Info> m_advertisingFuture;

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        GoogleApiAvailability apiAvailability = GoogleApiAvailability.getInstance();

        final Context context = application.getApplicationContext();
        final int resultCode = apiAvailability.isGooglePlayServicesAvailable(context);

        if (resultCode != ConnectionResult.SUCCESS) {
            return;
        }

        ExecutorService executor = Executors.newSingleThreadExecutor();

        Future<AdvertisingIdClient.Info> future = executor.submit(() -> {
            try {
                AdvertisingIdClient.Info adInfo = AdvertisingIdClient.getAdvertisingIdInfo(context);

                return adInfo;
            } catch (IOException e) {
                MengineAdvertisingPlugin.this.logError("[ERROR] invalid get advertising id info IOException: %s"
                    , e.getMessage()
                );
            } catch (IllegalStateException e) {
                MengineAdvertisingPlugin.this.logError("[ERROR] invalid get advertising id info IllegalStateException: %s"
                    , e.getMessage()
                );
            } catch (GooglePlayServicesNotAvailableException e) {
                MengineAdvertisingPlugin.this.logError("[ERROR] invalid get advertising id info GooglePlayServicesNotAvailableException: %s"
                    , e.getMessage()
                );
            } catch (GooglePlayServicesRepairableException e) {
                MengineAdvertisingPlugin.this.logError("[ERROR] invalid get advertising id info GooglePlayServicesRepairableException: %s"
                    , e.getMessage()
                );
            }

            return null;
        });

        m_advertisingFuture = future;
    }

    public void onMengineInitializeBaseServices(MengineActivity activity) {
        if (m_advertisingFuture == null) {
            return;
        }

        AdvertisingIdClient.Info adInfo = null;

        try {
            adInfo = m_advertisingFuture.get();
        } catch (CancellationException ex) {
            this.logError("[ERROR] invalid get advertising id CancellationException: %s"
                , ex.getMessage()
            );
        } catch (InterruptedException ex) {
            this.logError("[ERROR] invalid get advertising id InterruptedException: %s"
                , ex.getMessage()
            );
        } catch (ExecutionException ex) {
            this.logError("[ERROR] invalid get advertising id ExecutionException: %s"
                , ex.getMessage()
            );
        }

        m_advertisingFuture = null;

        this.postAdInfo(adInfo);
    }

    private void postAdInfo(AdvertisingIdClient.Info adInfo) {
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

            if (adInfoAdvertisingId.equals(LIMIT_ADVERTISING_ID) == true) {
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

        this.sendEvent(MengineEvent.EVENT_ADVERTISING_ID, advertisingId, advertisingLimitTrackingEnabled);
    }
}
