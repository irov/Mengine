package com.Mengine

import org.gradle.api.Project

class Utils {
    static Boolean getBooleanProperty(Project project, String name, Boolean d) {
        def p = project.getProperties().get(name, null)

        if (p == null) {
            return d
        }

        if (p as Integer == 0) {
            return false
        }

        return true
    }

    static String getStringProperty(Project project, String name, String d) {
        def p = project.getProperties().get(name, null)

        if (p == null) {
            return d
        }

        return p
    }

    static Integer getIntegerProperty(Project project, String name, Integer d) {
        def p = project.getProperties().get(name, null)

        if (p == null) {
            return d
        }

        def v = p as Integer

        return v
    }

    static Boolean existAppPlugin(Project project, String name) {
        def p = project.getProperties().get(name, null)

        if (p == null) {
            return false
        }

        if (p as Integer == 0) {
            return false
        }

        return true
    }

    static void logAvailable(String name, boolean value) {
        if (value == false) {
            println "\u001b[31m" + name + ": " + value + "\u001b[0m"
        } else {
            println "\u001b[32m" + name + ": " + value + "\u001b[0m"
        }
    }

    static void logString(String name, String value) {
        println "\u001b[33m" + name + ": " + value + "\u001b[0m"
    }

    static void logInteger(String name, int value) {
        println "\u001b[33m" + name + ": " + value + "\u001b[0m"
    }
}
