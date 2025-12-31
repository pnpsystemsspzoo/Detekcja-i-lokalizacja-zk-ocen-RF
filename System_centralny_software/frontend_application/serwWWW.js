require('dotenv').config();
const jwt = require('jsonwebtoken')
const bcrypt = require("bcrypt");
var net = require("net");
var formidable = require("formidable");
var fs = require("fs");
var moment = require('moment');
const path = require('path');
const Parser = require("./Parser");
const pool = require("./db");
const puppeteer = require('puppeteer');
var archiver = require('archiver');
var generatePdf_pl = require("./services/pdf/pdf_pl");
var generatePdf_gb = require("./services/pdf/pdf_gb");
const parseBuffer = require("./parseBuffer");
const schedule = require('node-schedule');
const tableoptions = require('./services/pdf/tableoptions');

var log4js = require("log4js");
var logger = log4js.getLogger();
logger.level = "debug";

const webSocketsServerPort = 98;

log4js.configure({
	appenders: {
		defaultLog: { type: "file", filename: `logs/defaultLog.log` },
		serverAllLog: {
			type: "file", filename: `logs/serverAllLog.log`, layout: { type: 'messagePassThrough' },
			maxLogSize: 10485760
		},
	},
	categories: {
		default: { appenders: ["defaultLog"], level: "trace" },
		serverAllLog: { appenders: ["serverAllLog"], level: "trace" },

	}
});

const serverAllLogger = log4js.getLogger("serverAllLog");

let query;
let basicReportData = [], extendedReportData = {}

const signaltypes = ["UNKNOWN", "WIDE_SWEEP_SLOW (CHIRP)", "WIDE_SWEEP_MEDIUM (CHIRP)", "WIDE_SWEEP_FAST (CHIRP)", "WIDE_SWEEP_RAPID (CHIRP)",
	"NARROW_SWEEP (CHIRP)", "TRIANGULAR_WAVE (CHIRP)", "TRIANGULAR (CHIRP)", "SAWTOOTH (CHIRP)", "HOOKED_SAWTOOTH (CHIRP)", "TICK", "MULTI_TONE", "PULSE",
	"BROADBAND_UNKNOWN"];

const USERS_LOGS_PATH = "../SC/users/"
const SC_LOGS_PATH = "../SC/SClogs/"
const PROBE_PATH = "../SC/"
const UPLOAD_PATH = "../SC/upload";
const REPORTS_PATH = "./reports";
const ARCHIVE_FILE_NAME = './reports.zip';

const REPORTS_LIMIT = 5;

const backend_address = { host: "192.168.16.2", port: 9001 }

var backend = new net.Socket();
backend.connect(backend_address);

const express = require('express');
const app = express();
const cors = require('cors');
const WSServerExpress = require('websocket').server;
const serverExpress = require('http').createServer();
app.use(cors())
app.use(express.static('public'));
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

let wss = new WSServerExpress({ httpServer: serverExpress });
serverExpress.on('request', app);
serverExpress.listen(webSocketsServerPort, function () {
	let date = moment().format('DD-MM-YYYY__HH-mm-ss');
	console.log(`http / ws server listening on ${webSocketsServerPort}, on ${date}`);

	const job = schedule.scheduleJob('0 0 6 * * *', function () {
		cron_generateReport();
		cron_removeOldInterferences();
	});
});

backend.on("connect", function () {
	backend.setEncoding("binary");
	console.log("Connection established with backend TCP server");
});

var partial_message = null;

const token_begin = Buffer.from([0xAA, 0xAA, 0xAA, 0xAA], "binary");
const token_end = Buffer.from([0xEE, 0xEE, 0xEE, 0xEE], "binary");

const getIndexOfTokenInBuffer = function (token, buff) {
	return buff.indexOf(token, 0, "binary")
}
const tokenExistsInBuffer = function (token, buff) {
	return buff.includes(token, 0, "binary")
}
backend.on("data", function (data) {
	var dat = null
	if (partial_message) {
		dat = partial_message
	} else {
		dat = Buffer.from(data, "binary");

		// encode frame - if frame_ID === 1 (radio interference) save interference to database
		let buffer = dat;
		const check = Parser.parsers.base.parse(buffer);
		const name = check.frame_ID; // eg. 1

		if (name && name === 1) {
			const parser = Parser.parsers.interference_parser;
			try {
				const result = parser.parse(buffer);
				handleInterference(result);
			} catch (e) {
				// console.error(e.message);
			}
		}

		if (name && name === 17) {
			const parser = Parser.parsers.interference_info_parser;
			try {
				const result = parser.parse(buffer);
				handleInterferenceInfo(result);
			} catch (e) {
				// console.error(e.message);
			}
		}

	}
	while (tokenExistsInBuffer(token_begin, dat)) {
		if (tokenExistsInBuffer(token_end, dat)) {
			var begin = getIndexOfTokenInBuffer(token_begin, dat) + 4
			var end = getIndexOfTokenInBuffer(token_end, dat)
			var message = dat.slice(begin, end)

			wss.connections.forEach((ws) => {
				if (ws.user_ID !== null) ws.sendBytes(message);
			})

			dat = dat.slice(end + 4)
			if (!tokenExistsInBuffer(token_begin, dat)) {
				partial_message = null
			}
		} else {
			var begin = getIndexOfTokenInBuffer(token_begin, dat)
			partial_message = dat.slice(begin)
		}
	}
});

backend.on("data", function (data) {
	var dat = Buffer.from(data, "binary");
	parseBuffer(dat, serverAllLogger);
	wss.connections.forEach((ws) => {
		if (ws.user_ID !== null) ws.sendBytes(dat);
	})
});

backend.on("error", function (error) {
	// console.log("TCP connection error");
	// console.log(error);
});

backend.on("close", function () {
	// console.log("Disconnected from backend");
	sendBackendDisconnected();

	backend.destroy();
	setTimeout(function () {
		console.log("reconnecting", new Date());

		backend.connect(backend_address);
	}, 2000);
});

wss.on("request", function (request) {
	var ws = request.accept(null, request.origin);
	ws.user_ID = null;

	ws.on("message", function (message) {
		if (message.type === "binary") {
			var msg = Buffer.from(message.binaryData)
			backend.write(msg)
		}
		else if (message.type === "utf8") {
			decodeMessage(JSON.parse(message.utf8Data), ws);
		}
	});

	ws.on("close", function (ws) {
		ws.user_ID = -1;
		// console.log('ws on close event');
	});

	ws.on("error", function (error) {
		// console.log('ws err', error);
	});

});


