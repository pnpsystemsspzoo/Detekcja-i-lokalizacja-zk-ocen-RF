import common from "./Common";
import auth from "./Auth";
import { decorate, observable } from "mobx";
import config from "../config/main";
import Parser from "./Parser";
import { getName, SOCK } from "../utils/Constants";
import Encoder from "./Encoder";

class Socket {
	/**
	 * Instance of WebSocket
	 * @type {WebSocket}
	 */
	instance = null;

	/**
	 * Defined handlers
	 * @type {Map<number, function>}
	 */
	callbacks = new Map();

	/**
	 * Connect to WebSocket
	 */
	constructor() {
		this.connect();
	}

	/**
	 * Main parser of incoming frames
	 * Detect frame ID, parse binary data and passing data to handler
	 * @param buffer
	 */
	parse = buffer => {
		const check = Parser.BASE.parse(buffer); // { frame_ID: x }
		const name = getName(check.frame_ID); // CS_STATUS, RADIO_INTERFERENCE etc.

		if (name) {
			const parser = Parser[name];
			try {
				const result = parser.parse(buffer);
				if (this.callbacks.has(result.frame_ID)) {
					const callback = this.callbacks.get(result.frame_ID);
					callback(result);
				}

				if (buffer.length > result.parserOffset) {
					const _buffer = new Buffer(buffer.length - result.parserOffset);
					buffer.copy(_buffer, 0, result.parserOffset);
					this.parse(_buffer);
				}
			} catch (e) {
				console.error('ERROR', e.message);
			}
		}
	};

	sendToken = () => {
		const token = localStorage.getItem("token");
		token !== null && this.sendMess(SOCK.USER_SEND_TOKEN, { token });
	}

	/**
	 * Connecting function to backend
	 */
	connect = () => {
		common.connecting = true;
		this.instance = new WebSocket(config.SOCKET);

		this.instance.onopen = () => {
			common.connecting = false;
			common.isConnected = true;
			this.sendToken();
		};

		this.instance.onclose = () => {
			common.isConnected = false;
			setTimeout(() => {
				this.connect();
			}, 1000);
		};

		this.instance.onerror = () => {
			this.instance.close();
			common.connecting = false;
		};

		this.instance.onmessage = event => {
			try {
				event.data.arrayBuffer().then(buffer => {
					const _buffer = new Buffer(buffer);
					this.parse(_buffer);
				});
			} catch (e) {
				console.error(e.message);
			}
		};
	};


	/**
	 * Send query to backend
	 * @param query {number} Frame ID
	 * @param data {object} JSON object
	 * @param encoder {Parser} Encoder to binaryData
	 * @returns {boolean}
	 */
	send = (query, data, encoder = null) => {
		let _data = Object.assign({
			frameID: query,
			userID: auth.id
		}, data);

		if (this.instance && this.instance.readyState === WebSocket.OPEN) {
			const data = (encoder ? encoder : Encoder.BASE).encode(_data);
			this.instance.send(data);
		}

		return true;
	};

	sendMess = (query, data) => {
		let frame = {
			frameID: query,
			data
		}

		if (this.instance && this.instance.readyState === WebSocket.OPEN) {
			this.instance.send(JSON.stringify(frame));
		}
		return true;
	}

	/**
	 * Set handler for frame by type
	 * @param type {number} Frame ID
	 * @param callback {function} Function to handle parsed data
	 */
	on = (type, callback) => {
		this.callbacks.set(type, callback);
	};

	/**
	 * Delete defined handler
	 * @param type {number} frame ID
	 */
	off = type => {
		this.callbacks.delete(type);
	};
}

decorate(Socket, {
	pending: observable,
});

export default new Socket();