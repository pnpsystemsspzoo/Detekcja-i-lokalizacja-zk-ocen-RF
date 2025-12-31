import React from "react";
import { inject, observer } from "mobx-react";
import { withRouter } from "react-router-dom";
import PageWrapper from "../../containers/PageWrapper";
import BasicReportTable from "./tables/BasicReportTable";
import BasicFilters from "./filters/BasicFilters";
import AdvancedFilters from "./filters/AdvancedFilters";
import ImageSection from "./ImageSection";
import { withTranslation } from "react-i18next";

class BasicReport extends React.Component {
    componentDidMount() {
        this.handleGenerateReport();
    }

    handleGenerateReport = async () => {
        const { map, reports, api } = this.props;
        const { startDate, endDate, fcMin, fcMax, bandMin, bandMax, periodMin, periodMax, jMin, jMax, currentSelectValue_ID, currentSelectValue_SignalType } = reports;

        map.clearRegistryBasicReport(); // there has to be 0 interferences in report registry
        reports.setInterferenceImageBasic(null);
        reports.filterCheckedOptionsColumn();
        reports.filterCheckedOptionsNames();

        // // request for table with data
        api.report.basic.generate_table({
            startDate, endDate, fcMin, fcMax, bandMin, bandMax, periodMin, periodMax,
            jMin, jMax, currentSelectValue_ID, currentSelectValue_SignalType,
        })
            .then(res => {
                if (res.data.length > 0) {
                    reports.processRow(res.data);
                    reports.renderSelectOptions(res.data);
                    reports.renderSignalTypes(res.data);
                    reports.setLoadingInterferenceImageBasic(true);

                    // request for image screen
                    api.report.basic.generate_screen({})
                        .then(res => {
                            const blob = new Blob([res.data]);
                            const url = URL.createObjectURL(blob);
                            reports.setLoadingInterferenceImageBasic(false);
                            reports.setInterferenceImageBasic(url);
                        })
                        .catch(err => {
                            console.log('err', err)
                        });
                }
                else reports.setTableData([]);
            })
            .catch(err => {
                console.log('err', err)
            });
    }

    generateExtendedReport = (id) => {
        const { api } = this.props;
        api.report.extended.generate_table({ id })
            .then(res => {
                window.open(`${process.env.REACT_APP_FRONTEND}/central-system-extended-report/`, "_blank");
            })
            .catch(err => {
                console.log('err', err)
            });
    }


    render() {
        const { reports, t } = this.props;


        return (
            <PageWrapper title={t('reports_title')}>
                <div>
                    <div className="padding padder">
                        <div className="central_system_report__box">
                            <div className="central_system_report__inner_box">
                                <BasicFilters handleGenerateReport={this.handleGenerateReport} />
                                <AdvancedFilters />

                                <div style={{ display: 'flex', justifyContent: 'center', alignItems: 'center', flexDirection: 'row', marginTop: 10, marginBottom: 10 }}>
                                    <button disabled={reports.loadingInterferenceImage} className="btn btn-s btn-success"
                                        onClick={this.handleGenerateReport}>{t('generate_new_table')}</button>
                                </div>
                            </div>

                            {reports.tableData && <div>
                                <div style={{ marginTop: 50 }}>
                                    <div><b>{t('reports_list_subtitle', { datestart: reports.datestart, dateend: reports.dateend })}</b></div>
                                    <p style={{ fontSize: 12 }}>{t('events_count')} {reports.tableData.length}</p>
                                </div>

                                <div className="table__data">
                                    <BasicReportTable generateExtendedReport={this.generateExtendedReport} />
                                </div>

                                {reports.tableData && reports.tableData.length > 0 &&
                                    <ImageSection
                                        loadingInterferenceImage={reports.loadingInterferenceImage}
                                        interferenceImage={reports.interferenceImage}
                                        tableData={reports.tableData}
                                        options={reports.checkedOptionsNames}
                                        isExtended={false}
                                    />
                                }
                            </div>}
                        </div>
                    </div>
                </div>
            </PageWrapper>
        );
    }
}

export default inject("map", "auth", "reports", "api")(withTranslation()(withRouter(observer(BasicReport))));