// users
app.get('/users', function (req, res) {
	getUsers(req, res);
})
app.post('/user/add', function (req, res) {
	addUser(req, res);
})
app.post('/user/edit', function (req, res) {
	editUser(req, res);
})
app.post('/user/delete', function (req, res) {
	deleteUsers(req, res);
})
app.get('/user/block', function (req, res) {
	blockUser(req, res);
})
app.get('/user/unblock', function (req, res) {
	unblockUser(req, res);
})
app.get('/user/logs', function (req, res) {
	getUserLogs(req, res);
})
app.post('/user/logs/clear', function (req, res) {
	clearUserLogs(req, res);
})
app.get('/user/session/time', function (req, res) {
	getSessionTime(req, res);
})
app.get('/user/session/close', function (req, res) {
	closeSession(req, res);
})
app.get('/user/permissions', function (req, res) {
	getPermissions(req, res);
})
app.post('/user/permissions/save', function (req, res) {
	savePermissions(req, res);
})

// account
app.get('/account/password/set_first', function (req, res) {
	setPasswordFirstTime(req, res);
})
app.get('/account/password/set', function (req, res) {
	setPassword(req, res);
})

// probes
app.get('/probe/logs', function (req, res) {
	getProbeLogs(req, res);
})
app.post('/probe/config/upload', function (req, res) {
	fileUpload(req, res, 'config');
})
app.get('/probe/firmware/upload', function (req, res) {
	fileUpload(req, res, 'firmware');
})
app.get('/probe/history', function (req, res) {
	getProbeHistory(req, res);
})

// signatures
app.get('/signatures', function (req, res) {
	getSignatures(req, res);
})

// reports
app.get('/reports', function (req, res) {
	getReports(req, res);
})

// report
app.post('/report/basic/generate_table', function (req, res) {
	generateTable(req, res);
})
app.post('/report/basic/generate_screen', function (req, res) {
	generateBasicReportScreen(req, res);
})
app.post('/report/extended/generate_table', function (req, res) {
	generateExtendedReportTable(req, res);
})
app.post('/report/extended/generate_screen', function (req, res) {
	generateExtendedReportScreen(req, res);
})
app.get('/report/basic/table', function (req, res) {
	getBasicReportData(req, res);
})
app.get('/report/extended/table', function (req, res) {
	getExtendedReportData(req, res);
})
app.get('/report/download', function (req, res) {
	downloadSelected(req, res);
})
app.get('/report/generate_and_download', function (req, res) {
	generateDownloadPdf(req, res);
})

// central system
app.get('/sc/logs', function (req, res) {
	getSCLogs(req, res);
})
app.post('/sc/logs/clear', function (req, res) {
	clearScLogs(req, res);
})


const decodeMessage = (message, ws) => {
	switch (message.frameID) {
		case 0x40:
			logInUserWs(message.data, ws);
			break;
		case 0x41:
			validateToken(message.data, ws);
			break;
		case 0x42:
			logoutUserWs(message.data, ws);
		default:
			;
	}
}

const getSignatures = async (req, res) => {
	try {
		const signatures = await selectAllSignatures();
		res.send({ signatures })
	}
	catch (err) {
		console.log('err getting signatures', err)
	}
}

const getReports = async (req, res) => {
	try {
		const reports = await selectAllReports();
		res.send({ reports });
	}
	catch (err) {
		console.log('err getting reports', err)
	}
}

const selectUserByLogin = async (login) => {
	return new Promise(async function (resolve, reject) {
		let sql = 'SELECT * FROM public.users where login = $1';
		let values = [login];
		let result = await pool.query(sql, values);

		if (result.rows.length === 1) {
			resolve(result.rows[0]);
		}
		else reject({});
	});
}

const selectAllSignatures = async () => {
	return new Promise(async function (resolve, reject) {
		let sql = 'SELECT * FROM public.interferences';
		let values = [];
		let result = await pool.query(sql, values);
		resolve(result.rows);
	});
}

const selectAllReports = async () => {
	return new Promise(async function (resolve, reject) {
		let sql = 'SELECT public.reports.id, authorid, creationdate, creationtime, type, filename, firstname, lastname, role FROM public.reports INNER JOIN public.users ON public.reports.authorid = users.id';
		let values = [];
		let result = await pool.query(sql, values);
		resolve(result.rows);
	});
}

const selectUserById = async (userid) => {
	return new Promise(async function (resolve, reject) {
		let sql = 'SELECT id, login, firstname, lastname, role, email, blocked, firstpasschanged FROM public.users where id = $1';
		let values = [userid];
		let result = await pool.query(sql, values);

		if (result.rows.length === 1) {
			resolve(result.rows[0]);
		}
		else reject({});
	});
}

const updateUserById = async (userid) => {
	return new Promise(async function (resolve, reject) {
		let sql = 'UPDATE users SET lastlogin = $1 WHERE id = $2 RETURNING *';
		let values = [new Date(), userid];
		let result = await pool.query(sql, values);

		if (result.rows.length === 1) {
			resolve(result.rows[0]);
		}
		else reject({});
	});
}

const editUserById = async (userid, firstname, lastname, email) => {
	return new Promise(async function (resolve, reject) {
		let sql = 'UPDATE users SET firstname = $1, lastname = $2, email = $3 WHERE id = $4 RETURNING *';
		let values = [firstname, lastname, email, userid];
		let result = await pool.query(sql, values);

		if (result.rows.length === 1) {
			resolve(result.rows[0]);
		}
		else reject({});
	});
}


const fileUpload = (req, res, type) => {
	let form = new formidable.IncomingForm();
	form.parse(req, function (err, fields, files) {
		if (err) { res.send({ status: "error" }); }
		else {
			const probesArray = fields.probesArray.split(',');
			const userid = fields.userid;
			const oldpath = files.filetoupload.path;
			const dir = `${UPLOAD_PATH}${type}`;

			// check if directory exists
			if (!fs.existsSync(dir)) {
				fs.mkdirSync(dir, { recursive: true });
			}
			// save sent file to public directory
			let newFileName = `${moment().format("DD-MM-YYYY__HH-mm-ss")}_${files.filetoupload.name}`;
			var newpath = `${dir}/${newFileName}`;
			fs.copyFileSync(oldpath, newpath);

			const response = { filename: newFileName, user_ID: userid, devices: probesArray }
			res.send(response);
		}
	});
}

