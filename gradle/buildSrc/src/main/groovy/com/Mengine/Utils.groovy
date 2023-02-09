package com.Mengine

import org.gradle.api.Project

class Utils {
    public static Boolean getBooleanProperty(Project project, String name, Boolean d) {
        def p = project.getProperties().get(name, null)

        if (p == null) {
            return d
        }

        if (p as Integer == 0) {
            return false
        }

        return true
    }

    public static Boolean getStringProperty(Project project, String name, String d) {
        def p = project.getProperties().get(name, null)

        if (p == null) {
            return d
        }

        return p
    }

    public static Integer getIntegerProperty(Project project, String name, Integer d) {
        def p = project.getProperties().get(name, null)

        if (p == null) {
            return d
        }

        def v = p as Integer

        return v
    }

    public static Boolean existAppPlugin(Project project, String name) {
        def p = project.getProperties().get(name, null)

        if (p == null) {
            return false
        }

        if (p as Integer == 0) {
            return false
        }

        return true
    }
}
