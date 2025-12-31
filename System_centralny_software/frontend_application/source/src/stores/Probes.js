import Registry from "./base/Registry";
import { action, computed, decorate, observable, reaction } from "mobx";
import MapStore from "./Map";
import Probe from "../models/Probe";
import Socket from "./Socket";
import { SOCK, STATUS } from "../utils/Constants";
import Encoder from "./Encoder";
import Toasts from "./Toasts";

class Probes extends Registry {
	/**
	 * Current viewed probeID
	 * @type {Probe}
	 */
	current;

	/**
	 * Forms processing states
	 * @type {{add: boolean, firmwareUpload: boolean, configUpload: boolean, firmware: boolean, config: boolean}}
	 */
	processing = {
		add: false,
		delete: false,
		reset: false,
		firmware: false,
		firmwareUpload: false,
		config: false,
		configUpload: false,
		isFiltering: false,
		disable: false,
	};

	/**
	 * Forms results after success or failure
	 * @type {{add: boolean, firmwareUpload: boolean, configUpload: boolean, firmware: boolean, config: boolean}}
	 */
	result = {
		add: false,
		delete: false,
		reset: false,
		firmware: false,
		firmwareUpload: false,
		config: false,
		configUpload: false,
		disable: false,
	};

	/**
	 * Form steps
	 * @type {{add: number, firmwareUpload: number, configUpload: number, firmware: number, config: number}}
	 */
	step = {
		add: 0,
		delete: 0,
		reset: 0,
		firmware: 0,
		firmwareUpload: 0,
		config: 0,
		configUpload: 0,
		disable: 0,
	};

	/**
	 * Form modal opened state
	 * @type {{add: boolean, firmwareUpload: boolean, configUpload: boolean, firmware: boolean, config: boolean}}
	 */
	modals = {
		add: false,
		delete: false,
		reset: false,
		firmware: false,
		config: false,
		firmwareUpload: false,
		configUpload: false,
		disable: false,
	};

	logs = '';
	isFilteringOpen = false;

	toggleFiltering = () => {
		this.isFilteringOpen = !this.isFilteringOpen;
	}

	uploadFileResponse = (input) => {
		console.log('uploaded file - backend response', input)
	}
	/**
	 * Toggle modal
	 * @param modal {string} name of modal
	 */
	modalToggle = (modal) => {
		if (this.modals[modal] !== undefined) {
			this.modals[modal] = !this.modals[modal];
		}

	};

