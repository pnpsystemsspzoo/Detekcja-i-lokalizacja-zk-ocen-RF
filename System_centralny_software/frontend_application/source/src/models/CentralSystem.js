import { decorate, observable } from "mobx";
import Model from "../stores/base/Model";
import Usage from "./Usage";

/**
 * Central system object
 * @class
 */
class CentralSystem extends Model {
	/**
	 * Health of central system
	 * 0 - not working or not working properly
	 * 1 - partly working (some problem exist)
	 * 2 - working properly
	 * @type {number}
	 */
	hardwareHealth;

	/**
	 * Health of central system application
	 * 0 - not working or not working properly
	 * 1 - partly working (some problem exist)
	 * 2 - working properly
	 * @type {number}
	 */
	softwareHealth;

	/**
	 * Communication status
	 * 0 - not working or not working properly
	 * 1 - partly working (some problem exist)
	 * 2 - working properly
	 * @type {number}
	 */
	communication;

	/**
	 * Number of tasks launched
	 * @type {number}
	 */
	tasksNumber;

	currentHddUsage;
	currentCpuUsage;
	currentRamUsage;

	/**
	 * Use of system resources
	 * @type {{cpu: Usage, hdd: Usage, ram: Usage}}
	 */
	usage = {
		cpu: new Usage("GPU", 500),
		hdd: new Usage("HDD", 500),
		ram: new Usage("RAM", 500)
	};

	/**
	 * Update data from parsed frame
	 * @param input {Object}
	 */
	update = input => {
		this.lastUpdate = Date.now();

		this.hardwareHealth = input.hardwareHealth;
		this.softwareHealth = input.softwareHealth;
		this.communication = input.communication;
		this.tasksNumber = input.tasksNumber;

		// this.currentHddUsage = parseInt(input.hddUsage);
		// this.currentCpuUsage = parseInt(input.cpuUsage);
		// this.currentRamUsage = parseInt(input.ramUsage);

		this.currentHddUsage = parseInt(Math.floor(Math.random() * (60 - 40 + 1) + 40));
		this.currentRamUsage = parseInt(Math.floor(Math.random() * (80 - 50 + 1) + 50));
		this.currentCpuUsage = parseInt(Math.floor(Math.random() * (90 - 60 + 1) + 60));

	};
}

decorate(CentralSystem, {
	hardwareHealth: observable,
	softwareHealth: observable,
	communication: observable,
	tasksNumber: observable,
	usage: observable,
	currentHddUsage: observable,
	currentCpuUsage: observable,
	currentRamUsage: observable,
});

export default CentralSystem;