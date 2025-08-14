package org.Mengine.Base;

import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Parcelable;

import androidx.annotation.NonNull;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;

public class MengineProcedureSendMail implements MengineProcedureInterface {
    private static final MengineTag TAG = MengineTag.of("MPSendMail");

    private final String m_email;
    private final String m_subject;
    private final String m_body;

    public MengineProcedureSendMail(@NonNull String email, @NonNull String subject, @NonNull String body) {
        this.m_email = email;
        this.m_subject = subject;
        this.m_body = body;
    }

    @Override
    public boolean execute(@NonNull MengineActivity activity) {
        MengineLog.logInfo(TAG, "linkingOpenMail mail: %s subject: %s body: %s"
            , m_email
            , m_subject
            , m_body
        );

        MengineApplication application = MengineApplication.INSTANCE;

        application.setState("open.mail", m_email);

        MengineAnalytics.buildEvent("mng_open_mail")
            .addParameterString("mail", m_email)
            .log();

        Context context = application.getApplicationContext();

        Intent intent = new Intent(Intent.ACTION_SEND_MULTIPLE);

        intent.setType("application/zip");

        StringBuilder body_builder = new StringBuilder(m_body);

        String installId = application.getInstallId();
        String userId = application.getUserId();
        String sessionId = application.getSessionId();
        String installationId = MengineFragmentRemoteConfig.INSTANCE.getInstallationId();

        body_builder.append("\n\n");
        body_builder.append("        Account Info:" + "\n");
        body_builder.append("        * install id: " + installId + "\n");
        body_builder.append("        * user id: " + userId + "\n");
        body_builder.append("        * session id: " + sessionId + "\n");
        body_builder.append("        * installation id: " + installationId + "\n");

        try {
            ArrayList<Parcelable> fileUris = new ArrayList<>();

            boolean hasAccount = MengineNative.AndroidEnvironmentService_hasCurrentAccount();

            if (hasAccount == true) {
                String extraPreferencesFolderName = MengineNative.AndroidEnvironmentService_getExtraPreferencesFolderName();
                String accountFolderName = MengineNative.AndroidEnvironmentService_getCurrentAccountFolderName();

                File filesDir = context.getFilesDir();

                File extraPreferencesFolder = new File(filesDir, extraPreferencesFolderName);
                File accountFolder = new File(extraPreferencesFolder, accountFolderName);

                File accountZipFile = MengineUtils.createTempFile(context, "mng_account_", ".zip");

                if (accountZipFile == null) {
                    MengineLog.logWarning(TAG, "linkingOpenMail invalid create temp file 'mng_account_***.zip' for mail: %s subject: %s"
                        , m_email
                        , m_subject
                    );

                    return false;
                }

                if (MengineUtils.zipFiles(accountFolder, accountZipFile) == true) {
                    Uri accountZIPUri = MengineUtils.getUriForFile(context, accountZipFile);

                    if (accountZIPUri == null) {
                        return false;
                    }

                    MengineLog.logInfo(TAG, "linkingOpenMail attach file '%s' for mail: %s subject: %s"
                        , accountZIPUri
                        , m_email
                        , m_subject
                    );

                    fileUris.add(accountZIPUri);
                } else {
                    body_builder.append("\n\n[ERROR] invalid zip account folder");

                    MengineLog.logWarning(TAG, "linkingOpenMail invalid zip account folder for mail: %s subject: %s"
                        , m_email
                        , m_subject
                    );
                }
            }

            File logFile = MengineUtils.createTempFile(context, "mng_log_", ".log");

            if (logFile == null) {
                MengineLog.logWarning(TAG, "linkingOpenMail invalid create temp file 'mng_log_***.log' for mail: %s subject: %s"
                    , m_email
                    , m_subject
                );

                return false;
            }

            OutputStreamWriter logFileStream = new OutputStreamWriter(new FileOutputStream(logFile), StandardCharsets.UTF_8);

            logFileStream.write("[BEGIN CURRENT LOG]\n\n");

            if (MengineNative.AndroidEnvironmentService_writeCurrentLogToFile(logFileStream) == true) {
                logFileStream.write("\n\n[END CURRENT LOG]");
                logFileStream.flush();
                logFileStream.close();

                File logZipFile = MengineUtils.createTempFile(context, "mng_log_", ".zip");

                if (logZipFile == null) {
                    MengineLog.logWarning(TAG, "linkingOpenMail invalid create temp file 'mng_log_***.zip' for mail: %s subject: %s"
                        , m_email
                        , m_subject
                    );

                    return false;
                }

                if (MengineUtils.zipFiles(logFile, logZipFile) == true) {
                    Uri logZipFileUri = MengineUtils.getUriForFile(context, logZipFile);

                    if (logZipFileUri == null) {
                        return false;
                    }

                    MengineLog.logInfo(TAG, "linkingOpenMail attach file '%s' for mail: %s subject: %s"
                        , logZipFileUri
                        , m_email
                        , m_subject
                    );

                    fileUris.add(logZipFileUri);
                } else {
                    body_builder.append("\n\n[ERROR] invalid zip current log file");

                    MengineLog.logMessage(TAG, "linkingOpenMail invalid zip current log file for mail: %s subject: %s"
                        , m_email
                        , m_subject
                    );
                }
            } else {
                body_builder.append("\n\n[ERROR] invalid write current log file");

                MengineLog.logMessage(TAG, "linkingOpenMail invalid write current log file for mail: %s subject: %s"
                    , m_email
                    , m_subject
                );
            }

            File oldLogFile = MengineUtils.createTempFile(context, "mng_old_log_", ".log");

            if (oldLogFile == null) {
                MengineLog.logWarning(TAG, "linkingOpenMail invalid create temp file 'mng_old_log_***.log' for mail: %s subject: %s"
                    , m_email
                    , m_subject
                );

                return false;
            }

            OutputStreamWriter oldLogFileStream = new OutputStreamWriter(new FileOutputStream(oldLogFile), StandardCharsets.UTF_8);

            oldLogFileStream.write("[BEGIN OLD LOG]\n\n");

            if (MengineNative.AndroidEnvironmentService_writeOldLogToFile(oldLogFileStream) == true) {
                oldLogFileStream.write("\n\n[END OLD LOG]");
                oldLogFileStream.flush();
                oldLogFileStream.close();

                File oldLogZipFile = MengineUtils.createTempFile(context, "mng_old_log_", ".zip");

                if (oldLogZipFile != null && MengineUtils.zipFiles(oldLogFile, oldLogZipFile) == true) {
                    Uri oldLogZipFileUri = MengineUtils.getUriForFile(context, oldLogZipFile);

                    if (oldLogZipFileUri == null) {
                        return false;
                    }

                    MengineLog.logInfo(TAG, "linkingOpenMail attach file '%s' for mail: %s subject: %s"
                        , oldLogZipFileUri
                        , m_email
                        , m_subject
                    );

                    fileUris.add(oldLogZipFileUri);
                } else {
                    body_builder.append("\n\n[ERROR] invalid zip old log file");

                    MengineLog.logMessage(TAG, "linkingOpenMail invalid zip old log file for mail: %s subject: %s"
                        , m_email
                        , m_subject
                    );
                }
            } else {
                body_builder.append("\n\nNOT_FOUND_OLD_LOG");

                MengineLog.logMessage(TAG, "linkingOpenMail not found old log file for mail: %s subject: %s"
                    , m_email
                    , m_subject
                );
            }

            intent.putParcelableArrayListExtra(Intent.EXTRA_STREAM, fileUris);
        } catch (IOException e) {
            body_builder.append("\n\n[ERROR] invalid attaches file");

            MengineLog.logError(TAG, "[ERROR] linkingOpenMail failed attaches file mail: %s subject: %s exception: %s"
                , m_email
                , m_subject
                , e.getMessage()
            );
        }

        String full_body = body_builder.toString();

        intent.putExtra(Intent.EXTRA_EMAIL, new String[] { m_email });
        intent.putExtra(Intent.EXTRA_SUBJECT, m_subject);
        intent.putExtra(Intent.EXTRA_TEXT, full_body);

        intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
        intent.addFlags(Intent.FLAG_GRANT_WRITE_URI_PERMISSION);

        Intent chooser = Intent.createChooser(intent, "Send Email");

        chooser.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

        try {
            activity.startActivity(chooser);
        } catch (Exception e) {
            MengineLog.logError(TAG, "[ERROR] linkingOpenMail failed start mail: %s subject: %s body: %s exception: %s"
                , m_email
                , m_subject
                , full_body
                , e.getMessage()
            );

            return false;
        }

        return true;
    }

    static {
        MengineFactoryManager.registerClazz("sendMail", MengineProcedureSendMail.class, String.class, String.class, String.class);
    }
}