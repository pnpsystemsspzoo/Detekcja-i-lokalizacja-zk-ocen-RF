import React
	// , { useEffect } 
	from "react";
import "./styles/main.scss";
import "./styles/_dark/main.scss";
import { Provider } from "mobx-react";
import Main from "./containers/Main";
import Common from "./stores/Common";
import { BrowserRouter } from "react-router-dom";
import Auth from "./stores/Auth";
import SingleProbe from "./stores/SingleProbe";
import Probes from "./stores/Probes";
import Users from "./stores/Users";
import Socket from "./stores/Socket";
import Map from "./stores/Map";
import Alerts from "./stores/Alerts";
import moment from "moment";
import "moment/locale/pl";
import { SOCK } from "./utils/Constants";
import Api from "./stores/base/Api";
import Toasts from "./stores/Toasts";
// import Beforeunload from './pages/Alert/List/Beforeunload';
// import { HashRouter as Router, Switch, Route } from 'react-router-dom';
import { Switch, Route } from 'react-router-dom';
import BasicReportMap from "./pages/ReportMap/BasicReportMap";
import ExtendedReportMap from "./pages/ReportMap/ExtendedReportMap";
import Reports from "./stores/Reports";
import Signatures from "./stores/Signatures";

moment().locale("pl");

const stores = {
	common: Common,
	socket: Socket,
	api: Api,
	auth: Auth,
	singleprobe: SingleProbe,
	toasts: Toasts,
	map: Map,
	probes: new Probes(Map),
	users: Users,
	alerts: new Alerts(),
	reports: new Reports(),
	signatures: new Signatures(),
};

// COMMON

Socket.on(SOCK.USER_LOGIN_RESPONSE, stores.auth.loginResponse);
Socket.on(SOCK.USER_ACTIVITY_STATUS, stores.users.userActivityStatus);
Socket.on(SOCK.USER_FORCE_LOGOUT, stores.auth._logout);

Socket.on(SOCK.CS_STATUS, stores.common.handleCentralSystem);
Socket.on(SOCK.RADIO_INTERFERENCE, stores.map.handleInterference);
Socket.on(SOCK.RADIO_INTERFERENCE_INFO, stores.probes.handleInterferenceInfoDevices);


//PROBES
// Socket.on(SOCK.DEVICE_LIST, stores.probes.handleList);
Socket.on(SOCK.DEVICE_SHORT_STATUS, stores.probes.handleProbeShortStatus);
Socket.on(SOCK.DEVICE_FULL_STATUS, stores.probes.handleProbeFullStatus);
Socket.on(SOCK.FFT, stores.probes.handleMeasurement);
// Socket.on(SOCK.GET_LOG_RESPONSE, stores.probes.getProbeLogsResponse);
Socket.on(SOCK.DEVICE_INFO, stores.probes.handleDeviceInfo);

//FORMS
Socket.on(SOCK.ADD_DEVICE, stores.probes.handleAddProbe);
Socket.on(SOCK.DELETE_DEVICE, stores.probes.handleRemoveProbe);
Socket.on(SOCK.CONFIGURATION_LOAD_STATUS, stores.probes.handleConfigStatus);
Socket.on(SOCK.FIRMWARE_LOAD_STATUS, stores.probes.handleFirmwareStatus);
Socket.on(SOCK.NEW_CONFIGURATION_UPLOADED, stores.probes.handleConfigUpload);
Socket.on(SOCK.NEW_FIRMWARE_UPLOADED, stores.probes.handleFirmwareUpload);

//ALERTS
Socket.on(SOCK.ALERT, stores.alerts.handleAlerts);
Socket.on(SOCK.BACKEND_DISCONNECTED, stores.alerts.backendDisconnected);

function App() {
	// useEffect(() => {
	// 	// window.addEventListener('beforeunload', handleUnload);
	// }, []);

	// function handleUnload(e) {

	// var message = "\o/";

	// (e || window.event).returnValue = message; //Gecko + IE
	// return message;
	// }

	return (
		<Provider {...stores}>
			{/* <Beforeunload alertsstore={stores.alerts} onBeforeunload={event => event.preventDefault()} > */}
			<BrowserRouter>
				<Switch>
					<Route path="/basic_report_map" component={BasicReportMap} />
					<Route path="/extended_report_map" component={ExtendedReportMap} />
					<Route path="/" component={Main} />
				</Switch>
			</BrowserRouter>
			{/* </Beforeunload> */}
		</Provider>
	);
}

export default App;
