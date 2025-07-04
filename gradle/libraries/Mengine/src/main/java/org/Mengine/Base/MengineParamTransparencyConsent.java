package org.Mengine.Base;

import android.content.Context;
import android.content.SharedPreferences;

import androidx.annotation.NonNull;
import androidx.preference.PreferenceManager;

import java.util.List;
import java.util.Objects;

public class MengineParamTransparencyConsent {
    private static final String TAG = "MengineParamTransparencyConsent";

    public static MengineConsentFlowUserGeography TRANSPARENCYCONSENT_USERGEOGRAPHY = MengineConsentFlowUserGeography.MengineConsentFlowUserGeography_Unknown;

    //  Purposes
    // 1 - Store and/or access information on a device
    // 2 - Select basic ads
    // 3 - Create a personalised ads profile
    // 4 - Select personalised ads
    // 5 - Create a personalised content profile
    // 6 - Select personalised content
    // 7 - Measure ad performance
    // 8 - Measure content performance
    // 9 - Apply market research to generate audience insights
    // 10 - Develop and improve products

    //  Special Purposes
    // 1 - Ensure security, prevent fraud, and debug
    // 2 - Technically deliver ads or content

    //  Features
    // 1 - Match and combine offline data sources
    // 2 - Link different devices
    // 3 - Receive and use automatically-sent device characteristics for identification

    // IABUSPrivacy_String format (version 1):
    // Format: <Version><ExplicitNotice><OptOutSale><LSPA>
    // Example: "1YNY"
    //
    // charAt(0) — Version:
    //   Always '1' for the current version of the US Privacy string format.
    //
    // charAt(1) — Explicit Notice:
    //   'Y' — User was given explicit notice about data collection and sale.
    //   'N' — User was not given notice.
    //   '-' — Not applicable or unknown.
    //
    // charAt(2) — OptOutSale:
    //   'Y' — User has opted out of the sale of personal data ("Do Not Sell").
    //   'N' — User has not opted out (sale is allowed).
    //   '-' — Not applicable or unknown.
    //
    // charAt(3) — LSPA (Limited Service Provider Agreement):
    //   'Y' — The publisher is covered under the LSPA for this transaction.
    //   'N' — Not covered.
    //   '-' — Not applicable or unknown.

    public boolean TRANSPARENCYCONSENT_PENDING;
    public int TRANSPARENCYCONSENT_CMPSDKID;
    public int TRANSPARENCYCONSENT_CMPSDKVERSION;
    public int TRANSPARENCYCONSENT_POLICYVERSION;
    public int TRANSPARENCYCONSENT_GDPRAPPLIES;
    public String TRANSPARENCYCONSENT_PUBLISHERCC;
    public int TRANSPARENCYCONSENT_PURPOSEONETREATMENT;
    public int TRANSPARENCYCONSENT_USENONSTANDARDSTACKS;
    public String TRANSPARENCYCONSENT_TCSTRING;
    public String TRANSPARENCYCONSENT_VENDORCONSENTS;
    public String TRANSPARENCYCONSENT_VENDORLEGITIMATEINTERESTS;
    public String TRANSPARENCYCONSENT_PURPOSECONSENTS;
    public String TRANSPARENCYCONSENT_PURPOSELEGITIMATEINTERESTS;
    public String TRANSPARENCYCONSENT_SPECIALFEATURESOPTINS;
    public String TRANSPARENCYCONSENT_PUBLISHERRESTRICTIONS;
    public String TRANSPARENCYCONSENT_PUBLISHERCONSENTS;
    public String TRANSPARENCYCONSENT_PUBLISHERLEGITIMATEINTERESTS;
    public String TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESCONSENTS;
    public String TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESLEGITIMATEINTERESTS;

    public String TRANSPARENCYCONSENT_USPRIVACYSTRING;

