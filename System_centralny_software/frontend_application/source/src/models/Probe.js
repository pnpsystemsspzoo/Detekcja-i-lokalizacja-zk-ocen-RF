import { decorate, observable } from "mobx";
import Model from "../stores/base/Model";
import React from "react";
import * as L from "leaflet";

import Usage from "./Usage";
import ReactDOMServer from "react-dom/server";
import Popup from "../containers/parts/Popup";
import classNames from "classnames";
import Threat from "./Threat";
import Measurement from "./Measurement";
import MapStore from "../stores/Map.js"

import moment from "moment";
import "moment/locale/pl";

/**
 * Probe object
 * @class
 */
class Probe extends Model {
	/**
	 * Reference to the registry of probes
	 * @type {Probes}
	 */
	store;

	/**
	 * @type {number}
	 */
	id;


	logs;


	/**
	 * Health of probe
	 * 0 - not working or not working properly
	 * 1 - partly working (some problem exist)
	 * 2 - working properly
	 * @type {number}
	 */
	hardwareHealth;

	/**
	 * Health of probe application
	 * 0 - not working or not working properly
	 * 1 - partly working (some problem exist)
	 * 2 - working properly
	 * @type {number}
	 */
	softwareHealth;

	/**
	 * @type {string}
	 */
	macAddress;

	/**
	 * @type {string}
	 */
	ipAddress;

	/**
	 * @type {number}
	 */
	longitude;

	/**
	 * @type {number}
	 */
	latitude;

	/**
	 * @type {boolean}
	 */
	connected;

	/**
	 * @type {number}
	 */
	dataPort;

	/**
	 * @type {number}
	 */
	controlPort;

	/**
	 * @type {number}
	 */
	creationDate;

	/**
	 * @type {number}
	 */
	expirationDate;

	/**
	 * @type {number}
	 */
	code;

	/**
	 * @type {number}
	 */
	who;

	/**
	 * @type {number}
	 */
	firstMess;

	/**
	 * @type {number}
	 */
	cards = [];


	/**
	 * @type {number}
	 */
	alerts;

	/**
	 * @type {number}
	 */
	card_0_Health;

	/**
	 * @type {number}
	 */
	card_1_Health;

	/**
	 * @type {number}
	 */
	card_2_Health;

	/**
	 * @type {number}
	 */
	card_3_Health;

	/**
	 * @type {number}
	 */
	card_4_Health;

	/**
	 * @type {number}
	 */
	motherboardVoltage;

	/**
	 * @type {number}
	 */
	motherboardCurrent;

	/**
	 * @type {number}
	 */
	radioInterference;

	/**
	 * @type {number}
	 */
	timeStd;

	/**
	 * @type {boolean}
	 */
	synchronization;

	/**
	 * @type {number}
	 */
	tasksNumber;

	/**
	 * @type {boolean}
	 */
	communication;
	deviation;

	/**
	 * @type {Threat[]}
	 */
	threats = [];

	/**
	 * @type {L.divIcon}
	 */
	markerIcon;

	/**
	 * @type {L.marker}
	 */
	marker;

	/**
	 * @type {boolean}
	 */
	version;

	/**
	 * @type {String}
	 */
	datetime;

	currentHddUsage;
	currentCpuUsage;
	currentRamUsage;

	/**
	 * Array of probe resources usage
	 * @type {{txTransfer: Usage, rxTransfer: Usage, hdd: Usage, cpu: Usage, ram: Usage}}
	 */
	usage = {
		hdd: new Usage("HDD"),
		ram: new Usage("RAM"),
		cpu: new Usage("CPU"),
		// transfer: new Usage("Transfer"),
		txTransfer: new Usage("Transfer"),
		rxTransfer: new Usage("Transfer"),
	};

	/**
	 * Array of channels
	 * @type {Measurement[]}
	 */
	channels = [
		new Measurement(this, "ILS 110"),
		new Measurement(this, "ILS 330"),
		new Measurement(this, "TETRA"),
		new Measurement(this, "GNSS")
	];


