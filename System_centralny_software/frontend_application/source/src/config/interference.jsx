let config = null;

if (localStorage.getItem("mapConfig") !== null) {
	config = JSON.parse(localStorage.getItem("mapConfig"));
}

export const hoverStyle = {
	marker: {
		color: "green",
		fillOpacity: .6,
	},
	path: {
		color: "green",
		opacity: 1
	}
};

export const extinguishedStyles = {
	marker: {
		fillOpacity: config ? config.opacity : 0.1,
		color: "black",
	},
	path: {
		opacity: config ? config.opacity + config.opacity * 2 : 0.2,
		color: "black"
	}
};

export const defaultStyles = {
	marker: {
		fillOpacity: 0.5,
	},
	path: {
		opacity: 1,
	}
};