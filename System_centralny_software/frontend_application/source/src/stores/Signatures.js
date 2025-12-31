import Registry from "./base/Registry";
import { action, computed, decorate, observable, reaction } from "mobx";
import Toasts from "./Toasts";
import Api from "./base/Api";
import Signature from "../models/Signature";

class Signatures extends Registry {
    /**
     * Current viewed signature id
     * @type {Signature}
     */
    current;

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

    /**
     * @override
     */
    columns = [
        {
            label: "ID sygnatury",
            sortable: true,
            sort: "id"
        },
        {
            label: "Data",
            sortable: true,
            sort: "date"
        },
        {
            label: "Czas",
            sortable: true,
            sort: "time"
        },
        {
            label: "Wariancja czasu",
            sortable: true,
            sort: "time_var"
        },
        {
            label: "Długość geograficzna (pierwsze wystąpienie)",
            sortable: true,
            sort: "latitude_first"
        },
        {
            label: "Długość geograficzna (ostatnie wystąpienie)",
            sortable: true,
            sort: "latitude_last"
        },
        {
            label: "Szerokość geograficzna (pierwsze wystąpienie)",
            sortable: true,
            sort: "longitude_first"
        },
        {
            label: "Szerokość geograficzna (ostatnie wystąpienie)",
            sortable: true,
            sort: "longitude_last"
        },
        {
            label: "Wysokość (pierwsze wystąpienie)",
            sortable: true,
            sort: "height_first"
        },
        {
            label: "Wysokość (ostatnie wystąpienie)",
            sortable: true,
            sort: "height_last"
        },
        {
            label: "Wariancja niepewności lokalizacji (ostatnie wystąpienie)",
            sortable: true,
            sort: "range_last"
        },
        {
            label: "Prędkość maksymalna",
            sortable: true,
            sort: "speed_max"
        },
        {
            label: "Kurs (ostatnie wystąpienie)",
            sortable: true,
            sort: "heading_last"
        },
        {
            label: "Okres sygnału",
            sortable: true,
            sort: "signal_period"
        },
        {
            label: "Typ sygnału",
            sortable: true,
            sort: "signal_type"
        },
        {
            label: "Id sygnału",
            sortable: true,
            sort: "signal_id"
        },
        {
            label: "Szybkość narastania (zbocze)",
            sortable: true,
            sort: "pulse_rise_time"
        },
        {
            label: "Czas powrotu",
            sortable: true,
            sort: "time_to_return"
        },
        {
            label: "Czas narastania impulsu",
            sortable: true,
            sort: "rise_rate_slope"
        },
        {
            label: "Czas trwania pojedynczego impulsu",
            sortable: true,
            sort: "single_pulse_duration"
        },
        {
            label: "Czas trwania podwójnego impulsu",
            sortable: true,
            sort: "double_pulse_duration"
        },
        {
            label: "Duty-cycle",
            sortable: true,
            sort: "duty_cycle"
        },
        {
            label: "Częstotliwość środkowa",
            sortable: true,
            sort: "f0"
        },
        {
            label: "Wariancja częstotliwości środkowej",
            sortable: true,
            sort: "f0_var"
        },
        {
            label: "Pasmo",
            sortable: true,
            sort: "band"
        },
        {
            label: "Wariancja pasma",
            sortable: true,
            sort: "band_var"
        },
        {
            label: "Estymowana moc nadajnika",
            sortable: true,
            sort: "j"
        },
        {
            label: "Wariancja estymowanej mocy nadajnika",
            sortable: true,
            sort: "j_var"
        },
    ];

    /**
     * Set current viewed probe
     * @param id
     * @returns {Signature|any}
     */
    setCurrent = id => {
        this.current = parseInt(id);
        return this.getCurrent;
    };

    /**
     * Get current viewed signature
     * @returns {Signature|any}
     */
    get getCurrent() {
        if (this.registry.has(this.current)) {
            return this.registry.get(this.current);
        } else {
            const probe = new Signature(this, { id: this.current });
            this.registry.set(probe.id, probe);
            return probe;
        }
    }

    /**
     * Get signature by id
     * @param id {number}
     * @returns {Signature}
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
        Api.signatures.get()
            .then(res => {
                const { signatures } = res.data;

                signatures.forEach(signature => {
                    const newSignature = new Signature(signature);
                    this.registry.set(newSignature.id, newSignature);
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

}

decorate(Signatures, {
    current: observable,
    getCurrent: computed,
    isDownloading: observable
});

export default Signatures;
