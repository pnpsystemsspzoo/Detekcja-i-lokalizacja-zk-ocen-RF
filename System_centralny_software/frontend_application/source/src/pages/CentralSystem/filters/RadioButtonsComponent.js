import { inject, observer } from "mobx-react";
import React from "react";
import { withTranslation } from "react-i18next";


const RadioButtonsComponent = ({ reports, t }) => {
    const { selectedIntervalOption, onIntervalValueChange } = reports;

    return (
        <div className="radio_button_component__container">
            <form>
                <div className="radio_button_component__radio">
                    <label className="radio_container">
                        <input
                            type="radio"
                            value="daily"
                            className="radio_button_component__input"
                            checked={selectedIntervalOption === "daily"}
                            onChange={onIntervalValueChange}
                        />
                        <span className="radio_checkmark"></span>
                        {t('daily')}
                    </label>
                </div>
                <div className="radio_button_component__radio">
                    <label className="radio_container">
                        <input
                            type="radio"
                            value="weekly"
                            className="radio_button_component__input"
                            checked={selectedIntervalOption === "weekly"}
                            onChange={onIntervalValueChange}
                        />
                        <span className="radio_checkmark"></span>
                        {t('weekly')}
                    </label>
                </div>
                <div className="radio_button_component__radio">
                    <label className="radio_container">
                        <input
                            type="radio"
                            value="monthly"
                            className="radio_button_component__input"
                            checked={selectedIntervalOption === "monthly"}
                            onChange={onIntervalValueChange}
                        />
                        <span className="radio_checkmark"></span>
                        {t('monthly')}
                    </label>
                </div>
                <div className="radio_button_component__radio">
                    <label className="radio_container">
                        <input
                            type="radio"
                            value="custom"
                            className="radio_button_component__input"
                            checked={selectedIntervalOption === "custom"}
                            onChange={onIntervalValueChange}
                        />
                        <span className="radio_checkmark"></span>
                        {t('custom_time')}
                    </label>
                </div>
            </form>

        </div>
    );
};

export default inject("reports")(withTranslation()(observer(RadioButtonsComponent)));