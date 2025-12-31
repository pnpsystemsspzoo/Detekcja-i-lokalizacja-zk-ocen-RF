import { include } from "named-urls";

/**
 * Routing definitions
 */
export default {
	home: "/",
	user: include("/user/:id", {
		base: "",
		// edit: "edit",
	}),
	users: include("/users", {
		base: "",
		// create: "create",
	}),
	probe: include("/probe/:id", {
		base: "",
	}),
	probes: include("/probes", {
		base: "",
	}),
	auth: include("auth", {
		login: "/",
		recover: "/recover-password",
	}),
	alerts: include("/alerts", {
		base: "",
	}),
	reports: include("/reports", {
		base: "",
	}),
	signatures: include("/signatures", {
		base: "",
	}),
	centralsystem: include("/central-system", {
		base: "",
	}),
	centralsystembasicreport: include("/central-system-basic-report", {
		base: "",
	}),
	centralsystemextendedreport: include("/central-system-extended-report", {
		base: "",
	})
};