const getUsers = async (req, res) => {
	try {
		const result = await pool.query("SELECT id, login, firstname, lastname, role, email, lastlogin, blocked FROM public.users");
		const resLength = result.rows.length;

		const users = [];
		if (resLength > 0) {
			for (let i = 0; i < resLength; i++) { // for i users
				let is_logged = false;
				const r = result.rows[i];
				wss.connections.forEach((ws) => {
					if (ws.user_ID === r.id) is_logged = true;
				})
				users[i] = {
					id: r.id, firstName: r.firstname, lastName: r.lastname, lastUpdate: r.lastlogin, email: r.email,
					role: r.role, is_logged, blocked: r.blocked
				};
			}
			res.send(users)
			return;
		} else {
			res.send([])
			console.log('No users in database')
		}
	} catch (err) {
		res.send([])
		console.log('Error getting users: ', err)
	}
}

const checkIfUserExistsQuery = (login, email) => {
	return new Promise(async function (resolve, reject) {
		let sql = 'SELECT * FROM public.users where login = $1 OR email = $2';
		let values = [login, email];
		let result = await pool.query(sql, values);

		if (result.rows.length === 0) {
			resolve(0);
		}
		else reject("user exists (login or email)");
	});
}

const addReportQuery = (authorid, type, filename) => {
	return new Promise(async function (resolve, reject) {
		const creationdate = moment().format("DD-MM-YYYY");
		const creationtime = moment().format("HH:mm:ss");

		let sql = 'INSERT INTO reports(id, authorid, creationdate, creationtime, type, filename) VALUES (default, $1, $2, $3, $4, $5) RETURNING *';
		let values = [authorid, creationdate, creationtime, type, filename];
		let result = await pool.query(sql, values);

		if (result.rows.length === 1) {
			resolve(result.rows[0]);
		}
		else reject({});
	});
}

const deleteAutoReportsQuery = () => {
	return new Promise(async function (resolve, reject) {
		const deletionDate = moment().subtract(10, 'd').format('DD-MM-YYYY');

		let sql = 'DELETE FROM reports WHERE creationdate = $1 AND type = $2 RETURNING *';
		let values = [deletionDate, 'auto'];
		let result = await pool.query(sql, values);

		resolve(result.rows);
	});
}

const deleteOldInterferencesQuery = () => {
	return new Promise(async function (resolve, reject) {
		const deletionDate = moment().subtract(1, 'y').format('YYYY-MM-DD');

		let sql = 'DELETE FROM interferences WHERE date < $1 RETURNING *';
		let values = [deletionDate];
		let result = await pool.query(sql, values);

		resolve(result.rows);
	});
}

const deleteManualReportsQuery = () => {
	return new Promise(async function (resolve, reject) {

		let sql = 'DELETE FROM reports WHERE id = any (array(SELECT id FROM reports WHERE type = $1 ORDER BY creationdate DESC, creationtime DESC OFFSET 49)) RETURNING *';
		let values = ['manual'];
		let result = await pool.query(sql, values);

		resolve(result.rows);
	});
}

const addUserQuery = (login, bcryptPassword, firstName, lastName, type, email) => {
	const now = new Date();
	let first_pass_changed = "no";
	if (type === 'service') first_pass_changed = "yes";

	return new Promise(async function (resolve, reject) {
		let sql = 'INSERT INTO users(id, login, password, firstname, lastname, role, lastlogin, email, lastpasschange, firstpasschanged, blocked) VALUES \
		(default, $1, $2, $3, $4, $5, $6, $7, $8, $9, $10) RETURNING *';
		let values = [login, bcryptPassword, firstName, lastName, type, now, email, now, first_pass_changed, 'no'];
		let result = await pool.query(sql, values);

		if (result.rows.length === 1) {
			resolve(result.rows[0]);
		}
		else reject({});
	});
}

const insertPermissions = (userid, type) => {
	let alerts = "no", manage = "no", probes = "no", users = "no", cs = "no", reports_list = "no", generate_report = "no",
		signatures_list = "no", permissions = "no", map = "no";

	if (type === "admin") {
		alerts = manage = probes = users = cs = reports_list = generate_report = signatures_list = permissions = map = "yes";
	}
	else if (type === "operator") {
		alerts = map = "yes";
	}
	else if (type === "service") {
		manage = probes = "yes";
	}

	return new Promise(async function (resolve, reject) {
		let sql = 'INSERT INTO permissions(id, userid, alerts, manage, probes, users, cs, reports_list, generate_report, signatures_list, permissions, map) VALUES \
		(default, $1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11) RETURNING *';
		let values = [userid, alerts, manage, probes, users, cs, reports_list, generate_report, signatures_list, permissions, map];
		let result = await pool.query(sql, values);

		if (result.rows.length === 1) {
			resolve(result.rows[0]);
		}
		else reject({});
	});
}

const addUser = async (req, res) => {
	const { password, login, firstName, lastName, type, email } = req.body;

	try {
		await checkIfUserExistsQuery(login, email);
		const saltRound = 10;
		bcrypt.genSalt(saltRound, (err, salt) => {
			bcrypt.hash(password, salt, async (err, bcryptPassword) => {
				if (bcryptPassword) {
					// insert the user with bcrypted password
					try {
						let user = await addUserQuery(login, bcryptPassword, firstName, lastName, type, email);
						await insertPermissions(user.id, type);

						let newUser = { id: user.id, firstName: user.firstname, lastName: user.lastname, lastUpdate: user.lastlogin, email: user.email, type: user.role };
						res.send(newUser);
					}
					catch (err) {
						res.send({ id: -1 });
					}
				}
			});
		});
	}
	catch (err) {
		console.log('err adding user', err);
		res.send({ id: -1 });
	}

}

const deleteUsers = async (req, res) => {
	let usersIdArray = Array.from(req.body);

	(async function () {
		const promises = usersIdArray.map(async userid => {
			const user_id = await deleteUser(userid);
			await deletePermissions(userid);
			return user_id;
		})

		const usersIds = await Promise.all(promises);
		res.send({ status: 'success', users: usersIds })
	}());
}

const deleteUser = async (userid) => {
	return new Promise(function (resolve, reject) {
		query = {
			text: 'DELETE FROM public.users WHERE id = $1 RETURNING *',
			values: [userid],
		}
		pool
			.query(query)
			.then(result => {
				resolve(result.rows[0].id);
			})
			.catch(e => console.error(e.stack))
	});
}

const deletePermissions = async (userid) => {
	return new Promise(function (resolve, reject) {
		query = {
			text: 'DELETE FROM public.permissions WHERE userid = $1 RETURNING *',
			values: [userid],
		}
		pool
			.query(query)
			.then(result => {
				resolve(result.rows[0].id);
			})
			.catch(e => console.error(e.stack))
	});
}

