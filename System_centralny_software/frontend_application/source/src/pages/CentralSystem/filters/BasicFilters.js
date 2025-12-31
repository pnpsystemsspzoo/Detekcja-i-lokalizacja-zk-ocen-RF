import { inject, observer } from "mobx-react";
import React from "react";
import { withTranslation } from "react-i18next";
import CalendarComponent from "./CalendarComponent";
import Checkboxes from "./Checkboxes";
import RadioButtonsComponent from "./RadioButtonsComponent";

const BasicFilters = ({ onIntervalValueChange, t, reports }) => {

    return (
        <div style={{ minHeight: 380 }}>
            <div className="report_box">
                <div className="report_box_header">
                    <div><b>{t('generating_report')}</b></div>
                </div>
            </div>

            <div className="report_box">
                <div className="report_box_header_div">{t('report_date')}</div>

                <div className="central_system_report__buttons">
                    <RadioButtonsComponent onIntervalValueChange={(e) => onIntervalValueChange(e)} />
                    <CalendarComponent />
                </div>
            </div>

            <div className="report_box">
                <div className="report_box_header_div">{t('report_basic_info')}</div>
                <Checkboxes type="basic" />
            </div>

            <div className="report_box">
                <div className="report_box_header_div" style={{ display: 'flex', alignItems: 'center' }}>
                    <div style={{ paddingRight: 10 }}>{t('report_additional_info')}</div>
                    {reports.advancedOptionsCount !== 14 && <button className="btn btn-xs btn-outline-secondary" onClick={reports.selectAllCheckboxes}>{t('select_all')}</button>}
                    {reports.advancedOptionsCount === 14 && <button className="btn btn-xs btn-outline-secondary" onClick={reports.unselectAllCheckboxes}>{t('deselect_all')}</button>}
                </div>
                <Checkboxes type="advanced" />
            </div>
        </div>
    );
};

export default inject("reports")(withTranslation()(observer(BasicFilters)));