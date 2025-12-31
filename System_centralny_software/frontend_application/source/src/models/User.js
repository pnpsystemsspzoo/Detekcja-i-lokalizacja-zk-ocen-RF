import { decorate, observable, computed } from "mobx";
import Model from "../stores/base/Model";
import moment from "moment";
import "moment/locale/pl";

class User extends Model {
	/**
	 * Reference to the registry of users
	 * @type {Users}
	 */
	store;

	/**
	 * @type {number}
	 */
	id;

	/**
	 * @type {string}
	 */
	firstName;

	/**
	 * @type {string}
	 */
	lastName;

	/**
	 * @type {string}
	 */
	email;

	/**
	 * @type {string}
	 */
	role;

	/**
	 * @type {boolean}
	 */
	is_logged;

	/**
	 * @type {boolean}
	 */
	blocked;

	/**
	 * Initial field values in user created form
	 * @type {{password: string, lastName: string, password_repeat: string, firstName: string, email: string}}
	 */
	initialValues = {
		firstName: "",
		lastName: "",
		email: "",
		password: "",
		password_repeat: ""
	};

	/**
	 * Create user instance
	 * @param store {Users}
	 * @param input {Object}
	 */
	constructor(store, input) {
		super();

		this.store = store;

		this.id = input.id;

		var date = moment.utc(input.lastUpdate).format('YYYY-MM-DD HH:mm:ss');
		var stillUtc = moment.utc(date).toDate();
		this.lastUpdate = moment(stillUtc).local().format('DD-MM-YYYY HH:mm:ss');

		this.firstName = input.firstName;
		this.lastName = input.lastName;
		this.email = input.email;
		this.role = input.role;
		this.is_logged = input.is_logged;
		this.blocked = input.blocked;
	}

	/**
	 * Returns user name
	 * @returns {string}
	 */
	get name() {
		if (this.firstName) {
			return this.firstName + " " + this.lastName;
		}
		return "";
	}

	/**
	 * Returns use first letter of firstName and lastName
	 * @returns {string}
	 */
	get initial() {
		if (this.firstName) {
			return this.firstName[0] + "" + this.lastName[0];
		}
		return "";
	}

	update = (input) => {
		this.firstName = input.user.firstname;
		this.lastName = input.user.lastname;
		this.email = input.user.email;
	}

	updateStatus = (activity) => {
		this.is_logged = activity === 1 ? true : false;
	}

	updateBlocked = (blocked) => {
		this.blocked = blocked;
	}
}


decorate(User, {
	firstName: observable,
	lastName: observable,
	email: observable,
	name: computed,
	initial: computed,
	is_logged: observable,
	blocked: observable,
});

export default User;