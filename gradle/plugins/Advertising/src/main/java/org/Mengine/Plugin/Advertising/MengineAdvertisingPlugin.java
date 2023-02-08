package org.Mengine.Plugin.Advertising;

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
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class MengineAdvertisingPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "Advertising";

    private String m_advertisingId;
    private boolean m_advertisingLimitTrackingEnabled;

    private ExecutorService m_executor;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        m_executor = Executors.newSingleThreadExecutor();

        m_executor.execute(() -> {
            final Context context = activity.getApplicationContext();

            AdvertisingIdClient.Info adInfo = null;

            try {
                adInfo = AdvertisingIdClient.getAdvertisingIdInfo(context);
            } catch (IOException e) {
                e.printStackTrace();
            } catch (IllegalStateException e) {
                e.printStackTrace();
            } catch (GooglePlayServicesNotAvailableException e) {
                e.printStackTrace();
            } catch (GooglePlayServicesRepairableException e) {
                e.printStackTrace();
            }

            MengineAdvertisingPlugin.this.onPostAdInfo(activity, adInfo);
        });
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        m_executor.shutdown();
        m_executor = null;
    }

    public void onPostAdInfo(MengineActivity activity, AdvertisingIdClient.Info adInfo) {
        MengineUtils.performOnMainThread(() -> {
            if (adInfo == null ) {
                MengineAdvertisingPlugin.this.m_advertisingId = "00000000-0000-0000-0000-000000000000";
                MengineAdvertisingPlugin.this.m_advertisingLimitTrackingEnabled = true;
            } else if( adInfo.isLimitAdTrackingEnabled() == true) {
                MengineAdvertisingPlugin.this.m_advertisingId = "00000000-0000-0000-0000-000000000000";
                MengineAdvertisingPlugin.this.m_advertisingLimitTrackingEnabled = true;
            } else {
                MengineAdvertisingPlugin.this.m_advertisingId = adInfo.getId();
                MengineAdvertisingPlugin.this.m_advertisingLimitTrackingEnabled = false;
            }

            MengineAdvertisingPlugin.this.logInfo("AdvertisingId: %s [%s]"
                , MengineAdvertisingPlugin.this.m_advertisingId
                , MengineAdvertisingPlugin.this.m_advertisingLimitTrackingEnabled == true ? "true" : "false"
            );

            activity.sendEvent("AdvertisingId", MengineAdvertisingPlugin.this.m_advertisingId, MengineAdvertisingPlugin.this.m_advertisingLimitTrackingEnabled);
        });
    }
}
