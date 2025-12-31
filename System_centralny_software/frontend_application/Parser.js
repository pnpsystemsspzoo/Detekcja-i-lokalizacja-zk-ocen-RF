var Parser = require("binary-parser-encoder").Parser;

var interference_parser = new Parser()
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

var interference_info_parser = new Parser()
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
        type: new Parser()
            .uint16("id"),
        length: "devicesIdLength"
    })
    .saveOffset("parserOffset");

var upload_encoder = new Parser()
    .uint8("frame_ID")
    .uint8("user_ID")
    .uint8("filenameLength")
    .string("filename", {
        encoding: "ascii",
        stripNull: true,
        length: "filenameLength"
    })
    .uint16("devicesLength")
    .array("devices", {
        type: new Parser().uint16(),
        length: "devicesLength"
    })
    .saveOffset("parserOffset");


var login_response_encoder = new Parser()
    .uint8("frame_ID")
    .uint8("user_ID")
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



var logout_response_encoder = new Parser()
    .uint8("frame_ID")
    .int8("user_ID")
    .saveOffset("parserOffset");

var user_activity_status = new Parser()
    .uint8("frame_ID")
    .int8("user_ID")
    .uint8("activity")
    .saveOffset("parserOffset");

var user_force_logout = new Parser()
    .uint8("frame_ID")
    .int8("user_ID")
    .saveOffset("parserOffset");

var backend_disconnected = new Parser()
    .uint8("frame_ID")
    .int8("text")

var base = new Parser()
    .uint8("frame_ID");


var BASE = new Parser()
    .uint8("frame_ID");

var USER_LOGIN_RESPONSE = new Parser()
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
    .uint8("firstPassChangedLength")
    .string("firstPassChanged", {
        encoding: "ascii",
        stripNull: true,
        length: "firstPassChangedLength"
    })
    .uint8("errorCode")
    .saveOffset("parserOffset");


var USER_ACTIVITY_STATUS = new Parser()
    .uint8("frame_ID")
    .int8("user_ID")
    .uint8("activity")
    .saveOffset("parserOffset");

var USER_FORCE_LOGOUT = new Parser()
    .uint8("frame_ID")
    .int8("user_ID")
    .saveOffset("parserOffset");

var BACKEND_DISCONNECTED = new Parser()
    .uint8("frame_ID")
    .int8("text")
    .saveOffset("parserOffset");

var UPDATE_DEVICES_CONFIG_QUERY = new Parser()
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
        type: new Parser().uint16(),
        length: "devicesLength"
    })
    .saveOffset("parserOffset");

var UPDATE_DEVICES_FIRMWARE_QUERY = this.UPDATE_DEVICES_CONFIG_QUERY;



var DEVICE_INFO = new Parser()
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
        type: new Parser()
            .uint8("cardNameLength")
            .string("cardName", {
                encoding: "ascii",
                stripNull: true,
                length: "cardNameLength"
            }),
        length: "cardsLength"
    })
    .saveOffset("parserOffset");




var RADIO_INTERFERENCE = new Parser()
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

var RADIO_INTERFERENCE_INFO = new Parser()
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
    .uint32("field_1")
    .uint32("field_2")
    .uint32("field_3")
    .uint32("field_4")
    .uint16("devicesIdLength")
    .array("devicesId", {
        type: new Parser()
            .uint16("id"),
        length: "devicesIdLength"
    })
    .saveOffset("parserOffset");

var CS_STATUS = new Parser()
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

var DEVICE_FULL_STATUS = new Parser()
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
        type: new Parser()
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
        type: new Parser()
            .uint16("bandwidth")
            .uint16("f0")
            .uint8("type"),
        length: "threatsLength"
    })
    .saveOffset("parserOffset");


var DEVICE_SHORT_STATUS = new Parser()
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
var DEVICE_LIST = new Parser()
    .uint8("frame_ID")
    .uint16("devicesLength")
    .array("devices", {
        type: new Parser()
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
                type: new Parser()
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

var FFT = new Parser()
    .uint8("frame_ID")
    .uint16("id")
    .uint8("channel")
    .uint32("fBegin")
    .uint32("fEnd")
    .uint16("densityLength")
    .array("density", {
        type: new Parser().int8(),
        length: "densityLength"
    })
    .saveOffset("parserOffset");

var ADD_DEVICE = new Parser()
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
        type: new Parser()
            .uint8("cardNameLength")
            .string("cardName", {
                encoding: "ascii",
                stripNull: true,
                length: "cardNameLength"
            }),
        length: "cardsLength"
    })

    .saveOffset("parserOffset");