const editUser = async (req, res) => {
	const { id, firstname, lastname, email } = req.body;

	try {
		const user = await editUserById(id, firstname, lastname, email);
		res.send({ user })
	}
	catch (err) { console.log('err edit user', err) }
}

const queryReportData = (startDate, endDate) => {
	return new Promise(function (resolve, reject) {
		query = {
			text: 'SELECT * FROM interferences WHERE date >= $1 AND date <= $2 ORDER BY date',
			values: [startDate, endDate],
		}
		pool
			.query(query)
			.then(result => {
				resolve(result.rows);
			})
			.catch(e => console.error(e.stack))
	});
}

const generateTable = async (req, res) => {
	let startDate = moment(req.body.startDate).format('YYYY-MM-DD');
	let endDate = moment(req.body.endDate).format('YYYY-MM-DD');

	const { currentSelectValue_ID, currentSelectValue_SignalType, fcMin, fcMax, bandMin, bandMax, periodMin, periodMax, jMin, jMax } = req.body;

	try {
		const results = await queryReportData(startDate, endDate);

		let filteredData = results;

		// filter by ID
		if (currentSelectValue_ID.value !== -1) {
			filteredData = results.filter(function (currentElement) { return currentElement.id === currentSelectValue_ID.value; });
		}

		if (currentSelectValue_SignalType.value !== -1) {
			filteredData = results.filter(function (currentElement) { return signaltypes[currentElement.signal_type] === currentSelectValue_SignalType.label; });
		}

		if (currentSelectValue_ID.value === -1) {
			if (fcMin !== '') filteredData = filteredData.filter(function (currentElement) { return currentElement.f0 >= fcMin; });
			if (fcMax !== '') filteredData = filteredData.filter(function (currentElement) { return currentElement.f0 <= fcMax; });
			if (bandMin !== '') filteredData = filteredData.filter(function (currentElement) { return currentElement.band >= bandMin; });
			if (bandMax !== '') filteredData = filteredData.filter(function (currentElement) { return currentElement.band <= bandMax; });
			if (periodMin !== '') filteredData = filteredData.filter(function (currentElement) { return currentElement.signal_period >= periodMin; });
			if (periodMax !== '') filteredData = filteredData.filter(function (currentElement) { return currentElement.signal_period <= periodMax; });
			if (jMin !== '') filteredData = filteredData.filter(function (currentElement) { return currentElement.j >= jMin; });
			if (jMax !== '') filteredData = filteredData.filter(function (currentElement) { return currentElement.j <= jMax; });
		}
		res.send(filteredData)
		basicReportData = filteredData;
	}
	catch (err) { console.log('err getting report data', err) };
}



const generateBasicReportScreen = async (req, res) => {
	(async () => {
		try {
			const browser = await puppeteer.launch({
				// headless: false,
				args: ['--no-sandbox'],
				slowMo: 2000, // slow down by 250ms
			});
			const page = await browser.newPage();

			// Configure the navigation timeout
			await page.setDefaultNavigationTimeout(0);

			await page.goto(`${process.env.FRONTEND_ADDRESS}/basic_report_map?key=secret`);
			await page.setViewport({
				width: 2000,
				height: 1600,
				deviceScaleFactor: 1,
			});

			await page.screenshot({ path: 'basic_report_screenshot.jpeg' });
			fs.readFile('basic_report_screenshot.jpeg',
				function (err, content) {
					// Serving the image
					res.send(content);
				});
			await browser.close();

		}
		catch (err) {
			// console.log('err gen screen', err);
			fs.readFile('basic_report_screenshot.jpeg',
				function (err, content) {
					// Serving the image
					res.send(content);
				});
		}
	})();
}

const generateExtendedReportTable = async (req, res) => {
	const { id } = req.body;

	query = {
		text: 'SELECT * FROM interferences where id = $1',
		values: [id],
	}
	pool
		.query(query)
		.then(result => {
			if (result.rows.length > 0) {
				let results = result.rows;

				// open directory data/id
				// iterate over every file and get the data

				let tmpData = [];

				if (fs.existsSync(`data/${id}`)) {
					fs.readdir(`data/${id}`, async (err, files) => {
						if (err) console.log('err generating extended report table', err);
						else {
							files.forEach(async (file) => {
								const data = fs.readFileSync(`data/${id}/${file}`, 'utf8', () => { })
								let json = JSON.parse(data);
								tmpData = tmpData.concat(json)
							});

							extendedReportData = { data: results, json: tmpData };
							res.send({ data: results, json: tmpData })
						}
					});
				}
			}
		})
		.catch(e => console.error(e.stack))
}

const generateExtendedReportScreen = async (req, res) => {
	(async () => {
		try {
			const browser = await puppeteer.launch({
				// headless: false,
				args: ['--no-sandbox'],
				slowMo: 2000, // slow down by 250ms
			});
			const page = await browser.newPage();

			// Configure the navigation timeout
			await page.setDefaultNavigationTimeout(0);

			await page.goto(`${process.env.FRONTEND_ADDRESS}/extended_report_map?key=secret`);
			await page.setViewport({
				width: 2000,
				height: 1600,
				deviceScaleFactor: 1,
			});

			await page.screenshot({ path: 'extended_report_screenshot.jpeg' });
			fs.readFile('extended_report_screenshot.jpeg',
				function (err, content) {
					// Serving the image
					res.send(content);
				});
			await browser.close();
		}
		catch (err) {
			console.log('err gen extended report screen', err);
			fs.readFile('extended_report_screenshot.jpeg',
				function (err, content) {
					// Serving the image
					res.send(content);
				});
		}
	})();
}

const getBasicReportData = (req, res) => {
	res.send(basicReportData)
}

const getExtendedReportData = (req, res) => {
	res.send(extendedReportData);
}

const getSCLogs = (req, res) => {
	const path = `${SC_LOGS_PATH}event_logs.txt`;

	if (fs.existsSync(path)) {
		var stats = fs.statSync(path);
		var fileSizeInBytes = stats.size;
		if (fileSizeInBytes < 999999) {
			const data = fs.readFileSync(path, { encoding: 'utf8', flag: 'r' });
			res.send({ logs: data })
		}
		else res.send({ logs: "File is too big" });
	}
	else res.send({ logs: "No activity in central system" })
}

const getProbeLogs = (req, res) => {
	const { id } = req.query;

	const path = `${PROBE_PATH}probe${id}/event_logs.txt`;
	if (fs.existsSync(path)) {
		const data = fs.readFileSync(path, { encoding: 'utf8', flag: 'r' });
		res.send({ logs: data })
	}
	else res.send({ logs: "No probe activity" })
}

