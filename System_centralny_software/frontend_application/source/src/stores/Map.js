import { action, computed, decorate, observable } from "mobx";
import * as L from "leaflet";
import Interference from "../models/Interference";
import config from "../config/main";

/**
 * Map object
 * @class
 */
class MapE {
	basicReportData = [];
	extendedReportData = [];

	/**
	 * Map is loading
	 * @type {boolean}
	 */
	isLoading = true;

	/**
	 * instance of map
	 * @type {L.Map}
	 */
	instance;

	/**
	 * instance of map
	 * @type {L.Map}
	 */
	instanceBasicReport;

	instanceExtendedReport;

	/**
	 * Current layer
	 */
	layer;

	/**
	 * Map configuration
	 * @type {{lng: number, zoom: number, opacity: number, lat: number}}
	 */
	config = {
		lat: config.MAP.LAT,
		lng: config.MAP.LNG,
		zoom: config.MAP.ZOOM,
		opacity: 0.5
	};

	latitudeMin = 50.097452;
	latitudeMax = 50.125084;
	longitudeMin = 21.972415;
	longitudeMax = 22.063199;

	/**
	 * Registry of interferences
	 * @type {ObservableMap<number, Interference>}
	 */
	registry = observable.map();

	/**
	 * Registry of interferences
	 * @type {ObservableMap<number, Interference>}
	 */
	registryBasicReport = observable.map();

	registryExtendedReport = observable.map();

	/**
	 * Array of every position of interference for bound calculation
	 * @type {Array}
	 */
	latlngs = [];

	/**
	 * Interference colors
	 * @type {string[]} hex format
	 */
	colorArray = [
		"#14bf11", "#f58231", "#bfef45", "#911eb4", "#000075", "#42d4f4", "#4363d8", "#469990", "#9A6324", "#e6194B"
	];

	constructor() {
		if (localStorage.getItem("mapConfig") !== null) {
			this.config = JSON.parse(localStorage.getItem("mapConfig"));
		} else {
			this.config = { lat: config.MAP.LAT, lng: config.MAP.LNG, zoom: config.MAP.ZOOM };
		}
	}

	// set Report Data with "last" values of longitude and latitude (pin will be in the last position of interference)
	setBasicReportData = (data) => {
		let newData = data.map((row) => {
			row.longitude = row.longitude_last;
			row.latitude = row.latitude_last;
			return row;
		})

		this.basicReportData = newData;
	}

	setExtendedReportData = (data) => {
		this.extendedReportData = data;
	}

	getColor = () => {
		return this.colorArray[this.registry.size % 10];
	};

	getColorReport = () => {
		return this.colorArray[this.registryBasicReport.size % 10];
	};

	getColorExtendedReport = () => {
		return this.colorArray[this.registryExtendedReport.size % 10];
	};

	center = () => {
		if (this.instance) {
			this.instance.setView([this.config.lat, this.config.lng], this.config.zoom);
		}
	};

	saveConfig = (lat, lng, zoom, opacity) => {
		localStorage.setItem("mapConfig", JSON.stringify({ lat, lng, zoom, opacity }));

		if (opacity !== this.config.opacity) {
			this.interferences
				.forEach(interference => interference.setExtinguishedOpacity(opacity));
		}
		this.config = { lat, lng, zoom, opacity };

	};

	activeLayers = new Set();
	wmsLayer = L.tileLayer.wms(config.MAP.URL, {
		service: "WMS",
		version: "1.1.0",
		request: "GetMap",
		layers: "Mozaika_Jasionka",
	});

	layers = [
		// osm: L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {}),
		{
			title: "Mozaika",
			layers: [
				{ name: "Mozaika_Jasionka", title: "Mozaika", "active": true },
			]
		},
		{
			title: "Lidar",
			layers: [
				{ name: "LIDAR", title: "Wszystkie", "active": false },
				{ name: "building", title: "Zabudowa", "active": false },
				{ name: "ground", title: "Grunt", "active": false },
				{ name: "water", title: "Woda", "active": false },
				{ name: "veg_high", title: "Roślinność wysoka", "active": false },
				{ name: "veg_med", title: "Roślinność średnia", "active": false },
				{ name: "veg_low", title: "Roślinność niska", "active": false }
			]
		},
		{
			title: "Inne",
			layers: [
				{ name: "NMPT", title: "NMPT", "active": false },
				{ name: "NMT", title: "NMT", "active": false },
			]
		}
	];

	init = () => {
		this.wmsLayer.addTo(this.instance);
	};

	initBasicReport = () => {
		this.wmsLayer.addTo(this.instanceBasicReport);
	};

	initExtendedReport = () => {
		this.wmsLayer.addTo(this.instanceExtendedReport);
	};

	toggleLayer = layer => {
		layer.active = !layer.active;

		if (layer.active) {
			this.activeLayers.add(layer.name);
		} else {
			this.activeLayers.delete(layer.name);
		}

		const layers = [...this.activeLayers].join(",");
		this.wmsLayer.setParams({ layers: layers });
	};