var DELETE_DEVICE = new Parser()
    .uint8("frame_ID")
    .uint16("id")
    .uint8("status")
    .saveOffset("parserOffset");

var RESET_DEVICE = new Parser()
    .uint8("frame_ID")
    .uint16("id")
    .uint8("status")
    .saveOffset("parserOffset");

// var GET_LOG_RESPONSE = new Parser()
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

var UPDATE = new Parser()
    .uint8("frame_ID")
    .uint8("filenameLength")
    .string("filename", {
        encoding: "ascii",
        stripNull: true,
        length: "filenameLength"
    })
    .uint16("devicesLength")
    .array("devices", {
        type: new Parser()
            .uint16("id")
            .uint8("status"),
        length: "devicesLength"
    })
    .saveOffset("parserOffset");

var CONFIGURATION_LOAD_STATUS = UPDATE;
var FIRMWARE_LOAD_STATUS = UPDATE;

var ALERT = new Parser()
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

var NEW_CONFIGURATION_UPLOADED = new Parser()
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

var NEW_FIRMWARE_UPLOADED = new Parser()
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


const SOCK = {
    RADIO_INTERFERENCE: 1,
    CS_STATUS: 2,
    DEVICE_FULL_STATUS: 3,
    DEVICE_SHORT_STATUS: 4,
    DEVICE_LIST: 5,
    FFT: 6,
    ADD_DEVICE: 7,
    DELETE_DEVICE: 8,
    NEW_CONFIGURATION_UPLOADED: 9,
    CONFIGURATION_LOAD_STATUS: 10,
    NEW_FIRMWARE_UPLOADED: 11,
    FIRMWARE_LOAD_STATUS: 12,
    ALERT: 13,
    RADIO_INTERFERENCE_INFO: 17,
    DEVICE_INFO: 21,

    // messages sent by node backend to client
    USER_LOGIN_RESPONSE: 0x32, //50
    USER_ACTIVITY_STATUS: 0x33, // 51
    USER_FORCE_LOGOUT: 0x34, // 52
    BACKEND_DISCONNECTED: 0x35, // 53


    OPERATOR_INTERFACE_QUERY: 128, // 0x80
    ADMIN_INTERFACE_QUERY: 129, // 0x81
    DEVICES_MANAGEMENT_QUERY: 130, // 0x82
    DEVICE_STATUS_QUERY: 131, // 0x83
    ADD_DEVICE_QUERY: 132, // 0x84
    DELETE_DEVICE_QUERY: 133, // 0x85
    FFT_QUERY: 134, // 0x86
    STOP_DEVICE_STATUS_QUERY: 135, // 0x87
    UPDATE_DEVICES_CONFIG_QUERY: 136, // 0x88
    UPDATE_DEVICES_FIRMWARE_QUERY: 137, // 0x89
    RESET_DEVICE_QUERY: 138,
    GET_DEVICE_LOG_QUERY: 139,

    // messages sent by client to node backend
    USER_LOGIN: 0x40, // 0x40 64
    USER_SEND_TOKEN: 0x41, // 0x41 65

};

const getName = id => {
    return Object.keys(SOCK).find(key => SOCK[key] === id);
};



module.exports = {
    getName,
    SOCK,
    parsers: {
        interference_parser,
        interference_info_parser,
        base,
        upload_encoder,
        login_response_encoder,
        user_activity_status,
        user_force_logout,
        backend_disconnected,
        logout_response_encoder,
        BASE,
        USER_LOGIN_RESPONSE,
        USER_ACTIVITY_STATUS,
        USER_FORCE_LOGOUT,
        BACKEND_DISCONNECTED,
        UPDATE_DEVICES_CONFIG_QUERY,
        UPDATE_DEVICES_FIRMWARE_QUERY,
        DEVICE_INFO,
        RADIO_INTERFERENCE,
        RADIO_INTERFERENCE_INFO,
        CS_STATUS,
        DEVICE_FULL_STATUS,
        DEVICE_SHORT_STATUS,
        DEVICE_LIST,
        FFT,
        ADD_DEVICE,
        DELETE_DEVICE,
        RESET_DEVICE,
        UPDATE,
        CONFIGURATION_LOAD_STATUS,
        FIRMWARE_LOAD_STATUS,
        ALERT,
        NEW_CONFIGURATION_UPLOADED,
        NEW_FIRMWARE_UPLOADED,
    }
};