const getUserLogs = (req, res) => {
	const { id } = req.query;
	const path = `${USERS_LOGS_PATH}user${id}_logs.txt`;

	if (fs.existsSync(path)) {
		const data = fs.readFileSync(path, { encoding: 'utf8', flag: 'r' });
		res.send({ logs: data })
	}
	else res.send({ logs: "No user activity" })
}

const getSessionTime = async (req, res) => {
	const { id } = req.query;

	try {
		let user = await queryUserLogin(id);
		const date1 = user.lastlogin;
		const date2 = new Date();
		var diffInMs = date2.getTime() - date1.getTime();
		res.send({ sessionTimeInMs: diffInMs })
	}
	catch (err) { console.log('err querying user', err); }
}


const queryUserLogin = async (userid) => {
	return new Promise(async function (resolve, reject) {
		let sql = 'SELECT firstname, lastname, lastlogin FROM public.users where id = $1';
		let values = [userid];
		let result = await pool.query(sql, values);

		if (result.rows.length === 1) resolve(result.rows[0]);
		else reject({});
	});
}

const replaceIdWithUsername = async (data) => {
	return new Promise(async function (resolve, reject) {
		let newData = JSON.parse(data);

		for (let i = 0; i < newData.config_array.length; i++) {
			let row = newData.config_array[i];
			try {
				let user = await queryUserLogin(row.user_id);
				row.firstname = user.firstname;
				row.lastname = user.lastname;
			}
			catch (err) { console.log('err replace id with username', err); }
		}
		resolve(newData);
	});
}

const getProbeHistory = async (req, res) => {
	const { id } = req.query;
	const path = `${PROBE_PATH}probe${id}/config_list.json`;

	if (fs.existsSync(path)) {
		const data = fs.readFileSync(path, { encoding: 'utf8', flag: 'r' });
		if (data && data.length > 0) {
			try {
				const newData = await replaceIdWithUsername(data);
				res.send({ history: JSON.stringify(newData) })
			}
			catch (err) { console.log('err getting probe history', err); }
		}
	}
	else res.send({ history: "{}" })
}

const addFilesToArchive = async (files, archive, filenamesArray) => {
	return new Promise(resolve => {
		files.forEach(async (file) => {
			filenamesArray.forEach((fn) => {
				if (fn == file) archive.file(`${REPORTS_PATH}/${file}`, { name: file });
			})
		});
		resolve(archive);
	})
}

const downloadSelected = async (req, res) => {
	let { filenamesArray } = req.query;
	filenamesArray = JSON.parse(filenamesArray);

	if (fs.existsSync(REPORTS_PATH)) {
		fs.readdir(REPORTS_PATH, async (err, files) => {
			if (err) console.log('err downloading selected reports', err);
			else {
				var output = fs.createWriteStream(ARCHIVE_FILE_NAME);
				var archive = archiver('zip', {
					gzip: true,
					zlib: { level: 9 } // Sets the compression level.
				});

				archive.on('error', function (err) {
					throw err;
				});

				// pipe archive data to the output file
				archive.pipe(output);
				archive = await addFilesToArchive(files, archive, filenamesArray);
				archive.finalize();

				// send file to frontend
				archive.pipe(res);

				// delete the file
				if (fs.existsSync(ARCHIVE_FILE_NAME)) {
					fs.unlinkSync(`${ARCHIVE_FILE_NAME}`);
				}
			}
		});
	}
}

const clearUserLogs = async (req, res) => {
	const { id } = req.body;
	const path = `${USERS_LOGS_PATH}user${id}_logs.txt`;

	if (fs.existsSync(path)) {
		// delete the file
		fs.unlinkSync(`${path}`);
	}
	res.send({ logs: "No user activity" })
}

const clearScLogs = async (req, res) => {
	const { id } = req.body;
	const path = `${SC_LOGS_PATH}event_logs.txt`;

	if (fs.existsSync(path)) {
		// delete the file
		fs.unlinkSync(`${path}`);
	}
	res.send({ logs: "No central system activity" })
}

// frame_ID: 1, id, longitude, latitude, height, range, heading, speed, time_var, unix_time - this is sent to the database
const handleInterference = async (frame) => {
	const selectingResult = await selectInterferenceFromDbBySignalId(frame.id);
	if (selectingResult.length === 1) {
		await updateInterferenceInDb(selectingResult[0], frame);
	}
}

// frame_ID: 17, signal_id, unix_time,	time_var, signal_type, signal_period, f0, f0_var, bandwidth, bandwidth_var, J, J_var, field_1, field_2, field_3, field_4, devicesIdLength, devicesId
const handleInterferenceInfo = async (frame) => {
	const selectingResult = await selectInterferenceFromDb_classifier(frame);
	if (selectingResult.length === 0) {
		await addInterferenceToDb(frame);
	}
}

const createFileObject = (frame) => {
	let { date, time } = convertUnixTime(frame.unix_time);
	let { id, time_var, longitude, latitude, height, speed, heading, location_variance } = frame;
	return {
		id,
		date,
		time,
		time_var,
		longitude,
		latitude,
		height,
		speed,
		heading,
		location_variance,
	}
}

function selectInterferenceFromDbBySignalId(signal_id) {
	return new Promise(resolve => {
		query = {
			text: 'SELECT * FROM interferences WHERE signal_id = $1',
			values: [signal_id],
		}
		pool
			.query(query)
			.then(result => {
				resolve(result.rows);
			})
			.catch((err) => {
				console.log('err selecting interference by signal_id', err)
			})
	})
}

function selectInterferenceFromDb_classifier(interference) {
	return new Promise(resolve => {

		query = {
			text: 'SELECT * FROM interferences WHERE (f0 BETWEEN $1 AND $2) AND (band BETWEEN $3 AND $4) AND signal_type = $5',
			values: [interference.f0 - interference.f0_var, interference.f0 + interference.f0_var,
			interference.bandwidth - interference.bandwidth_var, interference.bandwidth + interference.bandwidth_var,
			interference.signal_type],
		}
		pool
			.query(query)
			.then(result => {
				resolve(result.rows);
			})
			.catch((err) => {
				console.log('err selecting interference classifier', err)
			})
	})
}

