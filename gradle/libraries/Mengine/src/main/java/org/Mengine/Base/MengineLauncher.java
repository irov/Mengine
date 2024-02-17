package org.Mengine.Base;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

import java.util.ArrayList;
public class MengineLauncher extends Activity {
    public static final String TAG = "MengineLauncher";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        MengineApplication application;

        try {
            application = (MengineApplication) this.getApplication();
        } catch (ClassCastException e) {
            MengineUtils.finishActivityWithAlertDialog(this, "[ERROR] onCreate invalid application ClassCastException: %s"
                , e.getMessage()
            );

            return;
        }

        if (application.isInvalidInitialize() == true) {
            String reason = application.getInvalidInitializeReason();

            MengineUtils.finishActivityWithAlertDialog(this, "[ERROR] onCreate invalid application initialize: %s"
                , reason
            );

            return;
        }

        String[] activities = application.getAndroidActivities();

        if (activities.length == 0) {
            Intent intent = new Intent(this, MengineActivity.class);
            this.startActivity(intent);
            this.finish();

            return;
        }

        ArrayList<String> chainActivities = new ArrayList<>();

        for (String activity : activities) {
            chainActivities.add(activity);
        };

        chainActivities.add(MengineActivity.class.getName());

        String startClassName = chainActivities.get(0);
        chainActivities.remove(0);

        try {
            MengineUtils.startChainActivity(this, startClassName, chainActivities);
        } catch (MengineChainActivityNotFoundException e) {
            MengineUtils.finishActivityWithAlertDialog(this, "[ERROR] not found chain activity: %s"
                , e.getClassName()
            );
        }
    }
}
