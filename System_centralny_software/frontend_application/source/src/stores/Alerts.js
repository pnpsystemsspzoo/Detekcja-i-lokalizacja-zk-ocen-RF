import { action, computed, decorate, observable, reaction } from "mobx";
// import Probe from "../models/Probe";
import Alert from "../models/Alert";
import Probes from "../stores/Probes";
import Toasts from "./Toasts";

class Alerts extends Probes {
	/**
	 * Current viewed AlertID
	 * @type {Alert}
	 */
	current;

	/**
	 * Number of unread alerts
	 * @type {number}
	 */
	unread = 0;

	/**
	 * Actual alert
	 * @type {Object}
	 */
	currentAlert = {};

	/**
	 * Forms results after success or failure
	 * @type {{add: boolean, firmwareUpload: boolean, configUpload: boolean, firmware: boolean, config: boolean}}
	 */
	result = {
		delete: false
	};

	/**
	 * Form steps
	 * @type {{add: number, firmwareUpload: number, configUpload: number, firmware: number, config: number}}
	 */
	step = {
		delete: 0,
		reset: 0
	};

	/**
	 * Form modal opened state
	 * @type {{add: boolean, firmwareUpload: boolean, configUpload: boolean, firmware: boolean, config: boolean}}
	 */
	modals = {
		isModalOpen: false,
	};

	/**
	 * Toggle modal
	 * @param modal {string} name of modal
	 */
	modalToggle = modal => {
		if (this.modals[modal] !== undefined) {
			const currentState = this.modals[modal];

			if (currentState === false) {
				this.step[modal] = 0;
				this.result[modal] = false;
			}

			this.modals[modal] = !this.modals[modal];
		}
	};

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

	get getCurrent() {
		if (this.registry.has(this.current)) {
			return this.registry.get(this.current);
		} else {
			const alert = new Alert(this, { id: this.current });
			this.registry.set(alert.id, alert);
			return alert;
		}
	}

	closeModal = () => {
		this.modals.isModalOpen = false;
	}

	add = item => {
		let alert = new Alert(this, item);
		this.currentAlert = alert;
		this.registry.set(alert.id, alert);

		this.modals.isModalOpen = true;

		this.unread++;
		this.totalPages = Math.ceil(this.registry.size / this.perPage);
		this.isLoading = false;
	};

	backendDisconnected = (input) => {
		Toasts.add("error", "Błąd", `Backend rozłączony`);
	}
	/**
	 * ALERT frame handler
	 * @param input {Object}
	 */
	handleAlerts = input => {
		if (input.alert_ID === 16385) {
			this.modalToggle('reset');
			Toasts.add("error", "Błąd", `Zresetowanie sondy nie powiodło się`);
			this.isLoading = false;
			this.isprocessing = false;
		} else if (input.alert_ID === 1) {
			this.modalToggle('reset');
			Toasts.add("success", "Sukces", `Pomyślnie zresetowano sondę.`);
			this.isLoading = false;
			this.isprocessing = false;
		}
		else {
			this.add(input);
			this.isLoading = false;
		}
	};

	/**
	 * Set current viewed probe
	 * @param id
	 * @returns {Probe|any}
	 */
	setCurrent = id => {
		this.current = parseInt(id);
		return this.getCurrent;
	};

	/**
	 * Get probe by id
	 * @param id {number}
	 * @returns {Probe}
	 */
	get = id => {
		return this.registry.get(id);
	};

	/**
	 * @override
	 */
	getActualPage = () => {
		this.isLoading = false;
	};

	delete = id => {
		const alertID = id[0];
		if (this.registry.has(alertID)) {
			this.registry.delete(alertID);
			this.totalPages = Math.ceil(this.registry.size / this.perPage);
			this.unread--;
		}
	}


}

decorate(Alerts, {
	modals: observable,
	unread: observable,
	currentAlert: observable,
	processing: observable,
	result: observable,
	current: observable,
	getCurrent: computed,
	closeModal: action,
	columnsList: observable,
	columnsModal: observable
});

export default Alerts;
