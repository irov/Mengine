package org.Mengine.Build.DevToDev;

import android.app.Activity;

import com.devtodev.core.DevToDev;

/**
 * Created by sweatcoin7 on 7/13/18.
 */

public class DevToDevInteractionLayer {
    private static final String APPID = "eb9820ae-f1aa-09f2-9295-5b1d931f7d64";
    private static final String SECRET = "YeSsjbXTg6GAM5lPBEOI3Jx2ZNh7qcDR";
    private static final String APIKEY =  "ak-BVbZMln2aXWOLEuSD1tipYsr8kqN5dIA";

    public DevToDevInteractionLayer(Activity activity) {

        DevToDev.init(activity, APPID, SECRET);
    }


}
