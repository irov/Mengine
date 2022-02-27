package com.Mengine

import org.gradle.api.Project

class Utils {

    public static Boolean getBooleanProperty(Project project, String name, Boolean d = false) {
        def p = project.getProperties().get(name, null)

        if (p == null) {
            return d
        }

        if (p as Integer == 0) {
            return false
        }

        return true
    }


}
