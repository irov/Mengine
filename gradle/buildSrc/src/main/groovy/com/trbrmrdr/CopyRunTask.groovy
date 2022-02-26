package com.trbrmrdr


import org.gradle.api.DefaultTask
import org.gradle.api.Project
import org.gradle.api.tasks.Input
import org.gradle.api.tasks.TaskAction
import org.gradle.process.ExecResult

class CopyRunTask extends DefaultTask {

    @Input
    def hasBundle = false

    @Input
    Project targetProject
    @Input
    String pathDirOut

    @Input
    def hasRun = false
    @Input
    def uninstall = true
    @Input
    def clean_in_android = true

    @TaskAction
    def runQuery() {
        def pathIn = "${targetProject.buildDir.absolutePath}/outputs/apk/release/${targetProject.name}-release.apk"
        if (hasBundle) {
            pathIn = "${targetProject.buildDir.absolutePath}/outputs/bundle/release/${targetProject.name}-release.aab"
        }
//        def pathDirOut = "${this.projectDir}/../builds/"
        (new File(pathDirOut)).mkdirs()

        def applicationId = targetProject.android.defaultConfig.applicationId
        def versionCode = targetProject.android.defaultConfig.versionCode
        def versionName = targetProject.android.defaultConfig.versionName
        def formattedDate = new Date().format('dd.MM.yy~HH-mm')

        def pathOut = "${pathDirOut}/${applicationId} - $versionCode($versionName) - ${formattedDate}.apk"
        if (hasBundle) {
            pathOut = "${pathDirOut}/${applicationId} - $versionCode($versionName) - ${formattedDate}.aab"
        }
        (new File(pathOut)).delete()

        exec(["cp", pathIn, pathOut])

        try {
            if (hasRun) {
                def ret = exec(["adb", "devices"])
                def lines = -1
                println "+++++++"
                ret.split("\n").each { line ->
                    lines++
                    if (lines == 0) return
                    def param = line.split()
                    try {
                        def ssid = param[0]
                        if (param.length >= 3) {
                            if (param[2].contains("offline")) {
                                return
                            }
                        }
//                        println ssid
                        shellInDevice(applicationId, pathIn, ssid)
                    } catch (Exception ex) {
                        println ex.localizedMessage
                    }

                }
            }

        } catch (Exception e) {
            println e.localizedMessage
        }
    }

    def shellInDevice(String applicationId, String apkPath, String ssid) {
        if (clean_in_android) {
            exec(["adb", "-s", ssid, "shell", "rm ./data/local/tmp/*"])
        }
        if (uninstall) {
            exec(["adb", "-s", ssid, "shell", "pm uninstall $applicationId"])
        }
        def vendorName = "com.android.vending"
//                if (nameProj.contains("amazon"))
//                    vendorName = "com.amazon.venezia"

        if (HelperUtils.hasUNIX())
            exec(["adb", "-s", ssid, "install", "-r", "-i $vendorName", apkPath.replace("\"", "\'")], false)
        else
            exec(["adb -s $ssid install  -r -i \"$vendorName\" \"$apkPath\""], false)

        exec(["adb", "-s", ssid, "shell", "monkey -p  $applicationId -c android.intent.category.LAUNCHER 1"])
    }

    def exec(Collection<String> param, Boolean skipCrash = true) {
        println param
        param = HelperUtils.fixOS(param)

        def logParam = ""
        param.each { it -> logParam += " [" + it + "]" }
        println logParam

        try {
            def stdout = new ByteArrayOutputStream()

            targetProject.exec {
                commandLine param
                standardOutput = stdout
            }

            return stdout.toString()
        } catch (Exception ex) {
            if (!skipCrash)
                throw ex
        }
    }

}
