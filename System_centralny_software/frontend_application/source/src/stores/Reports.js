import Registry from "./base/Registry";
import { action, computed, decorate, observable, reaction } from "mobx";
import Toasts from "./Toasts";
import Api from "./base/Api";
import Report from "../models/Report";
import { t } from "i18next";
import moment from 'moment';
import { signaltypes } from "../pages/CentralSystem/tableoptions";
var FileSaver = require('file-saver');

class Reports extends Registry {
    /**
     * Current viewed report id
     * @type {Report}
     */
    current;

    /**
    * Is downloading report button loading spinner
    * @type {boolean}
    */
    isDownloading;

    fcMin = '';
    fcMax = '';
    bandMin = '';
    bandMax = '';
    periodMin = '';
    periodMax = '';
    jMin = '';
    jMax = '';

    setFcMin = (value) => this.fcMin = value;
    setFcMax = (value) => this.fcMax = value;
    setBandMin = (value) => this.bandMin = value;
    setBandMax = (value) => this.bandMax = value;
    setPeriodMin = (value) => this.periodMin = value;
    setPeriodMax = (value) => this.periodMax = value;
    setJMin = (value) => this.jMin = value;
    setJMax = (value) => this.jMax = value;

    tableData = null;
    setTableData = (data) => this.tableData = data;

    extendedTableData = null;
    setExtendedTableData = (data) => this.extendedTableData = data;

    interferenceImage = null;
    setInterferenceImageBasic = (url) => this.interferenceImage = url;

    interferenceImageExtended = null;
    setInterferenceImageExtended = (url) => this.interferenceImageExtended = url;

    loadingInterferenceImage = false;
    setLoadingInterferenceImageBasic = (value) => this.loadingInterferenceImage = value;

    loadingInterferenceImageExtended = false;
    setLoadingInterferenceImageExtended = (value) => this.loadingInterferenceImageExtended = value;

    advancedOptionsCount = 0;

    // checked options columns
    checkedOptionsColumns = [];
    filterCheckedOptionsColumn = () => {
        // filter only checked options (eg. time_var) for report
        var checkedOptionsColumnsTmp = this.options.reduce((checkedOptionsColumnsTmp, item) => {
            if (item.checked === true) checkedOptionsColumnsTmp.push(item.column_name)
            return checkedOptionsColumnsTmp;
        }, [])
        this.checkedOptionsColumns = checkedOptionsColumnsTmp;
    }

    checkedOptionsNames = [];
    filterCheckedOptionsNames = () => {
        // filter only checked options names(eg.Wariancja czasu) for report
        var checkedOptionsNamesTmp = this.options.reduce((checkedOptionsNamesTmp, item) => {
            // item.name = t(item.column_name);
            if (item.checked === true) checkedOptionsNamesTmp.push(item)
            return checkedOptionsNamesTmp
        }, [])
        this.checkedOptionsNames = checkedOptionsNamesTmp;
    }

    // interval
    selectedIntervalOption = "weekly";
    startDate = new Date(Date.now() - 7 * 24 * 60 * 60 * 1000); // date 7 days ago
    // startDate = new Date(Date.now() - 90 * 24 * 60 * 60 * 1000); // date 90 days ago
    endDate = new Date();
    setSelectedIntervalOption = (interval) => this.selectedIntervalOption = interval;
    onIntervalValueChange = (e) => {
        this.clearDataBeforeRequest();

        this.setSelectedIntervalOption(e.target.value);
        if (e.target.value === "daily") this.setStartDate(new Date());
        if (e.target.value === "weekly") this.setStartDate(new Date(Date.now() - 7 * 24 * 60 * 60 * 1000));
        if (e.target.value === "monthly") this.setStartDate(new Date(Date.now() - 30 * 24 * 60 * 60 * 1000));
        this.setEndDate(new Date());
        this.setTableData(null);
        this.setInterferenceImageBasic(null);
    }

    filteringInformation = "";

    datestart = `${moment(this.startDate).format("DD-MM-YYYY")}`;
    dateend = `${moment(this.endDate).format("DD-MM-YYYY")}`;
    setStartDate = (date) => this.startDate = date;
    setEndDate = (date) => this.endDate = date;

