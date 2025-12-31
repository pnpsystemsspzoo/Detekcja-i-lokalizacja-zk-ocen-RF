import convert from "color-convert";

export const stringToHslColor = (str, s, l) => {
	var hash = 0;
	for (var i = 0; i < str.length; i++) {
		hash = str.charCodeAt(i) + ((hash << 5) - hash);
	}

	var h = hash % 360;
	return [h, s, l];
};


export const getContrast = hexcolor => {
	var r = parseInt(hexcolor.substr(0, 2), 16);
	var g = parseInt(hexcolor.substr(2, 2), 16);
	var b = parseInt(hexcolor.substr(4, 2), 16);
	var yiq = ((r * 299) + (g * 587) + (b * 114)) / 1000;
	return (yiq >= 128) ? "black" : "white";
};

export const stringToColor = str => {
	const [sh, ss, sl] = stringToHslColor(str, 33, 66);
	return `hsl(${sh}, ${ss}%, ${sl}%)`;
};

export const nameToGradient = name => {
	const [sh, ss, sl] = stringToHslColor(name, 33, 66);
	const [eh, es, el] = stringToHslColor(name, 33, 75);

	const startColor = `hsl(${sh}, ${ss}%, ${sl}%)`;
	const endColor = `hsl(${eh}, ${es}%, ${el}%)`;

	return {
		style: {
			background: `${startColor} linear-gradient(45deg, ${startColor}, ${endColor})`,
			color: getContrast("#" + convert.hsl.hex(sh, ss, sl))
		}
	};
};

export const userParse = user => {
	user.initial = (user.firstName[0] + "" + user.lastName[0]).toUpperCase();
	user.name = user.firstName + " " + user.lastName;

	return user;
};


export const formatSize = (size, scale = ["B", "KB", "MB", "GB"]) => {
	if (isNaN(size))
		return "";

	if (size < 1024)
		return size.toFixed(0) + scale[0];

	size = size / 1024;
	if (size < 1024)
		return size.toFixed(0) + scale[1];

	size = size / 1024;
	if (size < 1024)
		return size.toFixed(0) + scale[2];

	size = size / 1024;
	return size.toFixed(0) + scale[3];
};


export const timeConversion = miliseconds => {
	var seconds = (miliseconds / 1000).toFixed(1);

	var minutes = (miliseconds / (1000 * 60)).toFixed(1);

	var hours = (miliseconds / (1000 * 60 * 60)).toFixed(1);

	var days = (miliseconds / (1000 * 60 * 60 * 24)).toFixed(1);

	if (miliseconds < 1000) {
		return miliseconds + "ms";
	} else if (seconds < 60) {
		return seconds + "s";
	} else if (minutes < 60) {
		return minutes + "m";
	} else if (hours < 24) {
		return hours + "h";
	} else {
		return days + "d"
	}

	// var minutes = Math.floor(miliseconds / 60000);
	// var seconds = ((miliseconds % 60000) / 1000).toFixed(0);
	// return `${minutes}:${(seconds < 10 ? "0" : "")}${seconds}`;

};

const toDegreesMinutesAndSeconds = (coordinate) => {
	var absolute = Math.abs(coordinate);
	var degrees = Math.floor(absolute);
	var minutesNotTruncated = (absolute - degrees) * 60;
	var minutes = Math.floor(minutesNotTruncated);
	var seconds = Math.floor((minutesNotTruncated - minutes) * 60);

	return degrees + "°" + minutes + "'" + seconds + "''";
};


export const degreesToMinutes = (lat, lng) => {
	var latitude = toDegreesMinutesAndSeconds(lat);
	var latitudeCardinal = lat >= 0 ? "N" : "S";

	var longitude = toDegreesMinutesAndSeconds(lng);
	var longitudeCardinal = lng >= 0 ? "E" : "W";

	return latitude + " " + latitudeCardinal + ",  " + longitude + " " + longitudeCardinal;
};