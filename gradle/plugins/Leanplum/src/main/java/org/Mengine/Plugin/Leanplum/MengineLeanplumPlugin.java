package org.Mengine.Plugin.Leanplum;

import androidx.annotation.StringRes;

import com.leanplum.Leanplum;
import com.leanplum.LeanplumActivityHelper;
import com.leanplum.annotations.Parser;
import com.leanplum.internal.Log;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineListenerUser;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

public class MengineLeanplumPlugin extends MengineService implements MengineListenerApplication, MengineListenerUser {
    public static final String SERVICE_NAME = "Leanplum";

    public static final @StringRes int METADATA_APP_ID = R.string.mengine_leanplum_app_id;
    public static final @StringRes int METADATA_DEV_KEY = R.string.mengine_leanplum_dev_key;
    public static final @StringRes int METADATA_PROD_KEY = R.string.mengine_leanplum_prod_key;
    public static final @StringRes int METADATA_ENVIRONMENT = R.string.mengine_leanplum_environment;

    @Override
    public void onAppCreate(MengineApplication application) throws MengineServiceInvalidInitializeException {
        String AppId = this.getResourceString(METADATA_APP_ID);
        String DevKey = this.getResourceString(METADATA_DEV_KEY);
        String ProdKey = this.getResourceString(METADATA_PROD_KEY);
        String Environment = this.getResourceString(METADATA_ENVIRONMENT);

        this.logInfo("%s: %s"
            , this.getResourceName(METADATA_APP_ID)
            , MengineUtils.getRedactedValue(AppId)
        );

        if (BuildConfig.DEBUG == true) {
            this.logInfo("%s: %s"
                , this.getResourceName(METADATA_DEV_KEY)
                , MengineUtils.getRedactedValue(DevKey)
            );
        } else {
            this.logInfo("%s: %s"
                , this.getResourceName(METADATA_PROD_KEY)
                , MengineUtils.getRedactedValue(ProdKey)
            );
        }

        this.logInfo("%s: %s"
            , this.getResourceName(METADATA_ENVIRONMENT)
            , Environment
        );

        String userId = application.getUserId();
        Leanplum.setUserId(userId);

        Leanplum.setApplicationContext(application);
        Parser.parseVariables(application);

        LeanplumActivityHelper.enableLifecycleCallbacks(application);

        if (BuildConfig.DEBUG == true) {
            Leanplum.setLogLevel(Log.Level.DEBUG);
            Leanplum.setAppIdForDevelopmentMode(AppId, DevKey);
        } else {
            Leanplum.setAppIdForProductionMode(AppId, ProdKey);
        }

        Leanplum.start(application);
    }

    @Override
    public void onMengineChangeUserId(MengineApplication application, String oldUserId, String newUserId) {
        Leanplum.setUserId(newUserId);
    }

    @Override
    public void onMengineRemoveUserData(MengineApplication application) {
        Leanplum.setUserId(null);
        Leanplum.clearUserContent();
    }
}