    // select values
    currentSelectValue_ID = { value: -1, label: t('select_id') };
    handleSelectOnChangeID = (e) => {
        this.currentSelectValue_ID = e;
        this.currentSelectValue_SignalType = { value: -1, label: "select_signal_type" };
        this.filteringInformation = "id";
        if (e === null) {
            this.filteringInformation = "";
            this.currentSelectValue_ID = { value: -1, label: t('select_id') };
        }
    }

    currentSelectValue_SignalType = { value: -1, label: "select_signal_type" };
    handleSelectOnChangeSignalType = (e) => {
        this.currentSelectValue_ID = { value: -1, label: t('select_id') };
        this.currentSelectValue_SignalType = e;
        this.filteringInformation = "signal_type";
        if (e === null) {
            this.filteringInformation = "";
            this.currentSelectValue_SignalType = { value: -1, label: "select_signal_type" };
        }
    }

    // Selects - all options

    allSelectOptions_ID = [];
    renderSelectOptions = (data) => {
        // data for select options - add value and label properties
        let select_options = data.map((row, index) => {
            let newRow = {};
            newRow.value = row.id;
            newRow.label = `ID: ${row.id}`;
            return newRow;
        })
        this.allSelectOptions_ID = select_options;
    }

    allSelectOptions_SignalType = [];
    renderSignalTypes = (data) => {
        // reduce table data by signal type 
        let signalTypesArray = [];
        data.forEach((row, index) => {
            // check if value exists in array already
            let newRow = {};

            let indexOfSignalInArray = signaltypes.indexOf(row.signal_type);
            if (indexOfSignalInArray !== -1) {
                newRow.value = indexOfSignalInArray;
                newRow.label = signaltypes[indexOfSignalInArray];

                // check if this signal type is already in the array, if not - add it, if so - skip it
                const isInArray = signalTypesArray.find(element => element.label === signaltypes[indexOfSignalInArray]);
                if (!isInArray) signalTypesArray.push(newRow);
            }

        })
        this.allSelectOptions_SignalType = signalTypesArray;
    }

    clearDataBeforeRequest = () => {
        // clear select values
        this.allSelectOptions_ID = [];
        this.allSelectOptions_SignalType = [];

        this.currentSelectValue_ID = { value: -1, label: t('select_id') };
        this.currentSelectValue_SignalType = { value: -1, label: "select_signal_type" };
    }

    processRow = (filteredData) => {
        // add generateReport property to show "More" button, add _id, process date to human-readable value
        filteredData = filteredData.map((row, index) => {
            row.generateReport = true;
            row._id = index;
            row.date = moment(row.date).format("DD-MM-YYYY");
            row.signal_type = signaltypes[row.signal_type];
            return row;
        })
        this.setTableData(filteredData);
    }

    // checkboxes handlers
    handleCheckboxChecked = (id) => {
        let newOptions = this.options.map(obj => (obj.id === id ? Object.assign(obj, { checked: !this.options[id].checked }) : obj));
        this.options = newOptions;

        let advancedOptionsCountTmp = this.advancedOptionsCount;
        if (newOptions[id].checked) advancedOptionsCountTmp++;
        if (!newOptions[id].checked) advancedOptionsCountTmp--;
        this.advancedOptionsCount = advancedOptionsCountTmp;
    }

    selectAllCheckboxes = () => {
        let newOptions = this.options.map(obj => (obj.type === "advanced" ? Object.assign(obj, { checked: true }) : obj));
        this.options = newOptions;
        this.advancedOptionsCount = 14;
    }

    unselectAllCheckboxes = () => {
        let newOptions = this.options.map(obj => (obj.type === "advanced" ? Object.assign(obj, { checked: false }) : obj));
        this.options = newOptions;
        this.advancedOptionsCount = 0;
    }

