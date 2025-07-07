package org.Mengine.Base;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

public final class MengineTag {
    private static final int MAX_LENGTH = 23;

    private final String value;

    private MengineTag(@NonNull @Size(min = 1L,max = MengineTag.MAX_LENGTH) String value) {
        this.value = value;
    }

    public static MengineTag of(@NonNull @Size(min = 1L,max = MengineTag.MAX_LENGTH) String value) {
        return new MengineTag(value);
    }

    @Override
    public String toString() {
        return value;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }

        if (o instanceof MengineTag == false) {
            return false;
        }

        MengineTag tag = (MengineTag)o;

        return value.equals(tag.value);
    }

    @Override
    public int hashCode() {
        return value.hashCode();
    }
}