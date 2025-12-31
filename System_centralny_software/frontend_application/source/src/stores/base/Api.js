import Request from "../../utils/Request";

/**
 * Lost of API endpoints
 * @class
 */
class Api {
	user = {
		add: data => Request.call("post", "user/add", data),
		edit: (data) => Request.call("post", "user/edit", data),
		block: (data) => Request.call("get", "user/block", data),
		unblock: (data) => Request.call("get", "user/unblock", data),
		delete: data => Request.call("post", "user/delete", data),

		logs: {
			get: (id) => Request.call("get", "user/logs", id),
			clear: (data) => Request.call("post", "user/logs/clear", data),
		},

		session: {
			get_time: (id) => Request.call("get", "user/session/time", id),
			close: (data) => Request.call("get", "user/session/close", data),
		},

		permissions: {
			get: (id) => Request.call("get", "user/permissions", id),
			save: (data) => Request.call("post", "user/permissions/save", data),
		},
	};

	users = {
		get: () => Request.call("get", "users"),
	}

	probe = {
		logs: (id) => Request.call("get", "probe/logs", id),
		history: (id) => Request.call("get", "probe/history", id),

		config: {
			get: () => Request.call("get", "share/config/config_list.json"),
			upload: (data) => Request.call("post", "probe/config/upload", data),
		},

		firmware: {
			get: () => Request.call("get", "share/firmware/firmware_list.json"),
			upload: (data) => Request.call("post", "probe/firmware/upload", data),
		}
	};

	centralsystem = {
		logs: {
			get: () => Request.call("get", "sc/logs"),
			clear: () => Request.call("post", "sc/logs/clear"),
		}
	};

	signatures = {
		get: () => Request.call("get", "signatures"),
	};

	account = {
		password: {
			set: (data) => Request.call("post", "account/password/set", data),
			set_first: (data) => Request.call("post", "account/password/set_first", data),
		}
	};

	report = {
		download: (data) => Request.call("get", "report/download", data, "arraybuffer"),
		generate_and_download: (data) => Request.call("get", "report/generate_and_download", data, "arraybuffer"),

		basic: {
			generate_table: (data) => Request.call("post", "report/basic/generate_table", data),
			generate_screen: (data) => Request.call("post", "report/basic/generate_screen", data, "arraybuffer"),
			get_table: () => Request.call("get", "report/basic/table"),
		},

		extended: {
			generate_table: (data) => Request.call("post", "report/extended/generate_table", data),
			generate_screen: (data) => Request.call("post", "report/extended/generate_screen", data, "arraybuffer"),
			get_table: () => Request.call("get", "report/extended/table"),
		},

	};

	reports = {
		get: () => Request.call("get", "reports"),
	};






}

export default new Api();