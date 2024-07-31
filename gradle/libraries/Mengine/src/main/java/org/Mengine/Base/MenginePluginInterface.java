package org.Mengine.Base;

public interface MenginePluginInterface {
    default boolean onAvailable(MengineApplication application) {
        //Override

        return true;
    }

    String getPluginName();
}
