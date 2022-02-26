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


    public static GPathResult GetAndroidManifest(Project project) {
        String name = project.getName()
        //String projectPath = project(":" + nameProj).projectDir.absolutePath + "/" + name + "/"
        //println name
        //println project.projectDir.absolutePath
        String projectPath = project.projectDir.absolutePath + "/"// + name + "/../../"
        if (!projectPath.contains(name))
            projectPath += "/" + name + "/"

        String pathToManifest = (new File(projectPath + "AndroidManifest.xml")).absolutePath
        //println pathToManifest

        if (!(new File(pathToManifest)).exists())
            throw new Exception(" not finded  AndroidManifest " + projectPath)
        def AndroidManifest = new XmlSlurper().parse(pathToManifest)
        return AndroidManifest;
    }

    public static String GetAppId(Project project) {
        def AndroidManifest = GetAndroidManifest(project)
        def AppId = AndroidManifest.@package.text()
        return AppId;
    }

    public static String GetBuildName(Project project) {
        def AndroidManifest = GetAndroidManifest(project)

        //def AppId = AndroidManifest.@package.text()
        //def minSdk = Integer.parseInt(AndroidManifest.'uses-sdk'.@'android:minSdkVersion'.text())
        //def targetSdk = Integer.parseInt(AndroidManifest.'uses-sdk'.@'android:targetSdkVersion'.text())
        def VersionCode = Integer.parseInt(AndroidManifest.@'android:versionCode'.text())

        def buildName = project.getName() + "_" + VersionCode + "__" + new SimpleDateFormat("dd-MM-yyyy_HH-mm").format(new Date())
        //+ ".apk";
    }


}
