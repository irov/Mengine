package org.Mengine.Base;

import androidx.annotation.NonNull;

public final class MengineProcedureManager {
    public static boolean execute(@NonNull MengineActivity activity, @NonNull String id, Object... args) throws IllegalArgumentException {
        MengineFactorable factorable = MengineFactoryManager.build(id, args);

        if (factorable instanceof MengineProcedureInterface procedure) {
            boolean result = procedure.execute(activity);

            return result;
        }

        throw new IllegalArgumentException(String.format("procedure factorable %s is not instance of MengineProcedureInterface", id));
    }
}