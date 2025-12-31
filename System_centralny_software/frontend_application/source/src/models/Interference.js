import { computed, decorate, observable } from "mobx";
import Model from "../stores/base/Model";
import * as L from "leaflet";
import { defaultStyles, extinguishedStyles, hoverStyle } from "../config/interference";
import markerIcon from "../assets/location-pin-2.png";
import classNames from "classnames";
// var geo = require('mt-geo');

L.Marker.prototype.setIcon(L.icon({
	iconUrl: markerIcon,
	iconSize: [50, 70],
	iconAnchor: [25, 70]
}))

/**
 * Interference object
 * @class
 */
class Interference extends Model {
	/**
	 * Reference to registry of interferences
	 * @type {Map}
	 */
	store;

	/**
	 * @type {number}
	 */
	id;

	/**
	 * @type {number}
	 */
	longitude;

	/**
	 * @type {number}
	 */
	latitude;

	/**
	 * @type {number}
	 */
	height;

	/**
	 * @type {number}
	 */
	range;

	/**
	 * @type {number}
	 */
	heading;

	duty_cycle;

	rise_rate_slope;

	unix_time;
	date;
	time_var;
	signal_type;
	signal_period;
	f0;
	f0_var;
	bandwidth;
	bandwidth_var;
	J;
	J_var;
	field_1;
	field_2;
	field_3;
	field_4;
	devicesId = [];

	location_variance;

	/**
	 * @type {number}
	 */
	speed;

	/**
	 * Status of interference
	 * @type {number}
	 */
	extinguished = false;

	/**
	 * Basic styles for marker and path of extinguished interference
	 * @type {{path, marker}}
	 */
	extinguishedStyles = extinguishedStyles;

	/**
	 * 'Life' path of interference movement
	 * @type {L.polyline}
	 */
	path;

	pointsArray = [];

	pointsArrayExtendedReport = [];
	pointsArrayBasicReport = [];

	/**
	 * 'Life' path of interference movement
	 * @type {L.polyline}
	 */
	pathBasicReport;
	pathExtendedReport;
	/**
	 * 'Life' path of interference movement
	 * @type {L.marker}
	 */
	pinBasicReport;
	pinExtendedReport;
	/**
	 * Color of interference
	 * @type {string} hex format eg. #FFFFFF
	 */
	color;

	/**
	 * Color of interference
	 * @type {string} hex format eg. #FFFFFF
	 */
	colorBasicReport;
	colorExtendedReport;
	/**
	 * Representation interference on map
	 * @type {L.circle}
	 */
	marker;

	/**
		 * Representation interference on map
		 * @type {L.circle}
		 */
	markerBasicReport;
	markerExtendedReport;


	/**
	 * Timer to check if the interference has stopped
	 * @type {number}
	 */
	interval;

	/**
	 * Time of first detection
	 * @type {number} timestamp
	 */
	startAt;

	/**
	 * @constructs Interference
	 * @param store {Probes}
	 * @param input {Object}
	 */
	constructor(store, input) {
		super();

		this.store = store;

		this.startAt = Date.now();
		this.lastUpdate = Date.now();
		this.id = input.id;

		this.longitude = input.longitude;
		this.latitude = input.latitude;
		this.height = input.height;
		this.range = input.range;
		this.heading = input.heading;
		this.speed = input.speed;
	};

	createCirlceMarker = (color, longitude, latitude) => {
		let classe = classNames("interf-icon");
		let circle = L.divIcon({
			className: classe,
			html: `<svg viewBox="0 0 10 10" xmlns="http://www.w3.org/2000/svg">
						<circle cx="5" cy="5" r="5" fill="${color}" />
					 </svg>`,
			iconSize: [20, 20],
			iconAnchor: [10, 10]
		});
		return L.marker([latitude, longitude], { icon: circle });
	}

	/**
	 * Create interference representation on map
	 */
	createMarker = () => {
		this.path = L.polyline([[this.latitude, this.longitude]], { color: this.color });
		this.marker = L.circle([this.latitude, this.longitude], {
			fillOpacity: 0.5,
			stroke: false,
			radius: this.range,
			weight: 5,
			color: this.color
		});
	};



