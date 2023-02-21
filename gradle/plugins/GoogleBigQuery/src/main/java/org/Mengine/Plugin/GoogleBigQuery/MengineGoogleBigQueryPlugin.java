package org.Mengine.Plugin.GoogleBigQuery;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginAnalyticsListener;

import android.os.Bundle;

import com.google.cloud.bigquery.BigQuery;
import com.google.cloud.bigquery.BigQueryError;
import com.google.cloud.bigquery.BigQueryException;
import com.google.cloud.bigquery.BigQueryOptions;
import com.google.cloud.bigquery.InsertAllRequest;
import com.google.cloud.bigquery.InsertAllResponse;
import com.google.cloud.bigquery.TableId;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MengineGoogleBigQueryPlugin extends MenginePlugin implements MenginePluginAnalyticsListener {
    public static String PLUGIN_NAME = "GoogleBigQuery";
    public static boolean PLUGIN_EMBEDDING = true;

    private BigQuery m_bigQuery;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        new Thread(() -> {
            BigQueryOptions options = BigQueryOptions.newBuilder()
                .setProjectId("holidayfun2-d0660")
                .build();

            BigQuery bigQuery = options.getService();

            MengineGoogleBigQueryPlugin.this.m_bigQuery = bigQuery;
        }).start();
    }

    private void request(TableId tableId, Map<String, Object> params) {
        InsertAllRequest request = InsertAllRequest.newBuilder(tableId)
            .addRow(params)
            .build();

        try {
            InsertAllResponse response =
                m_bigQuery.insertAll(request);

            if (response.hasErrors() == true) {
                // If any of the insertions failed, this lets you inspect the errors
                for (Map.Entry<Long, List<BigQueryError>> entry : response.getInsertErrors().entrySet()) {
                    this.logError("Response error: %s"
                        , entry.getValue()
                    );
                }
            }
        }  catch (BigQueryException e) {
            this.logError("Response exception: %s"
                , e.getLocalizedMessage()
            );
        }
    }

    @Override
    public void onMengineAnalyticsEvent(MengineApplication application, int eventType, String eventName, long timestamp, Map<String, Object> parameters) {
        if (m_bigQuery == null) {
            return;
        }

        TableId tableId = TableId.of("analitycs", eventName);

        switch (eventType) {
            case EAET_CUSTOM: {
                Map<String, Object> params = parameters;

                this.logInfo("request [CUSTOM] eventName: %s params: %s"
                    , eventName
                    , params
                );

                this.request(tableId, params);
            } break;
            case EAET_EARN_VIRTUAL_CURRENCY: {
                String virtualCurrencyName = (String)parameters.get("@INTERNAL_VIRTUAL_CURRENCY_NAME");
                Double value = (Double)parameters.get("@INTERNAL_VALUE");

                Map<String, Object> params = new HashMap<>();
                params.put("virtual_currency_name", virtualCurrencyName);
                params.put("value", value);

                this.logInfo("request [EARN_VIRTUAL_CURRENCY] eventName: %s params: %s"
                    , eventName
                    , params
                );

                this.request(tableId, params);
            } break;
            case EAET_SPEND_VIRTUAL_CURRENCY: {
                String itemName = (String)parameters.get("@INTERNAL_ITEM_NAME");
                String virtualCurrencyName = (String)parameters.get("@INTERNAL_VIRTUAL_CURRENCY_NAME");
                Double value = (Double)parameters.get("@INTERNAL_VALUE");

                Map<String, Object> params = new HashMap<>();
                params.put("item_name", itemName);
                params.put("virtual_currency_name", virtualCurrencyName);
                params.put("value", value);

                this.logInfo("request [SPEND_VIRTUAL_CURRENCY] eventName: %s params: %s"
                    , eventName
                    , params
                );

                this.request(tableId, params);
            } break;
            case EAET_UNLOCK_ACHIEVEMENT: {
                String achievementId = (String)parameters.get("@INTERNAL_ACHIEVEMENT_ID");

                Map<String, Object> params = new HashMap<>();
                params.put("achievement_id", achievementId);

                this.logInfo("request [UNLOCK_ACHIEVEMENT] eventName: %s params: %s"
                    , eventName
                    , params
                );

                this.request(tableId, params);
            } break;
            default: {
                this.logWarning("event: %s unknown type: %d"
                    , eventName
                    , eventType
                );
            } break;
        }
    }
}
