import { decorate, observable } from "mobx";
import Model from "../stores/base/Model";

/**
 * Probe object
 * @class
 */
class Signature extends Model {
    /**
     * @type {number}
     */
    id;

    /**
     * @type {number}
     */
    band;

    /**
     * @type {number}
     */
    band_var;

    /**
     * @type {number}
     */
    f0;

    /**
     * @type {number}
     */
    f0_var;

    /**
     * @type {String}
     */
    date;

    /**
     * @type {number}
     */
    double_pulse_duration;

    /**
     * @type {number}
     */
    duty_cycle;

    /**
     * @type {number}
     */
    j;

    /**
     * @type {number}
     */
    j_var;

    /**
     * @type {number}
     */
    heading_last;

    /**
     * @type {number}
     */
    height_first;

    /**
     * @type {number}
     */
    height_last;

    /**
     * @type {number}
     */
    latitude_first;

    /**
     * @type {number}
     */
    latitude_last;

    /**
     * @type {number}
     */
    longitude_first;

    /**
     * @type {number}
     */
    longitude_last;

    /**
     * @type {number}
     */
    pulse_rise_time;

    /**
     * @type {number}
     */
    range_last;

    /**
     * @type {number}
     */
    rise_rate_slope;

    /**
     * @type {number}
     */
    signal_id;

    /**
     * @type {number}
     */
    signal_period;

    /**
     * @type {number}
     */
    signal_type;

    /**
     * @type {number}
     */
    single_pulse_duration;

    /**
     * @type {number}
     */
    speed_max;

    /**
     * @type {String}
     */
    time;

    /**
     * @type {number}
     */
    time_to_return;

    /**
     * @type {number}
     */
    time_var;

    /**
     * `Create new instance of probe
     * @param input {Object}
     */
    constructor(input) {
        super();

        this.id = input.id;
        this.band = input.band;
        this.band_var = input.band_var;
        this.f0 = input.f0;
        this.f0_var = input.f0_var;
        this.date = input.date;
        this.double_pulse_duration = input.double_pulse_duration;
        this.duty_cycle = input.duty_cycle;
        this.j = input.j;
        this.j_var = input.j_var;
        this.heading_last = input.heading_last;
        this.height_first = input.height_first;
        this.height_last = input.height_last;
        this.latitude_first = input.latitude_first;
        this.latitude_last = input.latitude_last;
        this.longitude_first = input.longitude_first;
        this.longitude_last = input.longitude_last;
        this.pulse_rise_time = input.pulse_rise_time;
        this.range_last = input.range_last;
        this.rise_rate_slope = input.rise_rate_slope;
        this.signal_id = input.signal_id;
        this.signal_period = input.signal_period;
        this.signal_type = input.signal_type;
        this.single_pulse_duration = input.single_pulse_duration;
        this.speed_max = input.speed_max;
        this.time = input.time;
        this.time_to_return = input.time_to_return;
        this.time_var = input.time_var;
    }
}

decorate(Signature, {
    id: observable,
    creationDate: observable,
    authorId: observable,
    authorRole: observable,
    authorName: observable,
    creationTime: observable,
});

export default Signature;