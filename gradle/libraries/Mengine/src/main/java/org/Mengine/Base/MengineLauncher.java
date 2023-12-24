package org.Mengine.Base;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MengineLauncher extends Activity {
    public static final String TAG = "MengineLauncher";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        MengineApplication application = (MengineApplication)this.getApplication();

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