	/**
	 * `Create new instance of probe
	 * @param store {Probe}
	 * @param input {Object}
	 */
	constructor(store, input) {
		super();
		this.store = store;

		this.id = input.id;
		this.lastUpdate = Date.now();
		// this.communication = input.communication;
		this.version = input.version;
		this.macAddress = input.macAddress;
		this.ipAddress = input.ipAddress;
		this.longitude = input.longitude;
		this.latitude = input.latitude;
		this.height = input.height;
		this.connected = input.connected;
		this.hardwareHealth = input.hardwareHealth;
		this.softwareHealth = input.softwareHealth;
		this.dataPort = input.dataPort;
		this.controlPort = input.controlPort;
		this.expirationDate = moment(input.expirationDate).format('YYYY-MM-DD HH:mm:ss');
		this.creationDate = moment(input.creationDate).format('YYYY-MM-DD HH:mm:ss');
		this.code = input.code;
		this.who = input.who;
		this.firstMess = input.firstMess;
		this.cards = input.cards;

		if (input.id && input.longitude && input.latitude) {
			this.createMarker();
		}

	}

	destroy = () => {
		if (this.marker) {
			// MapStore.clearLayers();
			MapStore.instance.removeLayer();
			//    MapStore.removeLayer(this.marker);
		}
	};

	/**
	 * Create marker on map
	 * @returns {L.marker}
	 */
	createMarker = () => {
		let classe = classNames("probe-icon", {
			"probe--disconnected": !this.connected,
			"probe--error": this.hardwareHealth === 0,
			"probe--warning": this.hardwareHealth === 1,
			"probe--good": this.hardwareHealth === 2,
		});

		this.markerIcon = L.divIcon({
			className: classe,
			html: `<svg width="25" height="25" viewBox="0 0 510 510" version="1.1" xmlns="http://www.w3.org/2000/svg"
			           xmlns:xlink="http://www.w3.org/1999/xlink">
				<g stroke="none" stroke-width="1" fill="none" fill-rule="evenodd">
					<path
						d="M256,25 C384.549923,25 489.195645,127.346049 492.898592,254.999571 L19.1014077,254.999571 C22.8043547,127.346049 127.450077,25 256,25 Z"
						class="bottom" fill="#FFFFFF"></path>
					<path
						d="M254.722811,255.71875 C383.272734,255.71875 487.918456,358.064799 491.621403,485.718321 L17.8242187,485.718321 C21.5271658,358.064799 126.172888,255.71875 254.722811,255.71875 Z"
						class="top" fill="#FFFFFF"
						transform="translate(254.722811, 370.718535) rotate(-180.000000) translate(-254.722811, -370.718535) "></path>
					<rect fill="#000000" x="47" y="230" width="423" height="60"></rect>
					<path
						d="M426.75622,259.037932 C426.297789,209.627875 407.245142,165.623341 372.865126,132.979574 C340.545606,102.292244 297.438764,84.707152 255,84.707152 C212.561236,84.707152 169.454394,102.292244 137.134874,132.979574 C102.324303,166.032153 83.2352941,210.721222 83.2352941,260.868924 C83.2352941,321.532079 108.833053,369.556454 162.328563,408.276051 C179.294201,420.555637 183.258385,444.258781 171.349928,461.455219 C159.211076,478.98436 135.150239,483.190223 117.852994,470.670623 C44.7042619,417.726189 7,346.671379 7,260.868924 C7,189.551412 34.7510179,124.639618 84.8533494,77.0675381 C131.216252,33.0460403 192.979645,7.88558241 255,7.88558241 C317.020355,7.88558241 378.783748,33.0460403 425.146651,77.0675381 C475.248982,124.639618 503,189.551412 503,260.868924 L503,261.861032 L502.818316,265.324224 L502.651256,268.095755 C497.001791,355.901173 459.13074,424.77473 390.368603,471.351489 C372.691659,483.325154 348.780108,478.363418 337.18823,460.47671 C325.811021,442.921245 330.491749,419.344487 347.837463,407.595185 C396.92091,374.347994 422.763332,326.936761 426.650006,261.931905 L426.761065,259.956672 L426.75622,259.037932 Z M453.034332,259.895976 L476.765482,261.140948 L476.762226,261.205713 L452.99978,261.205713 L452.99978,260.55458 L453.034332,259.895976 Z M452.892707,262.59978 L476.617443,263.944476 L476.617148,263.94967 L452.886979,262.704749 L452.892707,262.59978 Z"
						class="border" stroke="rgba(255,255,255,.5)" stroke-width="15" fill="#000000" fill-rule="nonzero"></path>
				</g>
			</svg>`,
			iconSize: [25, 25],
			iconAnchor: [12, 12]
		});

		this.marker = L.marker([this.latitude, this.longitude],
			{
				icon: this.markerIcon
			});

		this.marker.bindPopup(ReactDOMServer.renderToString(<Popup probe={this} />));

		return this.marker;
	};

