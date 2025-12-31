import {decorate, observable} from "mobx";
import Model from "../stores/base/Model";

/**
 * Probe object
 * @class
 */
class Alert extends Model {
	/**
	 * Reference to the registry of probes
	 * @type {Alerts}
	 */
	store;
	
	/**
	 * @type {number}
	 */
	id;
	
	/**
	 * @type {bool}
	 */
	read;
	
	/**
	 * @type {string}
	 */
	message;
	
	/**
	 * @type {String}
	 */
	timestamp;
	
	
	
	
	/**
	 * `Create new instance of probe
	 * @param store {Probe}
	 * @param input {Object}
	 */
	constructor(store, input) {
		super();
		
		this.store = store;
		
		this.id = input.alert_ID;
		
		this.read = false;
		this.message = input.message;
		this.timestamp = input.timestamp;
		
	}
}

decorate(Alert, {
	read: observable,
	message: observable,
	timestamp: observable
});

export default Alert;