    options = [
        { id: 0, type: 'basic', className: "td_short", name: 'ID', checked: true, disabled: true, column_name: 'id' },
        { id: 1, type: 'basic', className: "td_middle", name: 'Data', checked: true, disabled: true, column_name: 'date' },
        { id: 2, type: 'basic', className: "td_short", name: 'Czas', checked: true, disabled: true, column_name: 'time' },
        { id: 3, type: 'basic', className: "td_middle", name: 'Wariancja czasu [s2]', checked: true, disabled: true, column_name: 'time_var' },
        { id: 4, type: 'basic', className: "td_long", name: 'Długość geografczna (pierwsze wystąpienie) [stopnie]', checked: true, disabled: true, column_name: 'longitude_first' },
        { id: 5, type: 'basic', className: "td_long", name: 'Długość geografczna (ostatnie wystąpienie) [stopnie]', checked: true, disabled: true, column_name: 'longitude_last' },
        { id: 6, type: 'basic', className: "td_long", name: 'Szerokość geografczna (pierwsze wystąpienie) [stopnie]', checked: true, disabled: true, column_name: 'latitude_first' },
        { id: 7, type: 'basic', className: "td_long", name: 'Szerokość geografczna (ostatnie wystąpienie) [stopnie]', checked: true, disabled: true, column_name: 'latitude_last' },
        { id: 8, type: 'basic', className: "td_long", name: 'Wysokość (pierwsze wystąpienie) [m]', checked: true, disabled: true, column_name: 'height_first' },
        { id: 9, type: 'basic', className: "td_long", name: 'Wysokość (ostatnie wystąpienie) [m]', checked: true, disabled: true, column_name: 'height_last' },
        { id: 10, type: 'basic', className: "td_long", name: 'Wariancja niepewności lokalizacji (ostatnie wystąpienie) [m^2]', checked: true, disabled: true, column_name: 'range_last' },
        { id: 11, type: 'basic', className: "td_middle", name: 'Prędkość maksymalna', checked: true, disabled: true, column_name: 'speed_max' },
        { id: 12, type: 'basic', className: "td_long", name: 'Kurs (ostatnie wystąpienie) [stopnie]', checked: true, disabled: true, column_name: 'heading_last' },

        { id: 13, type: 'advanced', className: "td_middle", name: 'Okres sygnału [ms]', checked: false, disabled: false, column_name: 'signal_period' },
        { id: 14, type: 'advanced', className: "td_long", name: 'Typ sygnału', checked: false, disabled: false, column_name: 'signal_type' },
        { id: 15, type: 'advanced', className: "td_long", name: 'Szybkość narastania (zbocze) [kHz/µs]', checked: false, disabled: false, column_name: 'rise_rate_slope' },
        { id: 16, type: 'advanced', className: "td_middle", name: 'Czas powrotu [µs]', checked: false, disabled: false, column_name: 'time_to_return' },
        { id: 17, type: 'advanced', className: "td_middle", name: 'Czas narastania impulsu [µs]', checked: false, disabled: false, column_name: 'pulse_rise_time' },
        { id: 18, type: 'advanced', className: "td_long", name: 'Czas trwania pojedynczego impulsu [µs]', checked: false, disabled: false, column_name: 'single_pulse_duration' },
        { id: 19, type: 'advanced', className: "td_long", name: 'Czas trwania podwójnego impulsu [µs]', checked: false, disabled: false, column_name: 'double_pulse_duration' },
        { id: 20, type: 'advanced', className: "td_middle", name: 'Duty-cycle [%]', checked: false, disabled: false, column_name: 'duty_cycle' },
        { id: 21, type: 'advanced', className: "td_middle", name: 'Częstotliwość środkowa [MHz]', checked: false, disabled: false, column_name: 'f0' },
        { id: 22, type: 'advanced', className: "td_long", name: 'Wariancja częstotliwości środkowej [MHz^2]', checked: false, disabled: false, column_name: 'f0_var' },
        { id: 23, type: 'advanced', className: "td_short", name: 'Pasmo [kHz]', checked: false, disabled: false, column_name: 'band' },
        { id: 24, type: 'advanced', className: "td_middle", name: 'Wariancja pasma [kHz^2]', checked: false, disabled: false, column_name: 'band_var' },
        { id: 25, type: 'advanced', className: "td_middle", name: 'Estymowana moc nadajnika [dBm]', checked: false, disabled: false, column_name: 'j' },
        { id: 26, type: 'advanced', className: "td_long", name: 'Wariancja estymowanej mocy nadajnika [dBm^2]', checked: false, disabled: false, column_name: 'j_var' },
    ]

