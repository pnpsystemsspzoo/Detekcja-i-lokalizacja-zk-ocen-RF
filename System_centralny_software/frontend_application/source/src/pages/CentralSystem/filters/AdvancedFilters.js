import React from "react";
import RangeInput from "./RangeInput";
import Select from 'react-select'
import { withTranslation } from "react-i18next";
import { inject, observer } from "mobx-react";

const AdvancedFilters = (props) => {
    const { reports, t } = props;
    const { fcMin, setFcMin, fcMax, setFcMax, bandMin, setBandMin, bandMax, setBandMax, periodMin, setPeriodMin, periodMax, setPeriodMax, jMin, setJMin, jMax, setJMax,
        handleSelectOnChangeID, handleSelectOnChangeSignalType, filteringInformation, currentSelectValue_ID, currentSelectValue_SignalType, allSelectOptions_ID, allSelectOptions_SignalType
    } = reports;

    return (
        <div className="report_box">
            <div className="report_box_header_div">{t('search_title')}</div>

            <div style={{ display: 'flex', flexDirection: 'column', marginBottom: 20, alignItems: 'center', justifyContent: 'center' }}>
                {filteringInformation === "id" && <div style={{ height: 20, marginBottom: 10 }}>{t('filtering_by_id_message')}</div>}
                {filteringInformation === "signal_type" && <div style={{ height: 20, marginBottom: 10 }}>{t('filtering_by_signal_type_message')}</div>}
                <div style={{ display: 'flex', flexDirection: 'row', marginBottom: 10, alignItems: 'center', justifyContent: 'center' }}>
                    <Select
                        value={currentSelectValue_ID.value === -1 ? { value: -1, label: t('select_id') } : currentSelectValue_ID}
                        className="custom_select"
                        options={allSelectOptions_ID}
                        onChange={(e) => handleSelectOnChangeID(e)}
                        isClearable={true}
                    />
                    <Select
                        value={currentSelectValue_SignalType.value === -1 ? { value: -1, label: t('select_signal_type') } : currentSelectValue_SignalType}
                        className="custom_select"
                        options={allSelectOptions_SignalType}
                        onChange={(e) => handleSelectOnChangeSignalType(e)}
                        isClearable={true}
                    />
                </div>
            </div>

            <div style={{ display: 'flex', flexDirection: 'row', justifyContent: 'center', flexWrap: 'wrap' }}>
                <div style={{ display: 'flex', flexDirection: 'row', justifyContent: 'center' }}>
                    <div className="mini_box">
                        <div className="report__inputs_title">{t('f0')}</div>
                        <div className="report__inputs_title"> ({t('from')} - {t('to')})</div>
                        <div className="report__inputs_flex">
                            <RangeInput placeholder={"fcmin"} value={fcMin} handleOnChange={(e) => setFcMin(e.target.value)} />
                            <RangeInput placeholder={"fcmax"} value={fcMax} handleOnChange={(e) => setFcMax(e.target.value)} />
                        </div>
                    </div>

                    <div className="report__line"></div>

                    <div className="mini_box">
                        <div className="report__inputs_title">{t('band')}</div>
                        <div className="report__inputs_title"> ({t('from')} - {t('to')})</div>
                        <div className="report__inputs_flex">
                            <RangeInput placeholder={"kHz"} value={bandMin} handleOnChange={(e) => setBandMin(e.target.value)} />
                            <RangeInput placeholder={"kHz"} value={bandMax} handleOnChange={(e) => setBandMax(e.target.value)} />
                        </div>
                    </div>

                    <div className="report__line"></div>
                </div>


                <div style={{ display: 'flex', flexDirection: 'row', justifyContent: 'center' }}>
                    <div className="mini_box">
                        <div className="report__inputs_title">{t('signal_period')}</div>
                        <div className="report__inputs_title"> ({t('from')} - {t('to')})</div>
                        <div className="report__inputs_flex">
                            <RangeInput placeholder={"us"} value={periodMin} handleOnChange={(e) => setPeriodMin(e.target.value)} />
                            <RangeInput placeholder={"us"} value={periodMax} handleOnChange={(e) => setPeriodMax(e.target.value)} />
                        </div>
                    </div>

                    <div className="report__line"></div>

                    <div className="mini_box">
                        <div className="report__inputs_title">{t('j')}</div>
                        <div className="report__inputs_title"> ({t('from')} - {t('to')})</div>
                        <div className="report__inputs_flex">
                            <RangeInput placeholder={"dBm"} value={jMin} handleOnChange={(e) => setJMin(e.target.value)} />
                            <RangeInput placeholder={"dBm"} value={jMax} handleOnChange={(e) => setJMax(e.target.value)} />
                        </div>
                    </div>
                </div>
            </div>
        </div>
    );
};

export default inject("reports")(withTranslation()(observer(AdvancedFilters)));