    public static void setConsentFlowUserGeography(@NonNull Context context, MengineConsentFlowUserGeography userGeography) {
        if (MengineParamTransparencyConsent.TRANSPARENCYCONSENT_USERGEOGRAPHY == userGeography) {
            return;
        }

        MengineParamTransparencyConsent.TRANSPARENCYCONSENT_USERGEOGRAPHY = userGeography;

        MenginePreferences.setPreferenceEnum("mengine.consent.user_geography", userGeography);
    }

    public static MengineConsentFlowUserGeography getTransparencyconsentUsergeography() {
        return MengineParamTransparencyConsent.TRANSPARENCYCONSENT_USERGEOGRAPHY;
    }

    private boolean isTransparencyConsentPending(@NonNull SharedPreferences preferences) {
        String tcString = preferences.getString("IABTCF_TCString", "");
        String purposeConsents = preferences.getString("IABTCF_PurposeConsents", "");

        if (tcString == null || tcString.isEmpty() == true) {
            return true;
        }

        if (purposeConsents == null || purposeConsents.isEmpty() == true) {
            return true;
        }

        if (preferences.contains("IABTCF_gdprApplies") == false) {
            return true;
        }

        return false;
    }

    public void initFromDefaultSharedPreferences(@NonNull Context context) {
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(context);

        MengineParamTransparencyConsent.TRANSPARENCYCONSENT_USERGEOGRAPHY = MenginePreferences.getPreferenceEnum("mengine.consent.user_geography", MengineConsentFlowUserGeography.class, MengineConsentFlowUserGeography.MengineConsentFlowUserGeography_Unknown);

        TRANSPARENCYCONSENT_PENDING = this.isTransparencyConsentPending(preferences);

        TRANSPARENCYCONSENT_CMPSDKID = preferences.getInt("IABTCF_CmpSdkID", 0);
        TRANSPARENCYCONSENT_CMPSDKVERSION = preferences.getInt("IABTCF_CmpSdkVersion", 0);
        TRANSPARENCYCONSENT_POLICYVERSION = preferences.getInt("IABTCF_PolicyVersion", 0);
        TRANSPARENCYCONSENT_GDPRAPPLIES = preferences.getInt("IABTCF_gdprApplies", 0);
        TRANSPARENCYCONSENT_PUBLISHERCC = preferences.getString("IABTCF_PublisherCC", "AA");
        TRANSPARENCYCONSENT_PURPOSEONETREATMENT = preferences.getInt("IABTCF_PurposeOneTreatment", 0);
        TRANSPARENCYCONSENT_USENONSTANDARDSTACKS = preferences.getInt("IABTCF_UseNonStandardStacks", 0);
        TRANSPARENCYCONSENT_TCSTRING = preferences.getString("IABTCF_TCString", "");
        TRANSPARENCYCONSENT_VENDORCONSENTS = preferences.getString("IABTCF_VendorConsents", "");
        TRANSPARENCYCONSENT_VENDORLEGITIMATEINTERESTS = preferences.getString("IABTCF_VendorLegitimateInterests", "");
        TRANSPARENCYCONSENT_PURPOSECONSENTS = preferences.getString("IABTCF_PurposeConsents", "");
        TRANSPARENCYCONSENT_PURPOSELEGITIMATEINTERESTS = preferences.getString("IABTCF_PurposeLegitimateInterests", "");
        TRANSPARENCYCONSENT_SPECIALFEATURESOPTINS = preferences.getString("IABTCF_SpecialFeaturesOptins", "");
        TRANSPARENCYCONSENT_PUBLISHERRESTRICTIONS = preferences.getString("IABTCF_PublisherRestrictions", "");
        TRANSPARENCYCONSENT_PUBLISHERCONSENTS = preferences.getString("IABTCF_PublisherConsents", "");
        TRANSPARENCYCONSENT_PUBLISHERLEGITIMATEINTERESTS = preferences.getString("IABTCF_PublisherLegitimateInterests", "");
        TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESCONSENTS = preferences.getString("IABTCF_PublisherCustomPurposesConsents", "");
        TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESLEGITIMATEINTERESTS = preferences.getString("IABTCF_PublisherCustomPurposesLegitimateInterests", "");

        TRANSPARENCYCONSENT_USPRIVACYSTRING = preferences.getString("IABUSPrivacy_String", "");
    }