    options_gb = [
        { id: 0, type: 'basic', className: "td_short", name: 'ID', checked: true, disabled: true, column_name: 'id' },
        { id: 1, type: 'basic', className: "td_middle", name: 'Date', checked: true, disabled: true, column_name: 'date' },
        { id: 2, type: 'basic', className: "td_short", name: 'Time', checked: true, disabled: true, column_name: 'time' },
        { id: 3, type: 'basic', className: "td_middle", name: 'Time variance [s2]', checked: true, disabled: true, column_name: 'time_var' },
        { id: 4, type: 'basic', className: "td_long", name: 'Longitude (first occurrence) [degrees]', checked: true, disabled: true, column_name: 'longitude_first' },
        { id: 5, type: 'basic', className: "td_long", name: 'Longitude (last occurrence) [degrees]', checked: true, disabled: true, column_name: 'longitude_last' },
        { id: 6, type: 'basic', className: "td_long", name: 'Latitude (first occurrence) [degrees]', checked: true, disabled: true, column_name: 'latitude_first' },
        { id: 7, type: 'basic', className: "td_long", name: 'Latitude (last occurrence) [degrees]', checked: true, disabled: true, column_name: 'latitude_last' },
        { id: 8, type: 'basic', className: "td_long", name: 'Height (first occurrence) [m]', checked: true, disabled: true, column_name: 'height_first' },
        { id: 9, type: 'basic', className: "td_long", name: 'Height (last occurrence) [m]', checked: true, disabled: true, column_name: 'height_last' },
        { id: 10, type: 'basic', className: "td_long", name: 'Location variance (last occurrence) [m^2]', checked: true, disabled: true, column_name: 'range_last' },
        { id: 11, type: 'basic', className: "td_middle", name: 'Speed max', checked: true, disabled: true, column_name: 'speed_max' },
        { id: 12, type: 'basic', className: "td_long", name: 'Heading (last occurrence) [degrees]', checked: true, disabled: true, column_name: 'heading_last' },

        { id: 13, type: 'advanced', className: "td_middle", name: 'Signal period [ms]', checked: false, disabled: false, column_name: 'signal_period' },
        { id: 14, type: 'advanced', className: "td_long", name: 'Signal type', checked: false, disabled: false, column_name: 'signal_type' },
        { id: 15, type: 'advanced', className: "td_long", name: 'Rise rate (slope) [kHz/µs]', checked: false, disabled: false, column_name: 'rise_rate_slope' },
        { id: 16, type: 'advanced', className: "td_middle", name: 'Return time [µs]', checked: false, disabled: false, column_name: 'time_to_return' },
        { id: 17, type: 'advanced', className: "td_middle", name: 'Pulse rise time [µs]', checked: false, disabled: false, column_name: 'pulse_rise_time' },
        { id: 18, type: 'advanced', className: "td_long", name: 'Single pulse duration [µs]', checked: false, disabled: false, column_name: 'single_pulse_duration' },
        { id: 19, type: 'advanced', className: "td_long", name: 'Double pulse duration [µs]', checked: false, disabled: false, column_name: 'double_pulse_duration' },
        { id: 20, type: 'advanced', className: "td_middle", name: 'Duty-cycle [%]', checked: false, disabled: false, column_name: 'duty_cycle' },
        { id: 21, type: 'advanced', className: "td_middle", name: 'Center frequency [MHz]', checked: false, disabled: false, column_name: 'f0' },
        { id: 22, type: 'advanced', className: "td_long", name: 'Variance of the center frequency [MHz^2]', checked: false, disabled: false, column_name: 'f0_var' },
        { id: 23, type: 'advanced', className: "td_short", name: 'Band [kHz]', checked: false, disabled: false, column_name: 'band' },
        { id: 24, type: 'advanced', className: "td_middle", name: 'Band variance [kHz^2]', checked: false, disabled: false, column_name: 'band_var' },
        { id: 25, type: 'advanced', className: "td_middle", name: 'Estimated transmitter power [dBm]', checked: false, disabled: false, column_name: 'j' },
        { id: 26, type: 'advanced', className: "td_long", name: 'Variance of the estimated transmitter power [dBm^2]', checked: false, disabled: false, column_name: 'j_var' },
    ]


    /**
     * Create new probes store
     */
    constructor() {
        super();

        reaction(
            () => this.current,
            current => {
                if (current) {
                    this.getCurrent.isLoading = false;
                } else {
                    action(() => (this.current = null));
                }
            }
        );
    }