	updateMarkerDetected = () => {
		if (this.marker) {
			if (this.marker._icon) {
				this.marker._icon.classList.add("probe--detected");
			}
		}
	}
	updateMarkerNotDetected = () => {
		if (this.marker) {
			if (this.marker._icon) {
				this.marker._icon.classList.remove("probe--detected");
			}
		}
	}
	/**
	 * Update marker position and state
	 */
	updateMarker = () => {
		if (this.marker) {
			this.marker.getPopup().setContent(ReactDOMServer.renderToString(<Popup probe={this} />));

			if (this.marker._icon) {
				this.marker._icon.classList.remove("probe--good", "probe--warning", "probe--error");

				let classe = classNames({
					"probe--error": this.hardwareHealth === 0,
					"probe--warning": this.hardwareHealth === 1,
					"probe--good": this.hardwareHealth === 2
				});

				if (this.connected) {
					this.marker._icon.classList.remove("probe--disconnected");
				} else {
					this.marker._icon.classList.add("probe--disconnected");
				}

				if (this.systems && this.systems.length) {
					this.marker._icon.classList.add("probe--detected");
				} else {
					this.marker._icon.classList.remove("probe--detected");
				}

				if (classe) {
					this.marker._icon.classList.add(classe);
				}
			}
		}
	};

	/**
	 * Update data from parsed frame short of full status
	 * @param input {Object}
	 * @param type {string}
	 */
	update = (input, type = "full") => {
		if (type === "short") {
			this.hardwareHealth = input.hardwareHealth;
			this.softwareHealth = input.softwareHealth;
			this.communication = input.communication;
			input.interfered.forEach((status, index) => {
				if (this.channels[index]) {
					this.channels[index].interfered = !!status;
				}
			})
		} else if (type === "list") {
			this.version = input.version;
			this.macAddress = input.macAddress;
			this.ipAddress = input.ipAddress;
			this.longitude = input.longitude;
			this.latitude = input.latitude;
			this.height = input.height;
			// this.communication = input.communication;
			this.connected = input.connected;
			this.hardwareHealth = input.hardwareHealth;
			this.softwareHealth = input.softwareHealth;
			this.connected = input.connected;
			this.dataPort = input.dataPort;
			this.controlPort = input.controlPort;
			this.expirationDate = moment(input.expirationDate).format('YYYY-MM-DD HH:mm:ss');
			this.creationDate = moment(input.creationDate).format('YYYY-MM-DD HH:mm:ss');
			this.code = input.code;
			this.who = input.who;
			this.firstMess = input.firstMess;
			this.cards = input.cards;

			if (!this.marker) {
				this.createMarker();
			}
		} else { // full
			this.hardwareHealth = input.hardwareHealth;
			this.softwareHealth = input.softwareHealth;
			this.communication = input.connected;
			// this.connected = input.connected;
			this.tasksNumber = input.tasksNumber;
			this.synchronization = input.synchronization;
			this.deviation = input.deviation;
			this.datetime = input.datetime;
			this.alerts = input.alerts;
			this.card_0_Health = input.card0;
			this.card_1_Health = input.card1;
			this.card_2_Health = input.card2;
			this.card_3_Health = input.card3;
			this.card_4_Health = input.card4;
			this.cards = input.cards;
			this.motherboardCurrent = input.motherboardCurrent;
			this.motherboardVoltage = input.motherboardVoltage;
			this.radioInterference = input.radioInterference;
			this.timeStd = input.timeStd;

			this.currentHddUsage = parseInt(input.hddUsage);
			this.currentRamUsage = parseInt(input.ramUsage);
			this.currentCpuUsage = parseInt(input.cpuUsage);

			this.usage.txTransfer.append(input.txTransferRate);
			this.usage.rxTransfer.append(input.rxTransferRate);

			this.threats = input.threats ? input.threats.map(threat => new Threat(threat)) : [];
		}

		this.updateMarker();
	};


	/**
	 * Reset probe measurement on every channel
	 */
	reset = () => {
		this.channels.forEach(channel => {
			channel.registry = [];
		})
	}
}

decorate(Probe, {
	hardwareHealth: observable,
	softwareHealth: observable,
	communication: observable,
	connected: observable,
	tasksNumber: observable,
	usage: observable,
	logs: observable,
	currentHddUsage: observable,
	currentCpuUsage: observable,
	currentRamUsage: observable,
});

export default Probe;