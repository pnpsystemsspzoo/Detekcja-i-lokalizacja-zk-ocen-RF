import { inject, observer } from "mobx-react";
import React, { forwardRef } from "react";
import DatePicker from "react-datepicker";

import "react-datepicker/dist/react-datepicker.css";
import { withTranslation } from "react-i18next";

// CSS Modules, react-datepicker-cssmodules.css
// import 'react-datepicker/dist/react-datepicker-cssmodules.css';

const ExampleCustomInput = forwardRef(({ value, onClick }, ref) => (
    <button className="calendar_component__custom_input" onClick={onClick} ref={ref}>
        {value}
    </button>
));

const CalendarComponent = ({ reports, t }) => {
    const { startDate, endDate, setStartDate, setEndDate, setSelectedIntervalOption } = reports;

    return (
        <div className="calendar_component__container">
            <div>
                <span className="calendar_component__span">{t('from')}</span>
                <DatePicker customInput={<ExampleCustomInput />} selected={startDate}
                    dateFormat="dd.MM.yyyy" maxDate={new Date()}
                    onChange={(date) => {
                        setStartDate(date);
                        if (date > endDate) setEndDate(date);
                        setSelectedIntervalOption("custom");
                    }} />
            </div>
            <div>
                <span className="calendar_component__span">{t('to')}</span>
                <DatePicker customInput={<ExampleCustomInput />} selected={endDate}
                    dateFormat="dd.MM.yyyy" minDate={startDate} maxDate={new Date()}
                    onChange={(date) => {
                        setEndDate(date);
                        setSelectedIntervalOption("custom");
                    }} />
            </div>

        </div>
    );
};

export default inject("reports")(withTranslation()(observer(CalendarComponent)));