	createMarkerBasicReport = (id) => {
		this.pinBasicReport = L.marker([this.latitude, this.longitude])
			.bindTooltip(`ID: ${id}`,
				{
					permanent: true,
					direction: 'bottom'
				}
			);
	};

	createMarkerExtendedReport = () => {
		this.pathExtendedReport = L.polyline([[this.latitude, this.longitude]], { color: this.colorExtendedReport });
		this.pinExtendedReport = L.marker([this.latitude, this.longitude]);
	};

	createMarkerExtendedReportDots = () => {
		this.pathExtendedReport = L.polyline([[this.latitude, this.longitude]], { color: this.colorExtendedReport });
		this.pinExtendedReport = L.marker([this.latitude, this.longitude]);
	};

	createLastPoint = (longitude, latitude) => {
		// this.pinExtendedReport = L.marker([parseFloat(longitude), parseFloat(latitude)]);
	}

	/**
	 * Set interference color
	 * @param color {string} hex format eg. #FFFFFF
	 */
	setColor = (color) => {
		this.color = color;
	};

	/**
	 * Set interference color
	 * @param color {string} hex format eg. #FFFFFF
	 */
	setColorBasicReport = (color) => {
		this.colorBasicReport = color;
	};

	setColorExtendedReport = (color) => {
		this.colorExtendedReport = color;
	};

	/**
	 * Set interference timer
	 * @param callback {function}
	 * @param time {number}
	 */
	setInterval = (callback, time) => {
		this.interval = setInterval(callback, time);
	};

	/**
	 * Set extinguished state for interference
	 * @param opacity {number} 0-1
	 */
	setExtinguished = (opacity = null) => {
		clearInterval(this.interval);
		if (this.path !== undefined) {
			this.marker.setStyle(this.extinguishedStyles.marker);
			this.path.setStyle(this.extinguishedStyles.path);
			this.extinguished = true;
		}
	};

	setExtinguishedOpacity = opacity => {
		this.extinguishedStyles.marker.fillOpacity = opacity;
		this.extinguishedStyles.path.opacity = opacity + opacity * 2;

		if (this.extinguished) {
			this.marker.setStyle(this.extinguishedStyles.marker);
			this.path.setStyle(this.extinguishedStyles.path);
		}
	};

	get time() {
		return this.lastUpdate - this.startAt;
	}

	/**
	 * Update data from parsed frame
	 * @param input {Object}
	 */
	update = input => {
		this.lastUpdate = Date.now();
		if (!isNaN(input.latitude) || !isNaN(input.longitude)) {
			this.longitude = input.longitude;
			this.latitude = input.latitude;
		}


		// var longitude_converted = geo.toDMS(input.longitude);
		// var latitude_converted = geo.toDMS(input.latitude);
		// this.longitude = longitude_converted;
		// this.latitude = latitude_converted;

		this.height = input.height;
		this.range = input.range;
		this.heading = input.heading;
		this.speed = input.speed;
		this.location_variance = input.location_variance;

		if (!isNaN(input.latitude) || !isNaN(input.longitude)) { this.updateMarker(this.latitude, this.longitude, this.range); }
	};

	/**
	 * Update data from parsed frame
	 * @param input {Object}
	 */
	updateInfo = input => {
		this.devicesId = input.devicesId.map((device) => {
			return device.id
		})

		this.unix_time = input.unix_time;
		this.date = new Date(input.unix_time * 1000);

		this.time_var = input.time_var;
		this.signal_type = input.signal_type;
		this.signal_period = input.signal_period;
		this.f0 = input.f0;
		this.f0_var = input.f0_var;
		this.bandwidth = input.bandwidth;
		this.bandwidth_var = input.bandwidth_var;
		this.J = input.J;
		this.J_var = input.J_var;
		this.field_1 = input.field_1;
		this.field_2 = input.field_2;
		this.field_3 = input.field_3;
		this.field_4 = input.field_4;
		this.duty_cycle = (input.field_1 / (input.field_1 + input.field_2));
		this.rise_rate_slope = (input.bandwidth / input.field_2);
	};


