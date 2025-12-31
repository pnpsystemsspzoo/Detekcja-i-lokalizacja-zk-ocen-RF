import React from "react";
import { decorate, observable } from "mobx";

/**
 * Usage object
 * @class
 */
class Usage {
	/**
	 * Reference to HTML element
	 * @type {React.RefObject<unknown>}
	 */
	chart = new React.createRef();

	/**
	 * Length of historical data in memory
	 * @type {number}
	 */
	length = 300;

	/**
	 * Name of usage chart
	 * @type {string}
	 */
	name;

	/**
	 * Array with historical data
	 * @type {Array}
	 */
	registry = [];

	/**
	 * Latest data on chart
	 * @type {number}
	 */
	current = 0;

	/**
	 * Create usage chart instance
	 * @param name {string}
	 * @param length {number}
	 */
	constructor(name = "", length = 300) {
		this.name = name;
		this.length = length;
	}

	/**
	 * Get latest element in historical data
	 */
	getCurrent = () => {
		const { [this.registry.length - 1]: last } = this.registry;
		this.current = last.y;
		return last;
	};

	/**
	 * Append new data to registry
	 * @param value {number}
	 */
	append = value => {
		if (this.registry.length >= this.length) {
			this.registry.shift();

		}

		this.current = value;
		this.registry.push({ y: value, x: Date.now() });
		this.update();
	};

	/**
	 * Refresh chart on the screen
	 */
	update = () => {
		if (this.chart.current) {
			this.chart.current.chartInstance.data.datasets[0].data = [...this.registry];
			this.chart.current.chartInstance.update();
		}
	}
}

decorate(Usage, {
	current: observable,
	registry: observable
});


export default Usage;