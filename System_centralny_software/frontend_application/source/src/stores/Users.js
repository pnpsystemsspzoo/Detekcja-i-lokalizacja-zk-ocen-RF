import Api from "./base/Api";
import Registry from "./base/Registry";
import { computed, decorate, observable } from "mobx";
import User from "../models/User";
import Toasts from "./Toasts";

class Users extends Registry {

	initialValues = {
		useradd: {
			firstName: "",
			lastName: "",
			type: "",
			login: "",
			email: "",
			password: "",
			password_repeat: ""
		},
	};

	/**
	 * Current viewed user id
	 * @type {User}
	 */
	current;

	/**
	 * Current edited user object
	 * @type {User}
	 */
	userToEdit = { firstName: "", lastName: "", email: "" };

	/**
	 * Forms processing states
	 * @type {{add: boolean,  delete: boolean, edit: boolean, closeSession: boolean }}
	 */
	processing = {
		add: false,
		delete: false,
		edit: false,
		closeSession: false,
		block: false,
		unblock: false,
	};

	/**
	 * Form modal opened state
	 * @type {{add: boolean, delete: boolean, edit: boolean }}
	 */
	modals = {
		add: false,
		delete: false,
		edit: false,
		closeSession: false,
		block: false,
		unblock: false,
	};

	/**
	 * Toggle modal
	 * @param modal {string} name of modal
	 */
	modalToggle = modal => {
		if (this.modals[modal] !== undefined) {
			this.modals[modal] = !this.modals[modal];
		}
	};

