package org.Mengine.Plugin.Advertising;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineEvent;
import org.Mengine.Base.MenginePluginApplicationListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MengineUtils;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;

import com.google.android.gms.ads.identifier.AdvertisingIdClient;
import com.google.android.gms.common.GooglePlayServicesNotAvailableException;
import com.google.android.gms.common.GooglePlayServicesRepairableException;

import java.io.IOException;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class MengineAdvertisingPlugin extends MenginePlugin implements MenginePluginApplicationListener {
    public static final String PLUGIN_NAME = "Advertising";

    private String m_advertisingId;
    private boolean m_advertisingLimitTrackingEnabled;

    @Override
    public void onAppCreate(MengineApplication application) throws MenginePluginInvalidInitializeException {
        ExecutorService executor = Executors.newSingleThreadExecutor();

        Future<AdvertisingIdClient.Info> future = executor.submit(() -> {
            final Context context = application.getApplicationContext();

            try {
                AdvertisingIdClient.Info adInfo = AdvertisingIdClient.getAdvertisingIdInfo(context);

                return adInfo;
            } catch (IOException e) {
                MengineAdvertisingPlugin.this.logError("invalid get advertising id info IOException: %s"
                    , e.getLocalizedMessage()
                );
            } catch (IllegalStateException e) {
                MengineAdvertisingPlugin.this.logError("invalid get advertising id info IllegalStateException: %s"
                    , e.getLocalizedMessage()
                );
            } catch (GooglePlayServicesNotAvailableException e) {
                MengineAdvertisingPlugin.this.logError("invalid get advertising id info GooglePlayServicesNotAvailableException: %s"
                    , e.getLocalizedMessage()
                );
            } catch (GooglePlayServicesRepairableException e) {
                MengineAdvertisingPlugin.this.logError("invalid get advertising id info GooglePlayServicesRepairableException: %s"
                    , e.getLocalizedMessage()
                );
            }

            return null;
        });

        try {
            AdvertisingIdClient.Info adInfo = future.get();

            MengineAdvertisingPlugin.this.onPostAdInfo(application, adInfo);
        } catch (CancellationException ex) {
            this.logError("invalid get advertising id CancellationException: %s"
                , ex.getLocalizedMessage()
            );
        } catch (InterruptedException ex) {
            this.logError("invalid get advertising id InterruptedException: %s"
                , ex.getLocalizedMessage()
            );
        } catch (ExecutionException ex) {
            this.logError("invalid get advertising id ExecutionException: %s"
                , ex.getLocalizedMessage()
            );
        }
    }

    public void onPostAdInfo(MengineApplication application, AdvertisingIdClient.Info adInfo) {
        if (adInfo == null ) {
            MengineAdvertisingPlugin.this.m_advertisingId = "00000000-0000-0000-0000-000000000000";
            MengineAdvertisingPlugin.this.m_advertisingLimitTrackingEnabled = true;
        } else if( adInfo.isLimitAdTrackingEnabled() == true) {
            MengineAdvertisingPlugin.this.m_advertisingId = "00000000-0000-0000-0000-000000000000";
            MengineAdvertisingPlugin.this.m_advertisingLimitTrackingEnabled = true;
        } else {
            String advertisingId = adInfo.getId();

            if (advertisingId.equals("00000000-0000-0000-0000-000000000000") == true) {
                MengineAdvertisingPlugin.this.m_advertisingId = "00000000-0000-0000-0000-000000000000";
                MengineAdvertisingPlugin.this.m_advertisingLimitTrackingEnabled = true;
            } else {
                MengineAdvertisingPlugin.this.m_advertisingId = advertisingId;
                MengineAdvertisingPlugin.this.m_advertisingLimitTrackingEnabled = false;
            }
        }

        MengineAdvertisingPlugin.this.logMessage("AdvertisingId: %s limit: %s"
            , MengineAdvertisingPlugin.this.m_advertisingId
            , MengineAdvertisingPlugin.this.m_advertisingLimitTrackingEnabled == true ? "true" : "false"
        );

        application.sendEvent(MengineEvent.EVENT_ADVERTISING_ID, MengineAdvertisingPlugin.this.m_advertisingId, MengineAdvertisingPlugin.this.m_advertisingLimitTrackingEnabled);
    }
}
