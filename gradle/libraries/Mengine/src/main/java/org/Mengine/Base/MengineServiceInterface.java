package org.Mengine.Base;

public interface MengineServiceInterface {
    default boolean onAvailable(MengineApplication application) {
        //Override

        return true;
    }

    String getServiceName();
}
