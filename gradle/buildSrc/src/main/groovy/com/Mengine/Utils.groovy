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

    public static String normalizeAsssetsPath(Project project, String path) {
        String currPath = project.projectDir.absolutePath
        if (new File(currPath + "/../" + path).exists()) {
            return currPath + "/../" + path
        }

        String ret = "./"
        for (int i = currPath.findAll('/').size(); i > 0; i--) {
            ret += "../"
        }
        ret = ret.substring(0, ret.size() - 1)
        ret += path
        return ret
    }

}