const convertUnixTime = (unix_time) => {
	let _date = new Date(unix_time * 1000);
	let hours, minutes, seconds;

	// Hours part from the timestamp
	let _hours = _date.getHours();
	let _minutes = _date.getMinutes();
	let _seconds = _date.getSeconds();
	if (_hours < 10) hours = `0${_hours}`; else hours = _hours;
	if (_minutes < 10) minutes = `0${_minutes}`; else minutes = _minutes;
	if (_seconds < 10) seconds = `0${_seconds}`; else seconds = _seconds;
	let time = `${hours}:${minutes}:${seconds}`;

	let date = moment(_date).format("YYYY-MM-DD");
	return { date, time };
}

function updateInterferenceInDb(selectingResult, frame) {
	/** 
	 * If longitude_first in selected result from DB is null it means that the row was just added.
	 * Data like latitude_first, longitude_first, height_first, speed_max must be added from frame.
	 * If it's not null it means that this row was added some time ago and it already has longitude_first and latitude_first.
	 * Data like latitude_last, longitude_last, height_last must be added from frame. Speed_max has to be verified if it's greater that current or not.
	*/

	if (selectingResult.longitude_first === null) { // this is first update so set longitude_first, latitude_first, height_first
		return new Promise(resolve => {
			query = {
				text: 'UPDATE interferences SET latitude_first = $1, longitude_first = $2, height_first = $3, speed_max = $5 \
				WHERE signal_id = $4 RETURNING *',
				values: [frame.latitude, frame.longitude, frame.height, selectingResult.signal_id, frame.speed]
			}
			pool
				.query(query)
				.then(result => {
					resolve(result.rows);
				})
				.catch((err) => {
					console.log('err updating interference 1', err)
				})
		})
	}
	else {
		if (selectingResult.speed_max < frame.speed) {
			return new Promise(resolve => {
				query = {
					text: 'UPDATE interferences SET latitude_last = $1, longitude_last = $2, height_last = $3, range_last = $5, heading_last = $6, speed_max = $7 \
					WHERE signal_id = $4 RETURNING *',
					values: [frame.latitude, frame.longitude, frame.height, selectingResult.signal_id, frame.range, frame.heading, frame.speed]
				}

				pool
					.query(query)
					.then(result => {
						resolve(result.rows);
					})
					.catch((err) => {
						console.log('err updating interference 2', err)
					})
			})
		}
		else if (selectingResult.speed_max >= frame.speed) {
			return new Promise(resolve => {
				query = {
					text: 'UPDATE interferences SET latitude_last = $1, longitude_last = $2, height_last = $3, range_last = $5, heading_last = $6 \
					WHERE signal_id = $4 RETURNING *',
					values: [frame.latitude, frame.longitude, frame.height, selectingResult.signal_id, frame.range, frame.heading]
				}

				pool
					.query(query)
					.then(result => {
						resolve(result.rows);
					})
					.catch((err) => {
						console.log('err updating interference 3', err)
					})
			})
		}
	}
}

function addInterferenceToDb(i) {
	let { date, time } = convertUnixTime(i.unix_time);
	let duty_cycle = i.field_1 / (i.field_1 + i.field_2);
	let rise_rate_slope = i.bandwidth / i.field_2;

	return new Promise(resolve => {
		query = {
			text: 'INSERT INTO interferences(id, time, longitude_first, latitude_first, height_first, range_last, speed_max, heading_last, signal_period, \
				signal_type, rise_rate_slope, time_to_return, pulse_rise_time, single_pulse_duration, double_pulse_duration, duty_cycle, f0, \
				f0_var, band, band_var, j, j_var, longitude_last, \
				latitude_last, height_last, date, time_var, signal_id) VALUES \
				(default, $1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14, $15, $16, $17, $18, $19, $20, $21, $22, $23, $24, $25, $26, $27) \
				RETURNING *',
			values: [time, null, null, null, null, null, null, i.signal_period,
				i.signal_type, rise_rate_slope, i.field_3, i.field_2, i.field_1, i.field_3, duty_cycle, i.f0,
				i.f0_var, i.bandwidth, i.bandwidth_var, i.J, i.J_var, null,
				null, null, date, i.time_var, i.signal_id
			]
		}

		pool
			.query(query)
			.then(result => {
				resolve(result.rows);
			})
			.catch((err) => {
				console.log('err adding interference to database', err)
			})
	})
}

const saveInterferenceToFile = (object_file, id) => {
	let json, tmpData = [];

	let todaysDate = moment().format("DD-MM-YYYY");
	let filePath = `data/${id}/${todaysDate}.json`;
	var dir = `data/${id}`;

	if (!fs.existsSync(dir)) {
		fs.mkdirSync(dir);
	}

	fs.exists(filePath, function (exists) {
		if (exists) {
			const data = fs.readFileSync(filePath, { encoding: 'utf8', flag: 'r' });
			if (data && data.length > 0) {
				json = JSON.parse(data);
				tmpData = json;
			}
		}
		tmpData.push(object_file);
		json = JSON.stringify(tmpData);
		fs.writeFile(filePath, json, function () { });
	});
}

const queryBlockUser = async (block, userid) => {
	return new Promise(async function (resolve, reject) {
		let sql = 'UPDATE public.users SET blocked = $1 where id = $2 returning *';
		let values = [block, userid];
		let result = await pool.query(sql, values);

		if (result.rows.length === 1) resolve(result.rows[0]);
		else reject({});
	});
}

const blockUser = async (req, res) => {
	const id = parseInt(req.query.id);
	try {
		await queryBlockUser('yes', id);
		sendForceLogout(id);
		res.send({ status: "Success" })
	}
	catch (err) {
		console.log('err blocking user', err);
		res.send({ status: "Error" })
	}
}

const unblockUser = async (req, res) => {
	const id = parseInt(req.query.id);
	try {
		await queryBlockUser('no', id);
		res.send({ status: "Success" })
	}
	catch (err) {
		console.log('err unblocking user', err);
		res.send({ status: "Error" })
	}
}

const closeSession = (req, res) => {
	const { id } = req.query;

	wss.connections.forEach((ws) => {
		if (ws.user_ID === parseInt(id)) {
			// send force logout query to the client
			sendForceLogout(ws.user_ID);
			res.end();
		}
	})
}

