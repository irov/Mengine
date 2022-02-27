package com.trbrmrdr

import groovy.util.slurpersupport.GPathResult
import org.apache.tools.ant.taskdefs.condition.Os
import org.gradle.api.Project

import java.text.SimpleDateFormat

class Ext {

    public static Boolean GetBooleanProperty(Project project, String name, Boolean d = false) {
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
