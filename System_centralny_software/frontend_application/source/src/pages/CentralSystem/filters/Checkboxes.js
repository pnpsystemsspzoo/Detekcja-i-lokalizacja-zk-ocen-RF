import React from "react";
import { inject, observer } from "mobx-react";
import { withRouter } from "react-router-dom";
import { withTranslation } from "react-i18next";

const Checkboxes = ({ type, reports, t }) => {
    return (
        <div style={{ display: 'flex', flexWrap: 'wrap', flexDirection: 'row', }}>

            {reports.options.map((option) => {
                if (option.type === type) {
                    return (
                        <div className="option__report" key={option.id}>
                            <input name={option.column_name} id={option.column_name} type="checkbox" disabled={option.disabled} checked={option.checked}
                                onChange={() => reports.handleCheckboxChecked(option.id)} /><label htmlFor={option.column_name}>{t(option.column_name)}</label>
                        </div>
                    )
                }
                return null;
            })}

        </div>
    );
}
export default inject("reports")(withTranslation()(withRouter(observer(Checkboxes))));