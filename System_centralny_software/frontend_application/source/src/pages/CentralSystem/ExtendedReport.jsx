import React from "react";
import { inject, observer } from "mobx-react";
import { withRouter } from "react-router-dom";
import PageWrapper from "../../containers/PageWrapper";
import moment from "moment";
import ExtendedReportTable from "./tables/ExtendedReportTable";
import { signaltypes } from "./tableoptions";
import ImageSection from "./ImageSection";
import { withTranslation } from "react-i18next";

class ExtendedReport extends React.Component {
    componentDidMount() {
        this.handleComponentLoad();
    }

    handleComponentLoad = async () => {
        const { api, reports } = this.props;

        reports.setInterferenceImageExtended(null);
        reports.setLoadingInterferenceImageExtended(true);

        api.report.extended.get_table({})
            .then(res => {
                let newData = res.data.data.map((row) => {
                    row.date = moment(row.date).format("DD-MM-YYYY");
                    row.signal_type = signaltypes[row.signal_type];
                    return row;
                })
                reports.setExtendedTableData(newData);

                // request for image screen
                api.report.extended.generate_screen({})
                    .then(res => {
                        const blob = new Blob([res.data]);
                        const url = URL.createObjectURL(blob);
                        reports.setLoadingInterferenceImageExtended(false);
                        reports.setInterferenceImageExtended(url);
                    })
                    .catch(err => {
                        console.log('err', err)
                    });
            })
            .catch(err => {
                console.log('err', err)
            });
    };
    render() {
        const { reports, t } = this.props;

        return (
            <PageWrapper title={t('reports_title')}>
                <div>
                    <div className="padding padder">
                        <div>
                            <div><p>{t('generating_extended_report')}</p></div>
                            {reports.extendedTableData && <div>

                                <div style={{ marginTop: 50 }}>
                                    <p><b>{t('interference_details')}</b></p>
                                </div>

                                <div className="table__data">
                                    <ExtendedReportTable tableData={reports.extendedTableData} options={reports.options} />
                                </div>

                                {reports.extendedTableData && reports.extendedTableData.length > 0 &&
                                    <ImageSection
                                        loadingInterferenceImage={reports.loadingInterferenceImageExtended}
                                        interferenceImage={reports.interferenceImageExtended}
                                        tableData={reports.extendedTableData}
                                        options={reports.options}
                                        isExtended={true}
                                    />
                                }

                            </div>}
                        </div>
                    </div>

                </div>
            </PageWrapper >
        );
    }
}
export default inject("map", "auth", "reports", "api")(withTranslation()(withRouter(observer(ExtendedReport))));