const generateDownloadPdf = async (req, res) => {
	const { startDate, endDate, tableData, options, isExtended, userId, userName, userType, lang } = req.query;

	if (lang === "gb") {
		try {
			const filename = `interference_report__${new Date().getTime()}.pdf`;
			await generatePdf_gb(tableData, options, { isExtended, startDate, endDate }, userId, userName, userType, `${REPORTS_PATH}/${filename}`);

			addReportQuery(userId, 'manual', filename);

			const deletedReports = await deleteManualReportsQuery();
			deletedReports.forEach((report) => {
				const path = `${REPORTS_PATH}/${report.filename}`;
				if (fs.existsSync(path)) {
					fs.unlinkSync(`${path}`);
				}
			})

			res.writeHead(200, { "Content-Type": "application/pdf" });
			let reportFile = fs.readFileSync(__dirname + "/reports/" + filename);
			res.end(reportFile);
		} catch (err) {
			res.send({ status: "Error" });
		}
	}
	else if (lang === "pl") {
		try {
			const filename = `interference_report__${new Date().getTime()}.pdf`;
			await generatePdf_pl(tableData, options, { isExtended, startDate, endDate }, userId, userName, userType, `${REPORTS_PATH}/${filename}`);

			addReportQuery(userId, 'manual', filename);

			const deletedReports = await deleteManualReportsQuery();
			deletedReports.forEach((report) => {
				const path = `${REPORTS_PATH}/${report.filename}`;
				if (fs.existsSync(path)) {
					fs.unlinkSync(`${path}`);
				}
			})

			res.writeHead(200, { "Content-Type": "application/pdf" });
			let reportFile = fs.readFileSync(__dirname + "/reports/" + filename);
			res.end(reportFile);
		} catch (err) {
			res.send({ status: "Error" })
		}
	}
}

const getPermissions = async (req, res) => {
	const { id } = req.query;

	const permissions = await selectPermissionsQuery(id);
	res.send({ permissions })
}

const setPasswordFirstTime = async (req, res) => {
	const { userid, password } = req.body;

	try {
		await updatePasswordById(userid, password);
		await updateFirstPassChangedById(userid);
		res.send({ status: "Success" });
	}
	catch (err) { res.send({ status: "Error" }) }
}

const setPassword = async (req, res) => {
	const { userid, password } = req.body;

	try {
		await updatePasswordById(userid, password);
		res.send({ status: "Success" });
	}
	catch (err) { res.send({ status: "Error" }) }
}

const savePermissions = async (req, res) => {
	const { userid, permissions } = req.body;
	try {
		await updatePermissionsQuery(userid, permissions);
		res.send({ status: "Success" });
	}
	catch (err) { res.send({ status: "Error" }) }
}

const updatePermissionsQuery = async (userid, p) => {
	return new Promise(resolve => {
		query = {
			text: 'UPDATE permissions SET alerts = $1, manage = $2, probes = $3, users = $4, cs = $5, \
							reports_list = $6, generate_report = $7, signatures_list = $8, permissions = $9, map = $10 WHERE userid = $11 RETURNING *',
			values: [p.alerts, p.manage, p.probes, p.users, p.cs, p.reports_list, p.generate_report, p.signatures_list, p.permissions, p.map, userid],
		}
		pool
			.query(query)
			.then(result => {
				resolve(result.rows);
			})
			.catch((err) => {
				console.log('err updating password', err)
			})
	})
}

const updatePasswordById = async (userid, password) => {
	const now = new Date();
	return new Promise(resolve => {
		const saltRound = 10;
		bcrypt.genSalt(saltRound, (err, salt) => {
			bcrypt.hash(password, salt, async (err, bcryptPassword) => {
				if (bcryptPassword) {
					// update the user with bcrypted password
					try {
						query = {
							text: 'UPDATE users SET password = $1, lastpasschange = $3 WHERE id = $2 RETURNING *',
							values: [bcryptPassword, userid, now],
						}
						pool
							.query(query)
							.then(result => {
								resolve(result.rows);
							})
							.catch((err) => {
								console.log('err updating password', err)
							})
					}
					catch (err) {
						reject(err);
					}
				}
			});
		});
	})
}

const updateFirstPassChangedById = async (userid) => {
	return new Promise(resolve => {
		query = {
			text: 'UPDATE users SET firstpasschanged = $1 WHERE id = $2 RETURNING *',
			values: ['yes', userid],
		}
		pool
			.query(query)
			.then(result => {
				resolve(result.rows);
			})
			.catch((err) => {
				console.log('err updating firstPassChanged', err)
			})
	})
}


const cron_generateReport = async () => {
	const _startDate = new Date(Date.now() - 30 * 24 * 60 * 60 * 1000); // date 30 days ago
	const _endDate = new Date();

	let startDate = moment(_startDate).format('YYYY-MM-DD');
	let endDate = moment(_endDate).format('YYYY-MM-DD');

	try {
		const tableData = await queryReportData(startDate, endDate);

		try {
			const filename = `interference_report__${new Date().getTime()}.pdf`;
			await generatePdf_gb(JSON.stringify(tableData), JSON.stringify(tableoptions.tableoptions_gb), { isExtended: 'false', startDate, endDate },
				0, "System", "System", `${REPORTS_PATH}/${filename}`);

			// add report to database
			await addReportQuery(0, 'auto', filename);

			// delete old reports with type 'auto' and date 10 days before today
			const deletedReports = await deleteAutoReportsQuery(); // array of objects

			// delete old reports in reports type auto (older that 10 days - so in directory there is always 10 files with type auto)
			deletedReports.forEach((report) => {
				const path = `${REPORTS_PATH}/${report.filename}`;
				if (fs.existsSync(path)) {
					fs.unlinkSync(`${path}`);
				}
			})

		} catch (err) {
			console.log('err cron generate report', err)
		}
	} catch (err) {
		console.log('err generating cron report', err);

	}
}

const cron_removeOldInterferences = async () => {
	const deletedInterferences = await deleteOldInterferencesQuery();
}

const validateToken = async (message, ws) => {
	const { token } = message;
	const JWT_TOKEN_SECRET = process.env.JWT_TOKEN_SECRET;

	if (token !== null && token !== undefined) {
		jwt.verify(token, JWT_TOKEN_SECRET, async (err, data) => {
			if (err) {
				console.log('err validating token in validate token function');
				const _data = createUserObject(0, "", "", "", "", "no", "no", 1);

				const encoder = Parser.parsers.login_response_encoder;
				const data = (encoder).encode(_data);

				ws.send(data);
			}
			else {
				try {
					const { id, firstName, lastName, isAdmin } = data;

					let user = await selectUserById(id);
					let p = await selectPermissionsQuery(id);

					if (user.id === id) {
						const newuser = { id: user.id, firstName, lastName, isAdmin };
						const accessToken = jwt.sign(newuser, JWT_TOKEN_SECRET);
						const _data = createUserObject(user.id, accessToken, firstName, lastName, user.email, isAdmin, user.role, user.firstpasschanged, 0,
							p.alerts, p.manage, p.probes, p.users, p.cs, p.reports_list, p.generate_report, p.signatures_list, p.permissions, p.map);

						const encoder = Parser.parsers.login_response_encoder;
						const data = (encoder).encode(_data);
						ws.send(data);
						ws.user_ID = user.id;
						sendActivityStatus(1, user.id);
					}
				}
				catch (err) {
					console.log('err validate token', err, data);

					const _data = createUserObject(0, "", "", "", "", "no", "no", 1);
					const encoder = Parser.parsers.login_response_encoder;
					const dataToSend = (encoder).encode(_data);
					ws.send(dataToSend);
				}

			}
		})
	}
}

