package org.Mengine.Plugin.Leanplum;

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

    public static final String METADATA_APP_ID = "mengine.leanplum.app_id";
    public static final String METADATA_DEV_KEY = "mengine.leanplum.dev_key";
    public static final String METADATA_PROD_KEY = "mengine.leanplum.prod_key";
    public static final String METADATA_ENVIRONMENT = "mengine.leanplum.environment";

    @Override
    public void onAppCreate(MengineApplication application) throws MengineServiceInvalidInitializeException {
        String AppId = this.getMetaDataString(METADATA_APP_ID);
        String DevKey = this.getMetaDataString(METADATA_DEV_KEY);
        String ProdKey = this.getMetaDataString(METADATA_PROD_KEY);
        String Environment = this.getMetaDataString(METADATA_ENVIRONMENT);

        this.logInfo("%s: %s"
            , METADATA_APP_ID
            , MengineUtils.getRedactedValue(AppId)
        );

        if (BuildConfig.DEBUG == true) {
            this.logInfo("%s: %s"
                , METADATA_DEV_KEY
                , MengineUtils.getRedactedValue(DevKey)
            );
        } else {
            this.logInfo("%s: %s"
                , METADATA_PROD_KEY
                , MengineUtils.getRedactedValue(ProdKey)
            );
        }

        this.logInfo("%s: %s"
            , METADATA_ENVIRONMENT
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
