package org.Mengine.Plugin.AdMob;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MengineServiceInvalidInitializeException;

public class MengineAdMobPlugin extends MengineService implements MengineListenerApplication {
    public static final String SERVICE_NAME = "MengineAdMob";

    @Override
    public void onAppCreate(MengineApplication application) throws MengineServiceInvalidInitializeException {
        // Empty
    }
}
