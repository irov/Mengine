package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineProcedureInterface extends MengineFactorable {
    boolean execute(@NonNull MengineActivity activity);
}