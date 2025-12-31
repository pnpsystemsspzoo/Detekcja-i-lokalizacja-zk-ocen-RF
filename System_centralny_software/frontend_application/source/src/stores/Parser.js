import { Parser as _Parser } from "binary-parser-encoder";

/**
 * Definitions how to parse frames to objects (incoming)
 * @class
 */
class Parser {
	static BASE = new _Parser()
		.uint8("frame_ID");

	static USER_LOGIN_RESPONSE = new _Parser()
		.uint8("frame_ID")
		.int8("user_ID")
		.uint8("tokenLength")
		.string("token", {
			encoding: "ascii",
			stripNull: true,
			length: "tokenLength"
		})
		.uint8("firstNameLength")
		.string("firstName", {
			encoding: "ascii",
			stripNull: true,
			length: "firstNameLength"
		})
		.uint8("lastNameLength")
		.string("lastName", {
			encoding: "ascii",
			stripNull: true,
			length: "lastNameLength"
		})
		.uint8("emailLength")
		.string("email", {
			encoding: "ascii",
			stripNull: true,
			length: "emailLength"
		})
		.uint8("isAdminLength")
		.string("isAdmin", {
			encoding: "ascii",
			stripNull: true,
			length: "isAdminLength"
		})
		.uint8("userTypeLength")
		.string("userType", {
			encoding: "ascii",
			stripNull: true,
			length: "userTypeLength"
		})
		.uint8("firstPassChangedLength")
		.string("firstPassChanged", {
			encoding: "ascii",
			stripNull: true,
			length: "firstPassChangedLength"
		})
		.uint8("errorCode")
		.uint8("alertsLength")
		.string("alerts", {
			encoding: "ascii",
			stripNull: true,
			length: "alertsLength"
		})
		.uint8("manageLength")
		.string("manage", {
			encoding: "ascii",
			stripNull: true,
			length: "manageLength"
		})
		.uint8("probesLength")
		.string("probes", {
			encoding: "ascii",
			stripNull: true,
			length: "probesLength"
		})
		.uint8("usersLength")
		.string("users", {
			encoding: "ascii",
			stripNull: true,
			length: "usersLength"
		})
		.uint8("csLength")
		.string("cs", {
			encoding: "ascii",
			stripNull: true,
			length: "csLength"
		})
		.uint8("reportsListLength")
		.string("reportsList", {
			encoding: "ascii",
			stripNull: true,
			length: "reportsListLength"
		})
		.uint8("generateReportLength")
		.string("generateReport", {
			encoding: "ascii",
			stripNull: true,
			length: "generateReportLength"
		})
		.uint8("signaturesListLength")
		.string("signaturesList", {
			encoding: "ascii",
			stripNull: true,
			length: "signaturesListLength"
		})
		.uint8("permissionsLength")
		.string("permissions", {
			encoding: "ascii",
			stripNull: true,
			length: "permissionsLength"
		})
		.uint8("mapLength")
		.string("map", {
			encoding: "ascii",
			stripNull: true,
			length: "mapLength"
		})
		.saveOffset("parserOffset");


	static USER_ACTIVITY_STATUS = new _Parser()
		.uint8("frame_ID")
		.int8("user_ID")
		.uint8("activity")
		.saveOffset("parserOffset");

	static USER_FORCE_LOGOUT = new _Parser()
		.uint8("frame_ID")
		.int8("user_ID")
		.saveOffset("parserOffset");

	static BACKEND_DISCONNECTED = new _Parser()
		.uint8("frame_ID")
		.int8("text")
		.saveOffset("parserOffset");

	static UPDATE_DEVICES_CONFIG_QUERY = new _Parser()
		.uint8("frameID")
		.uint8("userID")
		.uint8("filenameLength")
		.string("filename", {
			encoding: "ascii",
			stripNull: true,
			length: "filenameLength"
		})
		.uint16("devicesLength")
		.array("devices", {
			type: new _Parser().uint16(),
			length: "devicesLength"
		})
		.saveOffset("parserOffset");