	fitBounds = () => {
		this.instance && this.instance.fitBounds(this.latlngs);
	};

	centerInterference = interference => {
		const bounds = interference.getBounds();
		bounds && this.instance.fitBounds(bounds);
	};

	get interferences() {
		return [...this.registry.values()];
	}

	get active() {
		return this.interferences.filter(interference => !interference.extinguished);
	}

	get inactive() {
		return this.interferences.filter(interference => interference.extinguished);
	}

	handleInterference = input => {
		let interference = this.registry.get(input.id);

		if (interference) { // interference exists
			interference.update(input);
			if (this.instance) {
				let circle = interference.createCirlceMarker(interference.color, interference.longitude, interference.latitude);
				circle.addTo(this.instance);
			}
		}
		else { // interference doesnt exist
			interference = new Interference(this, input);

			const color = this.getColor();
			if (!isNaN(input.latitude) || !isNaN(input.longitude)) {
				interference.setInterval(() => {
					if (interference && (Date.now() - interference.lastUpdate) > 1000) {
						interference.setExtinguished();
					}
				}, 50);

				interference.setColor(color);
				interference.createMarker();

				if (this.instance) {
					interference.marker.addTo(this.instance);

					let circle = interference.createCirlceMarker(interference.color, interference.longitude, interference.latitude);
					circle.addTo(this.instance);
				}
			}
		}

		this.registry.set(interference.id, interference);
	};

	handleInterferenceInfo = (input) => {
		let interference = this.registry.get(input.signal_id);

		if (interference) {
			interference.updateInfo(input);
		}
	}

	clearRegistryBasicReport = () => {
		this.registryBasicReport.clear();
	}

	handleInterferenceReportPins = () => {
		let lngLatArray = [];

		this.basicReportData.forEach((interf) => {

			const color = this.getColorReport();
			let interference = new Interference(this, interf);

			interference.setColorBasicReport(color);
			interference.createMarkerBasicReport(interference.id);
			lngLatArray.push([interf.latitude_last, interf.longitude_last]);

			if (this.instanceBasicReport) {
				interference.pinBasicReport.addTo(this.instanceBasicReport);
			}

			this.registryBasicReport.set(interference.id, interference);
		});

		// only if puppeteer opens tab because instanceBasicReport is undefined in normal browser (the component is not rendered)
		if (this.instanceBasicReport) {
			let boundsReport = new L.LatLngBounds(lngLatArray);
			this.instanceBasicReport.fitBounds(boundsReport);
		}
	};

	handleInterferenceExtendedReportLines = () => {
		// dots
		let lngLatArray = [];

		this.extendedReportData.forEach((interf) => {
			let interference = this.registryExtendedReport.get(interf.id);
			const color = "#f5dd0a";
			if (interference) {
				interference.updateExtendedReport(interf);
				lngLatArray.push([interference.latitude, interference.longitude]);

				let circlee = L.circle([interference.latitude, interference.longitude], {
					fillOpacity: 0.5,
					stroke: false,
					radius: 4,
					weight: 5,
					color: color
				});
				circlee.addTo(this.instanceExtendedReport);

			} else {
				interference = new Interference(this, interf);
				interference.setColorExtendedReport(color);
				interference.createMarkerExtendedReportDots();
				lngLatArray.push([interference.latitude, interference.longitude]);

				if (this.instanceExtendedReport) {
					let circlee = L.circle([interference.latitude, interference.longitude], {
						fillOpacity: 0.5,
						stroke: false,
						radius: 4,
						weight: 5,
						color: color
					});
					circlee.addTo(this.instanceExtendedReport);
				}
			}
			this.registryExtendedReport.set(interference.id, interference);
		})

		this.extendedReportData.forEach((interf) => {
			this.addMarkerWithLastPoint(interf.id);
		});

		// bounds
		if (this.instanceExtendedReport) {
			let bounds = new L.LatLngBounds(lngLatArray);
			this.instanceExtendedReport.fitBounds(bounds);
		}
	};

	addMarkerWithLastPoint = (id) => {
		let interference = this.registryExtendedReport.get(id);
		interference.createLastPoint(interference.latitude, interference.longitude);
		interference.pinExtendedReport.addTo(this.instanceExtendedReport);
	}

	removeExtinguished = () => {
		this.interferences.forEach(interference => {
			if (interference.extinguished) {
				this.deleteInterference(interference);
			}
		});
	};

	deleteInterference = interference => {
		interference.delete();
	};

	loadInterferences = () => {
		this.interferences.forEach(interference => {
			// draw dots after logout or change component
			interference.pointsArray && interference.pointsArray.forEach((point) => {
				let circle = interference.createCirlceMarker(interference.color, point.lng, point.lat);
				circle.addTo(this.instance);
			})
		});
	}
}

decorate(MapE, {
	isLoading: observable,
	registry: observable,
	registryBasicReport: observable,
	handleInterference: action,
	layers: observable,
	interferences: computed,
	active: computed,
	inactive: computed,
	activeLayer: observable,
});

export default new MapE();