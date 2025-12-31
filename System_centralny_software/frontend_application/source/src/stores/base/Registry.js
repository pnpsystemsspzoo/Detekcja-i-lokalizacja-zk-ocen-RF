import { observable, decorate, action, computed } from "mobx/lib/mobx";
import { PER_PAGE } from "../../utils/Constants";

/**
 * Stores instances of objects
 * @class
 */
class Registry {
	/**
	 * Store is loading
	 * @type {boolean}
	 */
	isLoading = true;

	/**
	 * Stores instances of objects
	 * @type {ObservableMap<number, any>}
	 */
	registry = observable.map();

	/**
	 * Stores instances of objects selected items
	 * @type {ObservableMap<number, any>}
	 */
	selected = observable.map();

	/**
	 * Current page
	 * @type {number}
	 */
	page = 1;

	/**
	 * Per page
	 * @type {number}
	 */
	perPage = PER_PAGE;

	/**
	 * Total pages
	 * @type {null}
	 */
	totalPages = null;

	/**
	 * Current sort definitions
	 * @type {{field: string, column: string|null, order: asc|desc}}
	 */
	sort = {
		column: null,
		order: "asc",
		field: ""
	};

	/**
	 * Object of filters
	 * @type {Object}
	 */
	filters = {};

	/**
	 * Object of columns in table
	 * @type {Object[]}
	 */
	columns = [];

	/**
	 * Return sorted and filtered elements of registry
	 * @returns {Object[]}
	 */
	get items() {
		let values = [...this.registry.values()];

		if (this.sort.column !== null) {
			values.sort((a, b) => {
				return a[this.sort.field] > b[this.sort.field] ? 1 : a[this.sort.field] < b[this.sort.field] ? -1 : 0;
			});
		}

		if (this.sort.order === "desc")
			values.reverse();

		Object.keys(this.filters).forEach(key => {
			if (this.filters[key].value) {
				values = values.filter(this.filters[key].function);
			}
		});

		return values.slice((this.page - 1) * this.perPage, this.page * this.perPage)
	}

	get selectedItems() {
		let values = [...this.selected.values()];
		return values;
	}

	/**
	 * Return ready state
	 * @returns {boolean}
	 */
	get ready() {
		return this.totalPages !== null && this.registry.size > 0;
	}

	/**
	 * checks if the store is empty
	 * @returns {boolean}
	 */
	get empty() {
		// return this.registry.size <= 0 && !this.isLoading;
		return this.registry.size <= 0;
	}

	/**
	 * Set sort
	 * @param index
	 * @param field
	 */
	setSort = (index, field) => {
		if (this.sort.column === index) {
			this.sort.order = this.sort.order === "asc" ? "desc" : "asc";
		} else {
			this.sort.field = index;
			this.sort.order = "desc";
		}

		this.sort.field = field;
		this.sort.column = index;
		this.getFirstPage();
	};

	/**
	 * Set filter
	 * @param values
	 */
	setFilters = values => {
		// filters are in Probes.js line 138
		// this.selected = new Set();
		this.selected = observable.map();

		Object.keys(values).forEach(key => {
			if (values[key]) { // if (values[id]) - tak
				this.filters[key].value = values[key];
			} else if (!values[key]) {
				this.filters[key].value = null;
			}
		});
	};

	/**
	 * Return index of record
	 * @param index {number}
	 * @returns {number}
	 */
	getIndex = index => {
		return index + 1 + ((this.page - 1) * this.perPage);
	};

	setPerPage = (perPage, reload = true) => {
		this.perPage = perPage;
		this.totalPages = Math.ceil(this.registry.size / this.perPage);
		reload && this.getFirstPage();
	};

	getFirstPage = () => {
		return this.getPage(1);
	};

	/**
	 * Return data of selected data
	 * @param page
	 * @returns {*}
	 */
	getPage = page => {
		this.page = page;
		// return this.getActualPage();
		return this.page;
	};

	/**
	 * Return pagination settings
	 * @returns {{current: number, total: number, changePages: Registry.setPerPage, pages: number|null, perPage: number}}
	 */
	get pagination() {
		return {
			current: this.page,
			pages: this.totalPages,
			total: this.registry.size,
			perPage: this.perPage,
			changePages: this.setPerPage
		}
	}
}

decorate(Registry, {
	isLoading: observable,
	isProcessing: observable,

	items: computed,
	registry: observable,
	selected: observable,

	pagination: computed,
	page: observable,
	perPage: observable,
	totalPages: observable,
	getActualPage: action,
	getPage: action,
	getParams: action,

	sort: observable,
	setSort: action,
	setFilter: action,

	ready: computed,
	empty: computed,

	filters: observable
});

export default Registry;