import { decorate, observable } from "mobx";
import CentralSystem from "../models/CentralSystem";

/**
 * Common settings of applications
 * @class
 */
class Common {
	/**
	 * Dark theme state
	 * @type {boolean}
	 */
	isDarkTheme = false;

	/**
	 * Connection to WebSocket state
	 * @type {boolean}
	 */
	isConnected = false;

	/**
	 * Connecting state
	 * @type {boolean}
	 */
	connecting = true;

	/**
	 * Loading state
	 * @type {boolean}
	 */
	isLoading = false;

	/**
	 * Instance of system status
	 * @type {CentralSystem}
	 */
	system = new CentralSystem();

	/**
	 * Create instance of user settings and load them from localStorage
	 * @constructor
	 */
	constructor() {
		if (localStorage.getItem("commonConfig") !== null) {
			const config = JSON.parse(localStorage.getItem("commonConfig"));

			if (config) {
				this.isDarkTheme = config.isDarkTheme;
				this.toggleTheme();
			}
		}
	}


	/**
	 * Set document title visible in browser tab
	 * @param title {string}
	 */
	setTitle = title => {
		document.title = `${title} | ProbeSystem`;
	};

	/**
	 * Change theme state handler
	 * @param event {Event}
	 */
	changeTheme = event => {
		this.isDarkTheme = event.target.checked;
		localStorage.setItem("commonConfig", JSON.stringify({ isDarkTheme: this.isDarkTheme }));
		this.toggleTheme();
	};

	/**
	 * Toggle theme, and save on localstorage
	 */
	toggleTheme = () => {
		const html = document.getElementsByTagName("html")[0];
		!this.isDarkTheme && html.classList.remove("bg-dark");
		this.isDarkTheme && html.classList.add("bg-dark");
	};

	/**
	 * Socket CS_STATUS frame handler
	 * @param input {Object}
	 */
	handleCentralSystem = input => {
		this.system.update(input);
	};
}

decorate(Common, {
	isDarkTheme: observable,
	isConnected: observable,
	conecting: observable,
	isLoading: observable,
	system: observable,
});

export default new Common();