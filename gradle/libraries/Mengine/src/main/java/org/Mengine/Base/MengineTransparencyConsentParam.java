package org.Mengine.Base;

import android.content.Context;
import android.content.SharedPreferences;

import androidx.annotation.NonNull;
import androidx.preference.PreferenceManager;

public class MengineTransparencyConsentParam {
    public boolean TRANSPARENCYCONSENT_PENDING;
    public int TRANSPARENCYCONSENT_CMPSDKID;
    public int TRANSPARENCYCONSENT_CMPSDKVERSION;
    public int TRANSPARENCYCONSENT_POLICYVERSION;
    public int TRANSPARENCYCONSENT_GDPRAPPLIES;
    public String TRANSPARENCYCONSENT_PUBLISHERCC;
    public int TRANSPARENCYCONSENT_PURPOSEONETREATMENT;
    public int TRANSPARENCYCONSENT_USENONSTANDARTTEXTS;
    public String TRANSPARENCYCONSENT_TCSTRING;
    public String TRANSPARENCYCONSENT_VENDORCONSENTS;
    public String TRANSPARENCYCONSENT_VEMDORLEGITIMATEINTERESTS;
    public String TRANSPARENCYCONSENT_PURPOSECONSENTS;
    public String TRANSPARENCYCONSENT_PURPOSELEGITIMATEINTERESTS;
    public String TRANSPARENCYCONSENT_SPECIALFEATURESOPTINS;
    public String TRANSPARENCYCONSENT_PUBLISHERRESTRICTIONS;
    public String TRANSPARENCYCONSENT_PUBLISHERCONSENTS;
    public String TRANSPARENCYCONSENT_PUBLISHERLEGITIMATEINTERESTS;
    public String TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESCONSENTS;
    public String TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESLEGITIMATEINTERESTS;

    public void initFromDefaultSharedPreferences(@NonNull Context context) {
        SharedPreferences preferences = PreferenceManager.getDefaultSharedPreferences(context);

        TRANSPARENCYCONSENT_PENDING = preferences.contains("IABTCF_gdprApplies") == false;
        TRANSPARENCYCONSENT_CMPSDKID = preferences.getInt("IABTCF_CmpSdkID", 0);
        TRANSPARENCYCONSENT_CMPSDKVERSION = preferences.getInt("IABTCF_CmpSdkVersion", 0);
        TRANSPARENCYCONSENT_POLICYVERSION = preferences.getInt("IABTCF_PolicyVersion", 0);
        TRANSPARENCYCONSENT_GDPRAPPLIES = preferences.getInt("IABTCF_gdprApplies", 0);
        TRANSPARENCYCONSENT_PUBLISHERCC = preferences.getString("IABTCF_PublisherCC", "AA");
        TRANSPARENCYCONSENT_PURPOSEONETREATMENT = preferences.getInt("IABTCF_PurposeOneTreatment", 0);
        TRANSPARENCYCONSENT_USENONSTANDARTTEXTS = preferences.getInt("IABTCF_UseNonStandardStacks", 0);
        TRANSPARENCYCONSENT_TCSTRING = preferences.getString("IABTCF_TCString", "");
        TRANSPARENCYCONSENT_VENDORCONSENTS = preferences.getString("IABTCF_VendorConsents", "");
        TRANSPARENCYCONSENT_VEMDORLEGITIMATEINTERESTS = preferences.getString("IABTCF_VendorLegitimateInterests", "");
        TRANSPARENCYCONSENT_PURPOSECONSENTS = preferences.getString("IABTCF_PurposeConsents", "");
        TRANSPARENCYCONSENT_PURPOSELEGITIMATEINTERESTS = preferences.getString("IABTCF_PurposeLegitimateInterests", "");
        TRANSPARENCYCONSENT_SPECIALFEATURESOPTINS = preferences.getString("IABTCF_SpecialFeaturesOptins", "");
        TRANSPARENCYCONSENT_PUBLISHERRESTRICTIONS = preferences.getString("IABTCF_PublisherRestrictions", "");
        TRANSPARENCYCONSENT_PUBLISHERCONSENTS = preferences.getString("IABTCF_PublisherConsents", "");
        TRANSPARENCYCONSENT_PUBLISHERLEGITIMATEINTERESTS = preferences.getString("IABTCF_PublisherLegitimateInterests", "");
        TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESCONSENTS = preferences.getString("IABTCF_PublisherCustomPurposesConsents", "");
        TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESLEGITIMATEINTERESTS = preferences.getString("IABTCF_PublisherCustomPurposesLegitimateInterests", "");
    }

    public boolean isPending() {
        return TRANSPARENCYCONSENT_PENDING;
    }

    public boolean getPurposeConsentArgument(int index) {
        if (TRANSPARENCYCONSENT_PURPOSECONSENTS.length() <= index) {
            return false;
        }

        if (TRANSPARENCYCONSENT_PURPOSECONSENTS.charAt(index) == '0') {
            return false;
        }

        return true;
    }

    public boolean isEEA() {
        if (TRANSPARENCYCONSENT_GDPRAPPLIES == 0) {
            return false;
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

        //ToDo

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
}