	static UPDATE_DEVICES_FIRMWARE_QUERY = this.UPDATE_DEVICES_CONFIG_QUERY;



	static DEVICE_INFO = new _Parser()
		.uint8("frame_ID")
		.uint16("id")
		.uint8("versionLength")
		.string("version", {
			encoding: "ascii",
			stripNull: true,
			length: "versionLength"
		})
		.uint8("ipLength")
		.string("ipAddress", {
			encoding: "ascii",
			stripNull: true,
			length: "ipLength"
		})
		.string("macAddress", {
			encoding: "ascii",
			stripNull: true,
			length: 18,
		})
		.doublebe("longitude")
		.doublebe("latitude")
		.uint16("height")
		.uint8("connected")
		.uint8("hardwareHealth")
		.uint8("softwareHealth")
		// .bit4("_")
		// .bit2("softwareHealth") // softwareHealth
		// .bit2("health") // hardwareHealth
		.uint16("dataPort")
		.uint16("controlPort")
		.uint8("creationDateLength")
		.string("creationDate", {
			encoding: "ascii",
			stripNull: true,
			length: "creationDateLength"
		})
		.uint8("expirationDateLength")
		.string("expirationDate", {
			encoding: "ascii",
			stripNull: true,
			length: "expirationDateLength"
		})
		.uint8("codeLength")
		.string("code", {
			encoding: "ascii",
			stripNull: true,
			length: "codeLength"
		})
		.uint8("whoLength")
		.string("who", {
			encoding: "ascii",
			stripNull: true,
			length: "whoLength"
		})
		.uint8("firstMess")
		.uint8("cardsLength")
		.array("cards", {
			type: new _Parser()
				.uint8("cardNameLength")
				.string("cardName", {
					encoding: "ascii",
					stripNull: true,
					length: "cardNameLength"
				}),
			length: "cardsLength"
		})
		.saveOffset("parserOffset");




	static RADIO_INTERFERENCE = new _Parser()
		.uint8("frame_ID")
		.uint32("id")
		.doublebe("longitude")
		.doublebe("latitude")
		.uint16("height")
		.uint16("range")
		.uint16("heading")
		.uint16("speed")
		.floatbe("location_variance")
		.doublebe('unix_time')
		.floatbe('time_var')
		.saveOffset("parserOffset");

	static RADIO_INTERFERENCE_INFO = new _Parser()
		.uint8("frame_ID")
		.uint32("signal_id")
		.doublebe('unix_time')
		.floatbe('time_var')
		.uint8("signal_type")
		.uint32("signal_period")
		.uint32("f0")
		.floatbe("f0_var")
		.uint16("bandwidth")
		.floatbe("bandwidth_var")
		.int16("J")
		.int16("J_var")
		.uint32("field_1") // chirp_sweep or pulse_on_1
		.uint32("field_2") // chirp_rise_time or pulse_off_2
		.uint32("field_3") // chirp_return_time or pulse_on_2
		.uint32("field_4") // chirp_reserved or pulse_off_2
		.uint16("devicesIdLength")
		.array("devicesId", {
			type: new _Parser()
				.uint16("id"),
			length: "devicesIdLength"
		})
		.saveOffset("parserOffset");

	static CS_STATUS = new _Parser()
		.uint8("frame_ID")
		.uint8("hardwareHealth")
		.uint8("softwareHealth")
		// .bit4("_")
		// .bit2("softwareHealth") // softwareHealth
		// .bit2("health") // hardwareHealth
		.uint8("communication")
		.uint8("cpuUsage")
		.uint8("ramUsage")
		.uint8("hddUsage")
		.uint16("tasksNumber")
		.saveOffset("parserOffset");