    public boolean isPending() {
        MengineConsentFlowUserGeography geography = MengineParamTransparencyConsent.getTransparencyconsentUsergeography();

        if (geography == MengineConsentFlowUserGeography.MengineConsentFlowUserGeography_NonEEA) {
            return false;
        }

        return TRANSPARENCYCONSENT_PENDING;
    }

    public boolean isEEA() {
        MengineConsentFlowUserGeography geography = MengineParamTransparencyConsent.getTransparencyconsentUsergeography();

        if (geography == MengineConsentFlowUserGeography.MengineConsentFlowUserGeography_NonEEA) {
            return false;
        }

        if (TRANSPARENCYCONSENT_GDPRAPPLIES == 0) {
            return false;
        }

        return true;
    }

    public boolean isCCPADoNotSell() {
        if (TRANSPARENCYCONSENT_USPRIVACYSTRING.length() < 3) {
            return false;
        }

        return TRANSPARENCYCONSENT_USPRIVACYSTRING.charAt(2) == 'Y';
    }

    public boolean getPurposeConsentArgument(int index) {
        if (this.isEEA() == false) {
            return true;
        }

        if (TRANSPARENCYCONSENT_PURPOSECONSENTS.length() <= index) {
            return false;
        }

        if (TRANSPARENCYCONSENT_PURPOSECONSENTS.charAt(index) == '0') {
            return false;
        }

        return true;
    }

    public boolean getPurposeConsentArguments(List<Integer> arguments) {
        if (this.isEEA() == false) {
            return true;
        }

        for (Integer argument : arguments) {
            if (this.getPurposeConsentArgument(argument) == false) {
                return false;
            }
        }

        return true;
    }

    public boolean getConsentAdStorage() {
        if (this.isEEA() == false) {
            return true;
        }

        if (this.getPurposeConsentArgument(0) == false) {
            return false;
        }

        return true;
    }

    public boolean getConsentAnalyticsStorage() {
        if (this.isEEA() == false) {
            return true;
        }

        if (this.getPurposeConsentArgument(6) == false) {
            return false;
        }

        if (this.getPurposeConsentArgument(9) == false) {
            return false;
        }

        return true;
    }

    public boolean getConsentAdPersonalization() {
        if (this.isEEA() == false) {
            return true;
        }

        if (this.getPurposeConsentArgument(2) == false) {
            return false;
        }

        if (this.getPurposeConsentArgument(3) == false) {
            return false;
        }

        return true;
    }

    public boolean getConsentAdUserData() {
        if (this.isEEA() == false) {
            return true;
        }

        if (this.getPurposeConsentArgument(0) == false) {
            return false;
        }

        if (this.getPurposeConsentArgument(6) == false) {
            return false;
        }

        return true;
    }

