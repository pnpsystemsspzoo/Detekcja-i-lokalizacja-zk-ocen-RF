import React, { Component } from "react";
import { Route, Switch } from "react-router-dom";
import { inject, observer } from "mobx-react";
import Sidebar from "./parts/Sidebar";
import Map from "../pages/Map";
import routes from "../config/routes";
import { ProbeList, ProbeSingle } from "../pages/Probe";
import Alerts from "../pages/Alert/List/Alerts";
import ReportsList from "../pages/Reports/List/ReportsList";
import CentralSystem from "../pages/CentralSystem/Main"
import { UserList, UserSingle } from "../pages/User";
import NoConnection from "../components/NoConnection";
import Toasts from "../components/Toasts";
import SingleAlert from "../pages/Alert/Single/SingleAlert";
import { SOCK } from "../utils/Constants";
import SignaturesList from "../pages/Signatures/List/SignaturesList";
import BasicReport from "../pages/CentralSystem/BasicReport";
import ExtendedReport from "../pages/CentralSystem/ExtendedReport";


class Panel extends Component {
	componentDidMount() {
		const { socket, common } = this.props;

		let intervalID = setInterval(() => {
			if (common.isConnected) {
				socket.send(SOCK.OPERATOR_INTERFACE_QUERY);
				socket.send(SOCK.DEVICES_MANAGEMENT_QUERY);
				clearInterval(intervalID);
			}
		}, 1000);
	}

	render() {

		const { auth, common, alerts } = this.props;
		return <>
			{!common.isConnected && <NoConnection />}

			{
				common.isConnected && <div id="main" className="layout-row flex">
					<Sidebar />

					<div id="content" className="flex" style={{ height: '100vh', overflowY: 'scroll' }}>
						<Switch location={this.props.location}>
							<>
								{auth.permissions.probes && <Route path={routes.probes.base} component={ProbeList} />}
								{auth.permissions.probes && <Route path={routes.probe.base} component={ProbeSingle} />}

								{auth.permissions.users && <Route path={routes.users.base} component={UserList} />}
								{auth.permissions.users && <Route path={routes.user.base} component={UserSingle} />}

								<Route path={routes.centralsystembasicreport.base} component={BasicReport} />
								<Route path={routes.centralsystemextendedreport.base} component={ExtendedReport} />

								{auth.permissions.cs && <Route path={routes.centralsystem.base} component={CentralSystem} />}

								{auth.permissions.alerts && <Route path={routes.alerts.base} component={Alerts} />}

								{auth.permissions.reports_list && <Route path={routes.reports.base} component={ReportsList} />}
								{auth.permissions.signatures_list && <Route path={routes.signatures.base} component={SignaturesList} />}

								{auth.permissions.map && <Route exact path={routes.home} component={Map} />}
							</>
						</Switch>
					</div>

					{auth.permissions.alerts && <SingleAlert ifShow={alerts.modals.isModalOpen} />}
					<Toasts />
				</div>
			}
		</>
	}
}

export default inject("auth", "common", "alerts", "socket")(observer(Panel));