	static DEVICE_FULL_STATUS = new _Parser()
		.uint8("frame_ID")
		.uint16("id")
		.uint8("hardwareHealth")
		.uint8("softwareHealth")
		// .bit4("_")
		// .bit2("softwareHealth") // softwareHealth
		// .bit2("health") // hardwareHealth
		.uint16("motherboardVoltage")
		.uint16("motherboardCurrent")
		.uint8("cardsLength")
		.array("cards", {
			type: new _Parser()
				.uint16("cardVoltage")
				.uint16("cardCurrent"),
			length: "cardsLength"
		})
		.bit6("_")
		.bit2("card4")
		.bit2("card3")
		.bit2("card2")
		.bit2("card1")
		.bit2("card0")
		.uint32("alerts")
		.uint8("radioInterference")
		// .uint8("communication")
		.uint8("connected")
		.uint8("cpuUsage")
		.uint8("ramUsage")
		.uint8("hddUsage")
		.uint32("txTransferRate")
		.uint32("rxTransferRate")
		.uint16("tasksNumber")
		.uint8("synchronization")
		.uint8("timeStd")
		.string("datetime", {
			encoding: "ascii",
			length: 25,
			stripNull: true
		})
		.uint8("threatsLength")
		.array("threats", {
			type: new _Parser()
				.uint16("bandwidth")
				.uint16("f0")
				.uint8("type"),
			length: "threatsLength"
		})
		.saveOffset("parserOffset");


	static DEVICE_SHORT_STATUS = new _Parser()
		.uint8("frame_ID")
		.uint16("id")
		.uint8("hardwareHealth")
		.uint8("softwareHealth")
		// .bit4("_")
		// .bit2("softwareHealth") // softwareHealth
		// .bit2("health") // hardwareHealth
		.uint8("communication")
		.uint8("interfered", {
			formatter: value => {
				const array = Number(value).toString(2).split("").reverse();
				// const systems = ["ILS 110", "ILS 330", "TETRA", "GNSS"];

				return array.map(i => parseInt(i));
				// return array.map((i, k) => i === "1" ? systems[k] : "").filter(i => i);
			}
		}).saveOffset("parserOffset");


	// frame disabled
	static DEVICE_LIST = new _Parser()
		.uint8("frame_ID")
		.uint16("devicesLength")
		.array("devices", {
			type: new _Parser()
				.uint16("id")
				.uint8("versionLength")
				.string("version", {
					encoding: "ascii",
					stripNull: true,
					length: "versionLength"
				})
				.uint8("ipLength")
				.string("ipAddress", {
					encoding: "ascii",
					stripNull: true,
					length: "ipLength"
				})
				.string("macAddress", {
					encoding: "ascii",
					stripNull: true,
					length: 18,
				})
				.doublebe("longitude")
				.doublebe("latitude")
				.uint16("height")
				.uint8("connected")
				// .bit4("_")
				// .bit2("softwareHealth") // softwareHealth
				// .bit2("health") // hardwareHealth
				.uint16("dataPort")
				.uint16("controlPort")
				.uint8("creationDateLength")
				.string("creationDate", {
					encoding: "ascii",
					stripNull: true,
					length: "creationDateLength"
				})
				.uint8("expirationDateLength")
				.string("expirationDate", {
					encoding: "ascii",
					stripNull: true,
					length: "expirationDateLength"
				})
				.uint8("codeLength")
				.string("code", {
					encoding: "ascii",
					stripNull: true,
					length: "codeLength"
				})
				.uint8("whoLength")
				.string("who", {
					encoding: "ascii",
					stripNull: true,
					length: "whoLength"
				})
				.uint8("firstMess")
				.uint8("cardsLength")
				.array("cards", {
					type: new _Parser()
						.uint8("cardNameLength")
						.string("cardName", {
							encoding: "ascii",
							stripNull: true,
							length: "cardNameLength"
						}),
					length: "cardsLength"
				}),
			length: "devicesLength"
		})
		.saveOffset("parserOffset");

	static FFT = new _Parser()
		.uint8("frame_ID")
		.uint16("id")
		.uint8("channel")
		.uint32("fBegin")
		.uint32("fEnd")
		.uint16("densityLength")
		.array("density", {
			type: new _Parser().int8(),
			length: "densityLength"
		})
		.saveOffset("parserOffset");