const sendActivityStatus = (activity, user_ID) => {
	// send activity status to all users
	const encoder = Parser.parsers.user_activity_status;
	const _data = {
		frame_ID: 0x33,
		user_ID,
		activity
	}
	const data = (encoder).encode(_data);
	wss.connections.forEach((ws) => {
		if (ws.user_ID !== null) ws.send(data);
	});
}

const logoutUserWs = async (params, ws) => {
	const { id } = params;

	const encoder = Parser.parsers.user_force_logout;
	const _data = {
		frame_ID: 0x34,
		user_ID: id,
	}
	const data = (encoder).encode(_data);
	wss.connections.forEach((ws) => {
		if (ws.user_ID === parseInt(id)) ws.send(data);
	});

	sendActivityStatus(0, id);
	ws.user_ID = null;
}

const sendForceLogout = (user_ID) => {
	const encoder = Parser.parsers.user_force_logout;
	const _data = {
		frame_ID: 0x34,
		user_ID,
	}
	const data = (encoder).encode(_data);
	wss.connections.forEach((ws) => {
		if (ws.user_ID === parseInt(user_ID)) ws.send(data);
	});
}

const sendBackendDisconnected = () => {
	const encoder = Parser.parsers.backend_disconnected;
	const _data = {
		frame_ID: 0x35,
		text: "Backend disconnected",
	}
	const data = (encoder).encode(_data);
	wss.connections.forEach((ws) => {
		ws.send(data);
	});
}

const selectPermissionsQuery = async (userid) => {
	return new Promise(resolve => {
		query = {
			text: 'SELECT * FROM permissions WHERE userid = $1',
			values: [userid],
		}
		pool
			.query(query)
			.then(result => {
				resolve(result.rows[0]);
			})
			.catch((err) => {
				console.log('err selecting permissions', err)
			})
	})
}

const logInUserWs = async (params, ws) => {
	const { login, password } = params;
	try {
		let user = await selectUserByLogin(login);

		bcrypt.compare(password, user.password).then(async function (validPassword) {
			if (validPassword) {
				if (user.role === 'service' && user.blocked === 'no') {
					// check if user account was created more than 24hours ago (lastpasschange with "service" user type means account creation date)
					const todaysDate = moment().format("DD-MM-YYYY HH:mm:ss");
					const accountCreationDatePlusOneDay = moment(user.lastpasschange, "YYYY-MM-DD HH:mm:ss").add(1, "days").format("DD-MM-YYYY HH:mm:ss");

					if (moment(todaysDate, "DD-MM-YYYY HH:mm:ss").isAfter(moment(accountCreationDatePlusOneDay, "DD-MM-YYYY HH:mm:ss"))) {
						await queryBlockUser('yes', user.id);
						user.blocked = 'yes';
					}
				}

				if (user.blocked === 'yes') {
					const _data = createUserObject(0, "", "", "", "", "no", "", "yes", 3);

					const encoder = Parser.parsers.login_response_encoder;
					const data = (encoder).encode(_data);

					ws.send(data);
				}
				else {
					const JWT_TOKEN_SECRET = process.env.JWT_TOKEN_SECRET;
					let resultOfUpdating = await updateUserById(user.id);

					let isAdmin;
					user.role === "admin" ? isAdmin = "yes" : isAdmin = "no";

					const newuser = { id: user.id, firstName: user.firstname, lastName: user.lastname, isAdmin };
					const accessToken = jwt.sign(newuser, JWT_TOKEN_SECRET);

					let error_code = 0;

					let p = await selectPermissionsQuery(user.id);

					const _data = createUserObject(user.id, accessToken, user.firstname, user.lastname, user.email, isAdmin, user.role, user.firstpasschanged, error_code,
						p.alerts, p.manage, p.probes, p.users, p.cs, p.reports_list, p.generate_report, p.signatures_list, p.permissions, p.map);

					const encoder = Parser.parsers.login_response_encoder;
					const data = (encoder).encode(_data);
					ws.send(data);
					ws.user_ID = user.id;

					// send activity status to all users
					sendActivityStatus(1, user.id);
				}

			}
			else { // send 0 as userid, frontend will show 'incorrect data' toast
				const _data = createUserObject(0, "", "", "", "", "no", "no", 2);

				const encoder = Parser.parsers.login_response_encoder;
				const data = (encoder).encode(_data);

				ws.send(data);
			}
		});
	}
	catch (err) {
		const _data = createUserObject(0, "", "", "", "", "no", "no", 2);

		const encoder = Parser.parsers.login_response_encoder;
		const data = (encoder).encode(_data);

		ws.send(data);
	}
}

const createUserObject = (userid, token, firstName, lastName, email, isAdmin, userType, firstPassChanged, errorCode,
	alerts = "", manage = "", probes = "", users = "", cs = "", reportsList = "", generateReport = "", signaturesList = "", permissions = "", map = "") => {
	return {
		frame_ID: 0x32,
		user_ID: userid,
		tokenLength: token.length,
		token: token,
		firstNameLength: firstName.length,
		firstName: firstName,
		lastNameLength: lastName.length,
		lastName: lastName,
		emailLength: email.length,
		email: email,
		isAdminLength: isAdmin.length,
		isAdmin: isAdmin,
		userTypeLength: userType.length,
		userType,
		firstPassChangedLength: firstPassChanged.length,
		firstPassChanged,
		errorCode,
		alertsLength: alerts.length,
		alerts,
		manageLength: manage.length,
		manage,
		probesLength: probes.length,
		probes,
		usersLength: users.length,
		users,
		csLength: cs.length,
		cs,
		reportsListLength: reportsList.length,
		reportsList,
		generateReportLength: generateReport.length,
		generateReport,
		signaturesListLength: signaturesList.length,
		signaturesList,
		permissionsLength: permissions.length,
		permissions,
		mapLength: map.length,
		map
	}
}