	onSelectEvent = (event) => {
		this.columns.select = true;
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

	setUserToEdit = (user) => {
		this.userToEdit = { ...user };
	}

	editUser = () => {
		let user = this.registry.get(this.userToEdit.id);

		Api.user.edit({ id: this.userToEdit.id, firstname: this.userToEdit.firstName, lastname: this.userToEdit.lastName, email: this.userToEdit.email })
			.then(res => {
				if (res.data.id !== -1) {
					this.modals.edit = false;
					user.update(res.data);
					Toasts.add("success", "Sukces", "Poprawnie edytowano użytkownika.");
				}
				else Toasts.add("error", "Błąd", "Błąd edytowania użytkownika");
			})
			.catch(err => {
				Toasts.add("error", "Błąd", "Błąd edytowania użytkownika");
				console.log('catch', err)
			})
			.finally(() => {
				this.isLoading = false;
			});
	}

	/**
	 * Clear selections on list
	 */
	clearSelected = () => {
		this.selected = observable.map();
	};

	setCurrent = id => {
		this.current = parseInt(id);
		return this.getCurrent;
	};

	/**
	 * Select user for bulk action
	 * @param event {Event}
	 */
	select = user => {
		const checked = this.selected.has(user.id);
		checked ? this.selected.delete(user.id) : this.selected.set(user.id, user);
	};


	/**
	 * Get current viewed probe
	 * @returns {User|null}
	 */
	get getCurrent() {
		if (this.registry.has(this.current)) {
			return this.registry.get(this.current);
		} else {
			return null;
		}
	}

	/**
	 * @override
	 * @returns {Q.Promise<any> | *}
	 */
	get = () => {
		this.isLoading = true;
		Api.users.get()
			.then(res => {
				res.data.forEach(user => {
					const newUser = new User(this, user);
					this.registry.set(newUser.id, newUser);
				});

				this.totalPages = Math.ceil(this.registry.size / this.perPage);
			})
			.catch(err => {
				Toasts.add("error", "Błąd", "Brak użytkowników");
			})
			.finally(() => {
				this.isLoading = false;
			});
	};

	/**
	 * Create new users registry from API request
	 * @param user {Object[]}
	 */
	add = user => {
		this.isLoading = true;
		Api.user.add(user)
			.then(res => {
				if (res.data.id !== -1) {
					this.modals.add = false;
					const newUser = new User(this, res.data);
					this.registry.set(newUser.id, newUser);
					this.totalPages = Math.ceil(this.registry.size / this.perPage);
					this.isLoading = false;
					Toasts.add("success", "Sukces", "Poprawnie dodano użytkownika.");
				}
				else {
					Toasts.add("error", "Błąd", "Taki użytkownik już istnieje!");
					// this.modals.add = false;
					this.isLoading = false;
				}
			})
			.catch(res => {
				Toasts.add("error", "Błąd", "Błąd dodawania użytkownika");
			})
			.finally(() => {
				// this.isLoading = false;
			});
	};

	/**
	 * Create new users registry from API request
	 * @param users {String[]} [id1, id2]
	 */
	delete = () => {
		this.modals.delete = false;
		this.isLoading = false;
		this.processing.delete = false;
		const users = this.selectedItems.map((item) => { return item.id });

		Api.user.delete(users)
			.then(res => {
				if (res.data.status === 'success') {
					Toasts.add("success", "Sukces", "Usuwanie użytkownika(ów) zakończone sukcesem.");
					res.data.users.forEach((userid) => {
						this.registry.delete(userid);
					})
					this.totalPages = Math.ceil(this.registry.size / this.perPage);
				}
				else Toasts.add("error", "Błąd", "Błąd usuwania użytkownika");
			})
			.catch(err => {
				Toasts.add("error", "Błąd", "Błąd usuwania użytkownika");
			})
	};

	userActivityStatus = (input) => {
		const { activity, user_ID } = input;
		let user = this.registry.get(user_ID);
		if (user) {
			user.updateStatus(activity);
		}
	}

	savePermissions = (userid, permissions) => {
		permissions.alerts = permissions.alerts === true ? "yes" : "no";
		permissions.manage = permissions.manage === true ? "yes" : "no";
		permissions.probes = permissions.probes === true ? "yes" : "no";
		permissions.users = permissions.users === true ? "yes" : "no";
		permissions.cs = permissions.cs === true ? "yes" : "no";
		permissions.reports_list = permissions.reports_list === true ? "yes" : "no";
		permissions.generate_report = permissions.generate_report === true ? "yes" : "no";
		permissions.signatures_list = permissions.signatures_list === true ? "yes" : "no";
		permissions.permissions = permissions.permissions === true ? "yes" : "no";
		permissions.map = permissions.map === true ? "yes" : "no";

		Api.user.permissions.save({ userid, permissions })
			.then(res => {
				if (res.data.status !== 'error') {
					Toasts.add("success", "Sukces", "Poprawnie zmieniono uprawnienia użytkownika.");
				}
				else Toasts.add("error", "Błąd", "Nie udało się zmienić uprawnień użytkownika");
			})
			.catch(err => {
				console.log('err permissions', err)
				Toasts.add("error", "Błąd", "Nie udało się zmienić uprawnień użytkownika");
			})
	}

	/**
	 * Create new users registry from API request
	 * @param devices {Object[]}
	 */
	closeSession = (user) => {
		this.modals.closeSession = false;
		this.isLoading = false;
		this.processing.closeSession = false;

		Api.user.session.close({ id: user.id })
			.then(res => {
				if (res.data.status !== 'error') {
					Toasts.add("success", "Sukces", "Poprawnie zakończono sesję użytkownika.");
				}
				else {
					Toasts.add("error", "Błąd", "Nie udało się zakończyć sesji użytkownika");
				}
			})
			.catch(err => {
				Toasts.add("error", "Błąd", "Nie udało się zakończyć sesji użytkownika");
				console.log('err', err);
			})
	};

	/**
	 * Block user
	 * @param user {Object}
	 */
	block = (user) => {
		this.modals.block = false;
		this.isLoading = false;
		this.processing.block = false;

		Api.user.block({ id: user.id })
			.then(res => {
				if (res.data.status !== 'error') {
					Toasts.add("success", "Sukces", "Poprawnie zablokowano użytkownika.");
					let changeduser = this.registry.get(user.id);
					if (changeduser) changeduser.updateBlocked('yes');
				}
				else Toasts.add("error", "Błąd", "Nie udało się zablokować użytkownika");
			})
			.catch(err => {
				Toasts.add("error", "Błąd", "Nie udało się zablokować użytkownika");
				console.log('err', err);
			})
	};

	/**
	 * Unblock user
	 * @param user {Object}
	 */
	unblock = (user) => {
		this.modals.unblock = false;
		this.isLoading = false;
		this.processing.unblock = false;

		Api.user.unblock({ id: user.id })
			.then(res => {
				if (res.data.status !== 'error') {
					Toasts.add("success", "Sukces", "Poprawnie odblokowano użytkownika.");
					let changeduser = this.registry.get(user.id);
					if (changeduser) changeduser.updateBlocked('no');
				}
				else Toasts.add("error", "Błąd", "Nie udało się odblokować użytkownika");
			})
			.catch(err => {
				Toasts.add("error", "Błąd", "Nie udało się odblokować użytkownika");
				console.log('err', err);
			})
	};
}

decorate(Users, {
	modals: observable,
	userToEdit: observable,
	processing: observable,
	current: observable,
	getCurrent: computed,
	columns: observable,
});

export default new Users();