	static ADD_DEVICE = new _Parser()
		.uint8("frame_ID")
		.uint8("status")
		.uint16("id")
		.uint8("versionLength")
		.string("version", {
			encoding: "ascii",
			stripNull: true,
			length: "versionLength"
		})
		.uint8("ipLength")
		.string("ipAddress", {
			encoding: "ascii",
			stripNull: true,
			length: "ipLength"
		})
		.string("macAddress", {
			encoding: "ascii",
			stripNull: true,
			length: 18,
		})
		.doublebe("longitude")
		.doublebe("latitude")
		.uint16("height")
		.uint8("connected")
		.uint8("hardwareHealth")
		.uint8("softwareHealth")
		// .bit4("_")
		// .bit2("softwareHealth") // softwareHealth
		// .bit2("health") // hardwareHealth
		.uint16("dataPort")
		.uint16("controlPort")
		.uint8("creationDateLength")
		.string("creationDate", {
			encoding: "ascii",
			stripNull: true,
			length: "creationDateLength"
		})
		.uint8("expirationDateLength")
		.string("expirationDate", {
			encoding: "ascii",
			stripNull: true,
			length: "expirationDateLength"
		})
		.uint8("codeLength")
		.string("code", {
			encoding: "ascii",
			stripNull: true,
			length: "codeLength"
		})
		.uint8("whoLength")
		.string("who", {
			encoding: "ascii",
			stripNull: true,
			length: "whoLength"
		})
		.uint8("firstMess")
		.uint8("cardsLength")
		.array("cards", {
			type: new _Parser()
				.uint8("cardNameLength")
				.string("cardName", {
					encoding: "ascii",
					stripNull: true,
					length: "cardNameLength"
				}),
			length: "cardsLength"
		})

		.saveOffset("parserOffset");


	static DELETE_DEVICE = new _Parser()
		.uint8("frame_ID")
		.uint16("id")
		.uint8("status")
		.saveOffset("parserOffset");

	static RESET_DEVICE = new _Parser()
		.uint8("frame_ID")
		.uint16("id")
		.uint8("status")
		.saveOffset("parserOffset");

	// static GET_LOG_RESPONSE = new _Parser()
	// 	.uint8("frame_ID")
	// 	.uint8("status")
	// 	.uint16("id")
	// 	.uint8("logFilenameLength")
	// 	.string("logFilename", {
	// 		encoding: "ascii",
	// 		stripNull: true,
	// 		length: "logFilenameLength"
	// 	})
	// 	.saveOffset("parserOffset");

	static UPDATE = new _Parser()
		.uint8("frame_ID")
		.uint8("filenameLength")
		.string("filename", {
			encoding: "ascii",
			stripNull: true,
			length: "filenameLength"
		})
		.uint16("devicesLength")
		.array("devices", {
			type: new _Parser()
				.uint16("id")
				.uint8("status"),
			length: "devicesLength"
		})
		.saveOffset("parserOffset");

	static CONFIGURATION_LOAD_STATUS = Parser.UPDATE;
	static FIRMWARE_LOAD_STATUS = Parser.UPDATE;

	static ALERT = new _Parser()
		.uint8("frame_ID")
		.uint16("alert_ID")
		.uint8("timestampLength")
		.string("timestamp", {
			encoding: "ascii",
			stripNull: true,
			length: "timestampLength"
		})
		.uint8("messageLength")
		.string("message", {
			encoding: "ascii",
			stripNull: true,
			length: "messageLength"
		})
		.saveOffset("parserOffset");

	static NEW_CONFIGURATION_UPLOADED = new _Parser()
		.uint8("frame_ID")
		.uint8("user_ID")
		.uint8("status")
		.uint8("filenameLength")
		.string("filename", {
			encoding: "ascii",
			stripNull: true,
			length: "filenameLength"
		})
		.saveOffset("parserOffset");

	static NEW_FIRMWARE_UPLOADED = new _Parser()
		.uint8("frame_ID")
		.uint8("user_ID")
		.uint8("status")
		.uint8("filenameLength")
		.string("filename", {
			encoding: "ascii",
			stripNull: true,
			length: "filenameLength"
		})
		.saveOffset("parserOffset");



}

export default Parser;
