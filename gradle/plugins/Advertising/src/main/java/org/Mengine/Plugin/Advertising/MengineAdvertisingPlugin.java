package org.Mengine.Plugin.Advertising;

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

    private static String m_advertisingId;
    private static boolean m_advertisingLimitTrackingEnabled;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        ExecutorService executor = Executors.newSingleThreadExecutor();

        executor.execute(() -> {
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

    public void onPostAdInfo(MengineActivity activity, AdvertisingIdClient.Info adInfo) {
        Handler handler = new Handler(Looper.getMainLooper());

        handler.post(() -> {
            if (adInfo == null ) {
                MengineAdvertisingPlugin.m_advertisingId = "00000000-0000-0000-0000-000000000000";
                MengineAdvertisingPlugin.m_advertisingLimitTrackingEnabled = true;
            } else if( adInfo.isLimitAdTrackingEnabled() == true) {
                MengineAdvertisingPlugin.m_advertisingId = "00000000-0000-0000-0000-000000000000";
                MengineAdvertisingPlugin.m_advertisingLimitTrackingEnabled = true;
            } else {
                MengineAdvertisingPlugin.m_advertisingId = adInfo.getId();
                MengineAdvertisingPlugin.m_advertisingLimitTrackingEnabled = false;
            }

            MengineAdvertisingPlugin.this.logInfo("AdvertisingId: %s [%s]"
                , MengineAdvertisingPlugin.m_advertisingId
                , MengineAdvertisingPlugin.m_advertisingLimitTrackingEnabled == true ? "true" : "false"
            );

            activity.sendEvent("AdvertisingId", MengineAdvertisingPlugin.m_advertisingId, MengineAdvertisingPlugin.m_advertisingLimitTrackingEnabled);
        });
    }
}
