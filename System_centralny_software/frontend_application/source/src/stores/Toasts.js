import {computed, decorate, observable} from "mobx/lib/mobx";

/**
 * Stores toasts
 * @class
 */
class Toasts {
	/**
	 * Registry of toasts
	 * @type {ObservableMap<number, Object>}
	 */
	registry = observable.map();
	
	/**
	 * Get all existing toasts
	 * @returns {V[]}
	 */
	get list() {
		return [...this.registry.values()];
	}
	
	/**
	 * Add toast
	 * @param type {string} success|warning|error
	 * @param title {string} Title of toast
	 * @param content {string|null} Content of toast
	 */
	add = (type, title, content = null) => {
		const id = Date.now();
		this.registry.set(id, {
			id,
			type,
			show: true,
			title: title,
			content,
		});
	};
	
	/**
	 * Hiding toast by ID
	 * @param id {number}
	 */
	hide = id => {
		if (this.registry.has(id)) {
			this.registry.get(id).show = false;
		}
	};
	
	
	/**
	 * Remove toast
	 * @param id {number}
	 */
	remove = id => {
		if (this.registry.has(id)) {
			this.registry.delete(id);
		}
	};
}

decorate(Toasts, {
	list: computed
});

export default new Toasts();