    isSelectedEvent = () => {
        return this.selected.size === this.items.length;
    }

    onSelectEvent = (event) => {
        this.columns.select = true;
        if (this.selected.size === this.items.length) {
            this.clearSelected();
        }
        else {
            const checked = event.target.checked;
            const selected = observable.map();

            checked && this.items.forEach(user => selected.set(user.id, user));
            this.selected = selected;

        }
    }

    selectReportRowToBeInPdfReport = (id) => {
        let tableDataTmp = this.tableData.map((row, index) => {
            if (row.id === id) row.generateReport = !row.generateReport;
            return row;
        })
        this.setTableData(tableDataTmp);
    }

    showToastNoDataInReport = () => {
        Toasts.add("error", "Błąd", t('no_selected_reports'));
    }
    /**
     * Set current viewed probe
     * @param id
     */
    setCurrent = id => {
        this.current = parseInt(id);
        return this.getCurrent;
    }

    /**
     * Get current viewed report
     * @returns {Report|any}
     */
    get getCurrent() {
        if (this.registry.has(this.current)) {
            return this.registry.get(this.current);
        } else {
            const report = new Report(this, { id: this.current });
            this.registry.set(report.id, report);
            return report;
        }
    }

    /**
     * Get report by id
     * @param id {number}
     */
    get = id => {
        return this.registry.get(id);
    };


    /**
     * @override
     * @returns {Q.Promise<any> | *}
     */
    getAll = () => {
        this.isLoading = true;
        Api.reports.get()
            .then(res => {
                const { reports } = res.data;
                reports.forEach(report => {
                    const newReport = new Report(report);
                    this.registry.set(newReport.id, newReport);
                });

                this.totalPages = Math.ceil(this.registry.size / this.perPage);
            })
            .catch(err => {
                console.log('err', err)
                Toasts.add("error", "Błąd", "Brak raportów");
            })
            .finally(() => {
                this.isLoading = false;
            });
    };

    /**
     * @override
     */
    getActualPage = () => {
        this.isLoading = false;
    };

    /**
     * Select report for bulk action
     * @param event {Event}
     */
    select = report => {
        const checked = this.selected.has(report.id);
        checked ? this.selected.delete(report.id) : this.selected.set(report.id, report);
    };

    /**
     * Clear selections on list
     */
    clearSelected = () => {
        this.selected = observable.map();
    };

    download = () => {
        const selectedReportsFilenames = [];
        const selectedReports = [...this.selected.values()];
        selectedReports.forEach((report) => {
            selectedReportsFilenames.push(report.filename);
        })

        Api.report.download({ filenamesArray: JSON.stringify(selectedReportsFilenames) })
            .then(res => {
                var blob = new Blob([res.data], { type: "application/octet-stream;charset=utf-8" });
                FileSaver.saveAs(blob, `reports.zip`);
            })
            .catch(err => {
                console.log('err', err)
                Toasts.add("error", "Błąd", "Nie udało się pobrać raportów");
            })
    }

    generateDownloadPdf = (data) => {
        this.isDownloading = true;
        Api.report.generate_and_download(data)
            .then(async res => {
                var blob = new Blob([res.data], { type: "application/octet-stream;charset=utf-8" });
                FileSaver.saveAs(blob, 'interference_report.pdf');
                this.isDownloading = false;
            })
            .catch(err => {
                console.log('err', err)
                Toasts.add("error", "Błąd", "Nie udało się wygenerować i pobrać raportu");
                this.isDownloading = false;
            })
    }
}

decorate(Reports, {
    current: observable,
    getCurrent: computed,
    isDownloading: observable,
    options: observable,
    advancedOptionsCount: observable,
    startDate: observable,
    endDate: observable,
    tableData: observable,
    checkedOptionsNames: observable,
    checkedOptionsColumns: observable,
    loadingInterferenceImage: observable,
    interferenceImage: observable,
    extendedTableData: observable,
    loadingInterferenceImageExtended: observable,
    interferenceImageExtended: observable,
    currentSelectValue_ID: observable,
    allSelectOptions_ID: observable,
    filteringInformation: observable,
    selectedIntervalOption: observable,
    options_gb: observable,
});

export default Reports;
