import React from "react";
import chroma from "chroma-js";
import { computed, decorate, observable } from "mobx";

/**
 * Channel object
 * @class
 */
class Measurement {
	/**
	 * Reference to probe
	 * @type {Probe}
	 */
	probe;

	/**
	 * Reference to HTML canvas element of spectogram chart
	 * @type {React.RefObject<unknown>}
	 */
	spectogram = React.createRef();

	/**
	 * Reference to HTML canvas element of histogram chart
	 * @type {React.RefObject<unknown>}
	 */
	histogram = React.createRef();

	/**
	 * @type {string}
	 */
	name;

	/**
	 * Initial frequency
	 * @type {number}
	 */
	fBegin = 0;

	/**
	 * Częstotliwość końcowa
	 * @type {number}
	 */
	fEnd = 0;

	/**
	 * Channel is in measurement state
	 * @type {boolean}
	 */
	measure = true;

	/**
	 * Channel is interfered by some interference
	 * @type {boolean}
	 */
	interfered = false;

	/**
	 * Number of samples in measurement
	 * @type {number}
	 */
	length = 200;

	/**
	 * Minimal value in destiny
	 * @type {number}
	 */
	min;

	/**
	 * Maximal value in destiny
	 * @type {number}
	 */
	max;

	density = [];
	/**
	 * Spectral power density array
	 * @type {number[]}
	 */
	registry = [];

	/**
	 * Array of steps colors used in gradient
	 * @type {number[]}
	 */
	baseColors = [[0, 0, 255, 1], [0, 255, 255, 1], [0, 255, 0, 1], [255, 255, 0, 1], [255, 0, 0, 1]];

	/**
	 * Percentage position of color in gradient
	 * @type {number[]}
	 */
	positions = [0, 0.25, 0.5, 0.75, 1];

	/**
	 * Chroma object of calculated colors
	 * @type {chroma.scale}
	 */
	scale = null;


	/**
	 * Create new channel instance
	 * @param probe {Probe}
	 * @param name {string}
	 */
	constructor(probe, name = "") {
		this.probe = probe;
		this.name = name;
		// this.scale = new chroma.scale(['#0a12f5', '#f54242']).domain([-128, 127]);
		// this.scale = new chroma.scale(['#0a12f5', '#f54242']).domain([-128, 10]);
		// this.scale = new chroma.scale(['#0a12f5', '#f54242']).domain([-70, -40]);
		this.scale = new chroma.scale(['#343c4a', '#f5f242']).domain([-100, 5]);
	};


	/**
	 * Set new color scale of the specified domain
	 * @param domain {number[]} min - max
	 */
	setScale(domain) {
		// colors: gray, pink
		// this.scale = new chroma.scale(['#343c4a', '#ed7bed']).domain([-128, 127]);
		// this.scale = new chroma.scale(['#343c4a', '#ed7bed']).domain([-128, 10]);
		// this.scale = new chroma.scale(['#343c4a', '#ed7bed']).domain([-70, -40]);
		this.scale = new chroma.scale(['#343c4a', '#f5f242']).domain([-100, 5]);
	}

	/**
	 * Get availability to show
	 * @returns {boolean}
	 */
	get available() {
		return this.registry.length > 0;

	}

	/**
	 * Get color in scale
	 * @param value
	 * @returns {Array} index 0 - red color, 1 - green color, 2 - blue color
	 */
	getColor = (value) => {
		return this.scale(value).rgb();
	};

	/**
	 * Append measurement data from frame to history and redraw charts
	 * @param input {Object}
	 */

	set = input => {
		this.length = input.densityLength; // constant, 256
		// this.min = -128;
		// this.max = 127;
		this.min = -80;
		this.max = 20;
		this.fBegin = input.fBegin;
		this.fEnd = input.fEnd;
		// this.fEnd = 430;
		// this.setScale([-128, 127]);
		this.setScale([-100, 5]);
		this.measure = true;

		let krok = (input.fEnd - input.fBegin) / (input.densityLength - 1);

		this.registry = input.density.map((value, i) => {
			return { x: 100000, y: value, c: this.getColor(value) };
		});

		this.density = input.density.map((value, i) => {
			return [input.fBegin + (krok * i), value];
		});


		// this.density = input.density;

		this.updateSpectogram();
		this.updateHistogram();
	};

	/**
	 * Redraw spectogram chart
	 */
	updateSpectogram = () => {
		if (this.spectogram.current) {
			this.spectogram.current.chartInstance.data.datasets[0].data = [...this.registry];
			this.spectogram.current.chartInstance.update();
		}
	};

	/**
	 * Redraw histogram chart
	 */
	updateHistogram = () => {
		if (this.histogram.current) {
			let canvas = this.histogram.current;
			let ctx = canvas.getContext("2d", { alpha: false });
			let old = ctx.getImageData(0, 0, this.length, 250);
			ctx.putImageData(old, 0, 1);

			var j = 0;
			let im = ctx.createImageData(this.length, 1);

			this.registry.forEach(c => {
				im.data[j] = c.c[0];
				im.data[j + 1] = c.c[1];
				im.data[j + 2] = c.c[2];
				im.data[j + 3] = 255;
				j += 4;
			});

			ctx.putImageData(im, 0, 0);
		}
	};

	/**
	 * Remove measurement data
	 */
	reset = () => {
		if (!this.measure) {
			this.registry = [];
		}
	};
}

decorate(Measurement, {
	interfered: observable,
	measure: observable,
	available: computed,
	registry: observable,
	fBegin: observable,
	fEnd: observable,
	density: observable
});

export default Measurement;