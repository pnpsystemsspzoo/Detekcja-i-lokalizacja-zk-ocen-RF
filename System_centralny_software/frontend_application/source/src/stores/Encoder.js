import { Parser } from "binary-parser-encoder";

/**
 * Definitions how to encode frames from objects (outgoing)
 * @class
 */
class Encoder {

	static BASE = new Parser()
		.uint8("frameID")
		.uint8("userID");

	static OPERATOR_INTERFACE = Encoder.BASE;
	static ADMIN_INTERFACE = Encoder.BASE;
	static DEVICES_MANAGEMENT = Encoder.BASE;

	static DEVICE_STATUS_QUERY = new Parser()
		.uint8("frameID")
		.uint8("userID")
		.uint16("id");

	static DEVICES_STATUS = new Parser()
		.uint8("frameID")
		.uint8("userID")
		.uint16("id");

	static ADD_DEVICE = new Parser()
		.uint8("frameID")
		.uint8("userID")
		.uint16("licenseKeyLength")
		.array("licenseKey", {
			type: new Parser().uint8(),
			lengthInBytes: "licenseKeyLength"
		});

	static DELETE_DEVICE = new Parser()
		.uint8("frameID")
		.uint8("userID")
		.uint16("id");

	static DISABLE_DEVICE = new Parser()
		.uint8("frameID")
		.uint8("userID")
		.uint16("id");

	static RESET_DEVICE = new Parser()
		.uint8("frameID")
		.uint8("userID")
		.uint16("id");

	static GET_DEVICE_LOG_QUERY = new Parser()
		.uint8("frameID")
		.uint8("userID")
		.uint16("id");

	static FFT = new Parser()
		.uint8("frameID")
		.uint8("userID")
		.uint16("deviceID")
		.uint8("channel")
		.uint8("repeat");

	static STOP_DEVICE_STATUS = new Parser()
		.uint8("frameID")
		.uint8("userID")
		.uint16("deviceID");

	static UPDATE = new Parser()
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
		});

	static UPDATE_DEVICES_CONFIG = Encoder.UPDATE;
	static UPDATE_DEVICES_FIRMWARE = Encoder.UPDATE;
}

export default Encoder;