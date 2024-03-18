package org.Mengine.Base;

public class MengineTransparencyConsentParam {
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

    public boolean getPurposeConsentArgument(int index) {
        if (TRANSPARENCYCONSENT_GDPRAPPLIES == 0) {
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

        if (this.getPurposeConsentArgument(1) == false) {
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

        if (this.getPurposeConsentArgument(3) == false) {
            return false;
        }

        if (this.getPurposeConsentArgument(4) == false) {
            return false;
        }

        return true;
    }

    public boolean getConsentAdUserData() {
        if (this.isEEA() == false) {
            return true;
        }

        if (this.getPurposeConsentArgument(1) == false) {
            return false;
        }

        if (this.getPurposeConsentArgument(7) == false) {
            return false;
        }

        return true;
    }
}