	onSelectEvent = (event) => {
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

	isSelectedEvent = () => {
		return this.selected.size === this.items.length;
	}


	/**
	 * @override
	 */
	filters = {
		id: {
			value: null,
			function: probe => {
				return probe.id === this.filters.id.value;
			}
		},
		mac: {
			value: null,
			function: probe => {
				return probe.macAddress.toLowerCase().indexOf(this.filters.mac.value.toLowerCase()) >= 0;
			}
		},
		ip: {
			value: null,
			function: probe => {
				return probe.ipAddress.indexOf(this.filters.ip.value) >= 0;
			}
		},
		card0: {
			value: null,
			function: probe => {
				this.isFiltering = true;
				return (
					probe.cards[0].cardName.includes(this.filters.card0.value)
				)
			}
		},
		card1: {
			value: null,
			function: probe => {
				return (
					probe.cards[1].cardName.includes(this.filters.card1.value)
				)
			}
		},
		card2: {
			value: null,
			function: probe => {
				return (
					probe.cards[2].cardName.includes(this.filters.card2.value)
				)
			}
		},
		card3: {
			value: null,
			function: probe => {
				return (
					probe.cards[3].cardName.includes(this.filters.card3.value)
				)
			}
		},
		expirationDateStart: {
			value: null,
			function: probe => {
				return probe.expirationDate >= this.filters.expirationDateStart.value && probe.expirationDate <= this.filters.expirationDateEnd.value;
			}
		},

		expirationDateEnd: {
			value: null,
			function: probe => {
				return probe.expirationDate >= this.filters.expirationDateStart.value && probe.expirationDate <= this.filters.expirationDateEnd.value;
			}
		},

		creationDateStart: {
			value: null,
			function: probe => {
				return probe.creationDate >= this.filters.creationDateStart.value && probe.creationDate <= this.filters.creationDateEnd.value;
			}
		},

		creationDateEnd: {
			value: null,
			function: probe => {
				return probe.creationDate >= this.filters.creationDateStart.value && probe.creationDate <= this.filters.creationDateEnd.value;
			}
		},
		// hardwareHealth: {
		// 	value: null,
		// 	function: probe => {
		// 		return probe.hardwareHealth === parseInt(this.filters.hardwareHealth.value);
		// 	}
		// },

	};

	/**
	 * Create new probes store
	 */
	constructor(mapStore) {
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
		// add map store instance to run functions on this instance of the store (interferenceInfo)
		this.mapStore = mapStore;
	}

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
	 * Get current viewed probe
	 * @returns {Probe|any}
	 */
	get getCurrent() {
		if (this.registry.has(this.current)) {
			return this.registry.get(this.current);
		} else {
			const probe = new Probe(this, { id: this.current });
			this.registry.set(probe.id, probe);
			return probe;
		}
	}

	/**
	 * Create new probes registry from DEVICE_LIST frame
	 * @param devices {Object[]}
	 */
	add = devices => {
		devices.forEach(item => {
			let probe = this.registry.get(item.id);

			if (probe) {
				probe.update(item, "list");
			} else {
				probe = new Probe(this, item);
				probe.createMarker();
			}

			this.registry.set(probe.id, probe);
			this.totalPages = Math.ceil(this.registry.size / this.perPage);
		});

		this.loadMarkers();
		this.isLoading = false;
	};

	addDevice = (device) => {
		let probe = this.registry.get(device.id);

		if (probe) {
			probe.update(device, "list");
		} else {
			probe = new Probe(this, device);
			probe.createMarker();
		}

		this.registry.set(probe.id, probe);
		this.totalPages = Math.ceil(this.registry.size / this.perPage);

		this.isLoading = false;
		this.loadMarkers();
	}

	handleInterferenceInfoDevices = (input) => {
		this.mapStore.handleInterferenceInfo(input);

		input.devicesId.forEach((device) => {
			let probe = this.registry.get(device.id);
			if (probe) probe.updateMarkerDetected();
		})
	}

	clearDetectedProbes = () => {
		this.registry._data.forEach((d) => {
			let probe = this.registry.get(d.value.id);
			if (probe) probe.updateMarkerNotDetected();
		})
	}
	/**
	 * Append probe to registry
	 * @param probe {Probe}
	 */
	append = probe => {
		probe = new Probe(this, probe);
		this.registry.set(probe.id, probe);
		this.totalPages = Math.ceil(this.registry.size / this.perPage);
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
	 * Delete probe from registry
	 * @param id {number} probe id
	 */
	delete = frame => {
		this.selectedItems.forEach((item) => {
			const probe = this.registry.get(item.id);

			if (probe) {
				probe.isProcessing = true;
				const frame = {
					id: probe.id
				};
				Socket.send(SOCK.DELETE_DEVICE_QUERY, frame, Encoder.DELETE_DEVICE);
			}
			this.isLoading = false;
		})
		this.clearSelected();
		this.modals.delete = false;
	};


	/**
	 * Disable probe frame
	 * @param id {number} probe id
	 */
	disable = () => {
		this.selectedItems.forEach((item) => {
			const probe = this.registry.get(item.id);

			if (probe) {
				probe.isProcessing = true;
				const frame = {
					id: probe.id
				};
				Socket.send(SOCK.DISABLE_DEVICE_QUERY, frame, Encoder.DISABLE_DEVICE);
			}
			this.isLoading = false;
		})
		this.clearSelected();
		this.modals.disable = false;
	};


	reset = frame => {
		this.selectedItems.forEach((item) => {
			const probe = this.registry.get(item.id);

			if (probe) {
				probe.isProcessing = true;
				const frame = {
					id: probe.id
				};
				Socket.send(SOCK.RESET_DEVICE_QUERY, frame, Encoder.RESET_DEVICE);
			}
			this.isLoading = false;
		})
		this.clearSelected();
		this.modals.reset = false;
	};


	/**
	 * Add probe markers to map
	 */
	loadMarkers = () => {
		this.registry.forEach(probe => {
			if (MapStore.instance !== null && MapStore.instance !== undefined) {
				probe.marker.addTo(MapStore.instance);
				MapStore.latlngs.push(probe.marker.getLatLng());
			}
		});
	};

	/**
	 * @override
	 */
	getActualPage = () => {
		this.isLoading = false;
	};

	/**
	 * Select probe for bulk action
	 * @param event {Event}
	 */
	select = probe => {
		const checked = this.selected.has(probe.id);
		checked ? this.selected.delete(probe.id) : this.selected.set(probe.id, probe);
	};

	/**
	 * Clear selections on list
	 */
	clearSelected = () => {
		this.selected = observable.map();
	};

	// on single probe
	handleDeviceInfo = (input) => {
		this.addDevice(input);
	}

	/**
	 * DEVICE_LIST frame handler
	 * @param input {Object}
	 */
	handleList = input => {
		this.add(input.devices);
	};

	/**
	 * DEVICE_LIST frame handler
	 * @param input {Object}
	 */
	handleResetDevice = input => {
		if (input.status === STATUS.SUCCESS) {
			Toasts.add("success", "Sukces", `Pomyślnie zresetowano sondę ${input.id}`);
		} else {
			Toasts.add("error", "Błąd", `Zresetowanie sondy nie powiodło się`);
		}

		if (this.processing.reset) {
			this.processing.reset = false;
		}
	}



	/**
	 * ADD_DEVICE frame handler
	 * @param input {Object}
	 */
	handleAddProbe = input => {
		if (input.status === STATUS.SUCCESS) {
			const probe = new Probe(this, input);
			this.registry.set(probe.id, probe);
			this.totalPages = Math.ceil(this.registry.size / this.perPage);
			this.modals.add = false;
			Toasts.add("success", "Sukces", `Dodano nową sondę ${input.id}`);
		} else {
			Toasts.add("error", "Błąd", `Dodanie nowej sondy nie powiodło się`);
		}

		if (this.processing.add) {
			this.processing.add = false;
		}
	};

	/**
	 * DELETE_DEVICE frame handler
	 * @param input {Object}
	 */
	handleRemoveProbe = input => {
		this.processing.delete = false;
		if (input.status === STATUS.SUCCESS) {
			if (this.registry.has(input.id)) {

				// const probe = this.registry.get(input.id);
				// probe.destroy();
				this.registry.delete(input.id);
				this.totalPages = Math.ceil(this.registry.size / this.perPage);
				Toasts.add("success", "Sukces", `Usunięto sondę ${input.id}`);
			}
		} else {
			Toasts.add("error", "Błąd", `Nie udało się usunąć sondy`);
		}
	};

	/**
	 * DELETE_DEVICE frame handler
	 * @param input {Object}
	 */
	handleDisableProbe = input => {
		this.processing.delete = false;
		if (input.status === STATUS.SUCCESS) {
			if (this.registry.has(input.id)) {

				this.registry.delete(input.id);
				this.totalPages = Math.ceil(this.registry.size / this.perPage);
				Toasts.add("success", "Sukces", `Usunięto sondę ${input.id}`);
			}
		} else {
			Toasts.add("error", "Błąd", `Nie udało się usunąć sondy`);
		}
	};

	/**
	 * DEVICE_SHORT_STATUS frame handler
	 * @param input {Object}
	 */
	handleProbeShortStatus = input => {
		const probe = this.registry.get(input.id);
		probe && probe.update(input, "short");
	};

	/**
	 * DEVICE_FULL_STATUS frame handler
	 * @param input {Object}
	 */
	handleProbeFullStatus = input => {
		if (this.registry.has(input.id)) {
			const probe = this.registry.get(input.id);
			probe.update(input);
		}
	};

	/**
	 * FFT frame handler
	 * @param input {Object}
	 */
	handleMeasurement = input => {
		let probe = this.registry.get(input.id);

		if (probe) {
			const channel = probe.channels[input.channel];

			if (channel && channel.measure) {
				channel.set(input);
			}
		}
	};

	/**
	 * CONFIGURATION_LOAD_STATUS frame handler
	 * @param input {Object}
	 */
	handleConfigStatus = input => {
		if (this.processing.config) {
			this.clearSelected();
			this.result.config = input;
			this.processing.config = false;
			input.devices.forEach((device) => {
				if (device.status === STATUS.SUCCESS) Toasts.add("success", "Sukces", `Załadowanie nowego pliku konfiguracyjnego powiodło się`);
				else Toasts.add("error", "Błąd", `Załadowanie nowego pliku konfiguracyjnego nie powiodło się`);
			})
		}
	};

	/**
	 * FIRMWARE_LOAD_STATUS frame handler
	 * @param input {Object}
	 */
	handleFirmwareStatus = input => {
		if (this.processing.firmware) {
			this.clearSelected();
			this.result.firmware = input;
			this.processing.firmware = false;
			input.devices.forEach((device) => {
				if (device.status === STATUS.SUCCESS) Toasts.add("success", "Sukces", `Załadowanie nowego pliku oprogramowania powiodło się`);
				else Toasts.add("error", "Błąd", `Załadowanie nowego pliku oprogramowania nie powiodło się`);
			})

		}
	};

	/**
	 * NEW_CONFIGURATION_UPLOADED frame handler
	 * @param input {Object}
	 */
	handleConfigUpload = status => {
		if (status === STATUS.SUCCESS) {
			Toasts.add("success", "Sukces", `Załadowanie nowego pliku konfiguracyjnego powiodło się`);
		} else {
			Toasts.add("error", "Błąd", `Załadowanie nowego pliku konfiguracyjnego nie powiodło się`);
		}

		if (this.processing.configUpload) {
			this.processing.configUpload = false;
		}
	};

	/**
	 * NEW_FIRMWARE_UPLOADED frame handler
	 * @param input {Object}
	 */
	handleFirmwareUpload = input => {
		if (input.status === STATUS.SUCCESS) {
			Toasts.add("success", "Sukces", `Załadowanie nowego pliku oprogramowania powiodło się`);
		} else {
			Toasts.add("error", "Błąd", `Załadowanie nowego pliku oprogramowania nie powiodło się`);
		}

		if (this.processing.firmwareUpload) {
			this.processing.firmwareUpload = false;
		}
	};
}

decorate(Probes, {
	modals: observable,
	processing: observable,
	isFilteringOpen: observable,
	result: observable,
	current: observable,
	getCurrent: computed,
	columns: observable,
});

export default Probes;
