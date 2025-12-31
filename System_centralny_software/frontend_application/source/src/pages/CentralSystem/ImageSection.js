import React from "react";
import Loading from "../../components/Loading";
import { inject, observer } from "mobx-react";
import { withTranslation } from "react-i18next";
import i18next from "i18next";

const ImageSection = ({ isExtended, reports, auth, loadingInterferenceImage, interferenceImage, tableData, options, t }) => {
    const { startDate, endDate, options_gb } = reports;

    const generateDownloadPdf = () => {
        if (isExtended === false) {
            const newTableData = tableData.filter(function (row) { return row.generateReport; });
            if (newTableData.length > 0) {
                reports.generateDownloadPdf({
                    startDate, endDate, interferenceImage, tableData: (JSON.stringify(newTableData)),
                    options: i18next.language === "pl" ? JSON.stringify(options) : JSON.stringify(options_gb),
                    isExtended, userId: auth.id, userName: auth.name, userType: auth.type, lang: i18next.language
                });
            }
            else reports.showToastNoDataInReport();
        }
        else {
            reports.generateDownloadPdf({
                startDate, endDate, interferenceImage, tableData: (JSON.stringify(tableData)),
                options: i18next.language === "pl" ? JSON.stringify(options) : JSON.stringify(options_gb),
                isExtended, userId: auth.id, userName: auth.name, userType: auth.type, lang: i18next.language
            });
        }

    }

    return (
        <div>
            <div style={{ marginTop: 50 }}>
                <div><b>{t('reports_image_title', { datestart: reports.datestart, dateend: reports.dateend })}</b></div>
                <p style={{ fontSize: 12 }}>{t('pins_info')}</p>
            </div>

            <div style={{ minHeight: 300, paddingBottom: 10, textAlign: "center" }}>
                {loadingInterferenceImage &&
                    <div style={{ height: 600, minHeight: 600, display: 'flex', alignItems: 'center', justifyContent: 'center' }}>
                        <div>{t('loading_preview')}</div>
                        <Loading show={true} />
                    </div>
                }
                {interferenceImage ? <div style={{ textAlign: "center" }}><img alt="interference" id="interferenceImage" height="800" width="auto" src={interferenceImage} /></div> : null}
            </div>

            {tableData && !loadingInterferenceImage &&
                <div style={{ paddingTop: 10, paddingBottom: 10, width: "100%", textAlign: 'center' }}>
                    <button className="btn btn-s btn-success" onClick={generateDownloadPdf} disabled={reports.isDownloading}>{t('generate_pdf')}</button>
                </div>
            }
        </div>
    );
};

export default inject("reports", "auth")((withTranslation()(observer(ImageSection))));