	/**
	 * Update data from parsed frame
	 * @param input {Object}
	 */
	updateBasicReport = input => {
		this.lastUpdate = Date.now();

		this.longitude = input.longitude;
		this.latitude = input.latitude;
		this.height = input.height;
		this.range = input.range;
		this.heading = input.heading;
		this.speed = input.speed;

		this.updateMarkerBasicReport(this.latitude, this.longitude);
	};

	updateExtendedReport = input => {

		this.lastUpdate = Date.now();

		this.longitude = input.longitude;
		this.latitude = input.latitude;
		this.height = input.height;
		this.range = input.range;
		this.heading = input.heading;
		this.speed = input.speed;

		this.updateMarkerExtendedReport(this.latitude, this.longitude, this.range);
	};

	/**
	 * Get bounds of interference marker and path
	 * @returns {null|Array}
	 */
	getBounds = () => {
		if (this.marker && this.path) {
			return [this.marker.getBounds(), this.path.getBounds()];
		}
		return null;
	};


	/**
	 * Update marker on map
	 * @param latitude {number}
	 * @param longitude {number}
	 * @param range {number}
	 */
	updateMarker = (latitude, longitude, range) => {
		this.path.addLatLng([latitude, longitude]);
		this.pointsArray.push({ lat: latitude, lng: longitude });
		this.marker.setLatLng([latitude, longitude]);
		this.marker.setRadius(range);
	};

	/**
	 * Update marker on map
	 * @param latitude {number}
	 * @param longitude {number}
	 * @param range {number}
	 */
	updateMarkerBasicReport = (latitude, longitude) => {
		// this.pathBasicReport.addLatLng([latitude, longitude]);
		// this.markerBasicReport.setLatLng([latitude, longitude]);
		// this.markerBasicReport.setRadius(range);
		this.pointsArrayExtendedReport.push([latitude, longitude]);
	};

	updateMarkerExtendedReport = (latitude, longitude) => {
		// lines
		// this.pathExtendedReport.addLatLng([latitude, longitude]);
		// this.pinExtendedReport.setLatLng([latitude, longitude]);

		// dots

		this.pointsArrayBasicReport.push([latitude, longitude]);
	};

	/**
	 * Delete interference
	 */
	delete = () => {
		if (this.extinguished) {
			this.destroy();
			this.store.registry.delete(this.id);
		}
	};

	/**
	 * Destroy marker and path on map
	 */
	destroy = () => {
		clearInterval(this.interval);
		this.path.remove();
		this.path = null;
		this.marker.remove();
		this.marker = null;
	};

	/**
	 * Event handler on hover marker
	 */
	onMouseEnter = () => {
		this.marker.setStyle(hoverStyle.marker);
		this.path.setStyle(hoverStyle.path);
	};

	/**
	 * Event handler on hover out marker
	 */
	onMouseLeave = () => {
		if (this.extinguished) {
			this.marker.setStyle(extinguishedStyles.marker);
			this.path.setStyle(extinguishedStyles.path);
		} else {
			this.marker.setStyle(Object.assign(defaultStyles.marker, { color: this.color }));
			this.path.setStyle(Object.assign(defaultStyles.path, { color: this.color }));
		}
	};

}

decorate(Interference, {
	extinguished: observable,
	longitude: observable,
	latitude: observable,
	height: observable,
	range: observable,
	heading: observable,
	speed: observable,
	devicesId: observable,
	time: computed,
	unix_time: observable,
	time_var: observable,
	signal_type: observable,
	signal_period: observable,
	f0: observable,
	f0_var: observable,
	bandwidth: observable,
	bandwidth_var: observable,
	J: observable,
	J_var: observable,
	field_1: observable,
	field_2: observable,
	field_3: observable,
	field_4: observable,
	duty_cycle: observable,
	rise_rate_slope: observable,
});

export default Interference;