    public boolean getConsentMeasurement() {
        if (this.isEEA() == false) {
            return true;
        }

        if (this.getPurposeConsentArgument(8) == false) {
            return false;
        }

        return true;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        MengineParamTransparencyConsent dst = (MengineParamTransparencyConsent) o;

        return TRANSPARENCYCONSENT_PENDING == dst.TRANSPARENCYCONSENT_PENDING &&
                TRANSPARENCYCONSENT_CMPSDKID == dst.TRANSPARENCYCONSENT_CMPSDKID &&
                TRANSPARENCYCONSENT_CMPSDKVERSION == dst.TRANSPARENCYCONSENT_CMPSDKVERSION &&
                TRANSPARENCYCONSENT_POLICYVERSION == dst.TRANSPARENCYCONSENT_POLICYVERSION &&
                TRANSPARENCYCONSENT_GDPRAPPLIES == dst.TRANSPARENCYCONSENT_GDPRAPPLIES &&
                TRANSPARENCYCONSENT_PURPOSEONETREATMENT == dst.TRANSPARENCYCONSENT_PURPOSEONETREATMENT &&
                TRANSPARENCYCONSENT_USENONSTANDARDSTACKS == dst.TRANSPARENCYCONSENT_USENONSTANDARDSTACKS &&
                Objects.equals(TRANSPARENCYCONSENT_PUBLISHERCC, dst.TRANSPARENCYCONSENT_PUBLISHERCC) &&
                Objects.equals(TRANSPARENCYCONSENT_TCSTRING, dst.TRANSPARENCYCONSENT_TCSTRING) &&
                Objects.equals(TRANSPARENCYCONSENT_VENDORCONSENTS, dst.TRANSPARENCYCONSENT_VENDORCONSENTS) &&
                Objects.equals(TRANSPARENCYCONSENT_VENDORLEGITIMATEINTERESTS, dst.TRANSPARENCYCONSENT_VENDORLEGITIMATEINTERESTS) &&
                Objects.equals(TRANSPARENCYCONSENT_PURPOSECONSENTS, dst.TRANSPARENCYCONSENT_PURPOSECONSENTS) &&
                Objects.equals(TRANSPARENCYCONSENT_PURPOSELEGITIMATEINTERESTS, dst.TRANSPARENCYCONSENT_PURPOSELEGITIMATEINTERESTS) &&
                Objects.equals(TRANSPARENCYCONSENT_SPECIALFEATURESOPTINS, dst.TRANSPARENCYCONSENT_SPECIALFEATURESOPTINS) &&
                Objects.equals(TRANSPARENCYCONSENT_PUBLISHERRESTRICTIONS, dst.TRANSPARENCYCONSENT_PUBLISHERRESTRICTIONS) &&
                Objects.equals(TRANSPARENCYCONSENT_PUBLISHERCONSENTS, dst.TRANSPARENCYCONSENT_PUBLISHERCONSENTS) &&
                Objects.equals(TRANSPARENCYCONSENT_PUBLISHERLEGITIMATEINTERESTS, dst.TRANSPARENCYCONSENT_PUBLISHERLEGITIMATEINTERESTS) &&
                Objects.equals(TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESCONSENTS, dst.TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESCONSENTS) &&
                Objects.equals(TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESLEGITIMATEINTERESTS, dst.TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESLEGITIMATEINTERESTS) &&
                Objects.equals(TRANSPARENCYCONSENT_USPRIVACYSTRING, dst.TRANSPARENCYCONSENT_USPRIVACYSTRING);
    }

    @Override
    public int hashCode() {
        return Objects.hash(
                TRANSPARENCYCONSENT_PENDING,
                TRANSPARENCYCONSENT_CMPSDKID,
                TRANSPARENCYCONSENT_CMPSDKVERSION,
                TRANSPARENCYCONSENT_POLICYVERSION,
                TRANSPARENCYCONSENT_GDPRAPPLIES,
                TRANSPARENCYCONSENT_PUBLISHERCC,
                TRANSPARENCYCONSENT_PURPOSEONETREATMENT,
                TRANSPARENCYCONSENT_USENONSTANDARDSTACKS,
                TRANSPARENCYCONSENT_TCSTRING,
                TRANSPARENCYCONSENT_VENDORCONSENTS,
                TRANSPARENCYCONSENT_VENDORLEGITIMATEINTERESTS,
                TRANSPARENCYCONSENT_PURPOSECONSENTS,
                TRANSPARENCYCONSENT_PURPOSELEGITIMATEINTERESTS,
                TRANSPARENCYCONSENT_SPECIALFEATURESOPTINS,
                TRANSPARENCYCONSENT_PUBLISHERRESTRICTIONS,
                TRANSPARENCYCONSENT_PUBLISHERCONSENTS,
                TRANSPARENCYCONSENT_PUBLISHERLEGITIMATEINTERESTS,
                TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESCONSENTS,
                TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESLEGITIMATEINTERESTS,
                TRANSPARENCYCONSENT_USPRIVACYSTRING
        );
    }
}