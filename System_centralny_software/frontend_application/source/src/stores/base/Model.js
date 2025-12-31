import {decorate, observable} from "mobx";


/**
 * Base model object
 * @class
 * @abstract
 */
class Model {
	/**
	 * @type {boolean}
	 */
	isLoading = true;
	
	/**
	 * @type {boolean}
	 */
	isProcessing = false;
	
	/**
	 * Timestamp of last model update
	 * @type {number}
	 */
	lastUpdate = 0;
}

decorate(Model, {
	isLoading: observable,
	isProcessing: observable,
	lastUpdate: observable
});


export default Model;