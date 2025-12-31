import { decorate, autorun, observable, computed, action } from "mobx";
import axios from "axios";
import Api from "./base/Api";
import Toasts from "./Toasts";
require('dotenv').config()

class Auth {
	timerId = null;
	isAuthenticated = false;
	hasToChangeFirstPassword = false;
	hasToChangePassword = false;
	isLoading = false;
	id = null;
	token = null;
	permissions = { alerts: true };
	owner = "";
	user = { firstName: "", lastName: "" };
	role = "";

	/**
	 * Authenticated user is admin
	 * @type {boolean}
	 */
	isAdmin = true;

	isProcessing = {
		password: false
	};

	constructor() {
		autorun(() => {
			let token = localStorage.getItem("token");
			this.token = token;
			token !== null ? this.loadAuthorization(token) : (this.isLoading = false);
		});
	}

	loadAuthorization = token => {
		axios.defaults.headers.common["Token"] = token;
	};

	initialValues = {
		login: {
			login: "",
			password: "",
		},
		passwordChange: {
			current_password: "",
			password: "",
			password_repeat: ""
		},
		firstPasswordSet: {
			first_password: "",
			first_password_repeat: ""
		}
	};

	get initial() {
		return this.user.firstName[0] + " " + this.user.lastName[0];
	}

	get name() {
		return this.user.firstName + " " + this.user.lastName;
	}

	get type() {
		return this.role;
	}

	loginResponse = (res) => {
		// websocket login response
		if (res.errorCode === 1) {
			localStorage.removeItem('token');
			this.isAuthenticated = false;
			this.token = null;
		}
		else if (res.errorCode === 2) {
			Toasts.add("error", "Błąd", "Niepoprawne dane logowania");
		}
		else if (res.errorCode === 3) {
			Toasts.add("error", "Błąd", "Użytkownik został zablokowany");
		}
		else if (res.errorCode === 4) {
			Toasts.add("info", "Informacja", "Użytkownik przy pierwszym logowaniu musi zmienić hasło.");
		}
		else if (res.errorCode === 5) {
			Toasts.add("info", "Informacja", "Użytkownik musi zmienić hasło.");
			this.id = res.user_ID;
			this.hasToChangePassword = true;
		}
		else if (res.errorCode === 0) {
			this.id = res.user_ID;
			this.role = res.userType;

			if (res.firstPassChanged === "no") this.hasToChangeFirstPassword = true;
			else {
				this.isAdmin = res.isAdmin === "yes" ? true : false;
				this.token = res.token;

				this.user = {
					firstName: res.firstName,
					lastName: res.lastName,
				};
				this.isAuthenticated = true;
				localStorage.setItem('token', this.token);

				this.permissions = {
					alerts: res.alerts === "yes" ? true : false,
					manage: res.manage === "yes" ? true : false,
					probes: res.probes === "yes" ? true : false,
					users: res.users === "yes" ? true : false,
					cs: res.cs === "yes" ? true : false,
					reports_list: res.reportsList === "yes" ? true : false,
					generate_report: res.generateReport === "yes" ? true : false,
					signatures_list: res.signaturesList === "yes" ? true : false,
					permissions: res.permissions === "yes" ? true : false,
					map: res.map === "yes" ? true : false,
				}
			}

		}
	}

	toggleCheckboxesPermissions = (what) => {
		this.permissions[what] = !this.permissions[what];
	}

	setPasswordFirstTime = (data) => {
		Api.account.password.set_first(JSON.stringify({ userid: this.id, password: data.password }))
			.then(action(res => {
				if (res.data.status === "Success") {
					Toasts.add("success", "Sukces", "Poprawnie zmieniono hasło. Zaloguj się.");
					this.hasToChangeFirstPassword = false;
				}
			}))
			.catch(err => {
				Toasts.add("error", "Błąd", "Coś poszło nie tak.");
			})
			.finally(() => {
				this.isLoading = false;
			});
	};

	setPassword = (data) => {
		Api.account.password.set(JSON.stringify({ userid: this.id, password: data.password }))
			.then(action(res => {
				if (res.data.status === "Success") {
					Toasts.add("success", "Sukces", "Poprawnie zmieniono hasło. Zaloguj się.");
					this.hasToChangePassword = false;
				}
			}))
			.catch(err => {
				Toasts.add("error", "Błąd", "Coś poszło nie tak.");
			})
			.finally(() => {
				this.isLoading = false;
			});
	};

	_logout = () => {
		localStorage.removeItem('token');

		this.isAuthenticated = false;
		this.isAdmin = false;
		this.user = { firstName: "", lastName: "" };
		this.token = null;
	}

	clearTimer = () => {
		clearInterval(this.timerId);
	}
}

decorate(Auth, {
	isAuthenticated: observable,
	isProcessing: observable,
	isLoading: observable,
	name: computed,
	user: observable,
	initial: computed,
	type: computed,
	timerId: observable,
	token: observable,
	hasToChangeFirstPassword: observable,
	hasToChangePassword: observable,
	permissions: observable,
	role: observable,
});

export default new Auth();