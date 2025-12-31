import {computed, decorate, observable} from "mobx";
import Model from "../stores/base/Model";

/**
 * Threat object
 * @class
 */
class Threat extends Model {
	/**
	 *  Interference bandwidth (kHz)
	 *  @type {number}
	 */
	bandwidth;
	/**
	 *  Interference centre frequency (kHz)
	 *  @type {number}
	 */
	f0;
	
	/**
	 * Type of interference
	 * @type {number}
	 */
	type;
	
	/**
	 * @constructs Threat
	 * @param input {Object}
	 */
	constructor(input) {
		super();
		
		this.bandwidth = input.bandwidth;
		this.f0 = input.f0;
		this.type = input.type;
	};
	
	/**
	 * Get human readable type of interference
	 * @returns {string}
	 */
	get getType() {
		if (this.type === 0) return "CHIRP";
		if (this.type === 1) return "CW";
		if (this.type === 2) return "BROADBAND";
		
		return "";
	}
	
	/**
	 * Update data from parsed frame
	 * @param input {Object}
	 */
	update = input => {
		this.lastUpdate = Date.now();
		
		this.bandwidth = input.bandwidth;
		this.f0 = input.f0;
		this.type = input.type;
	};
}

decorate(Threat, {
	bandwidth: observable,
	f0: observable,
	type: observable,
	getType: computed
});

export default Threat;