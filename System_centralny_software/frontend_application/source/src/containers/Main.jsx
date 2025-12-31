import React, { Component } from "react";
import routes from "../config/routes";
import { Route, Switch, withRouter } from "react-router-dom";
import Panel from "./Panel";
import Auth from "./Auth";
import { inject, observer } from "mobx-react";
import IdleTimer from 'react-idle-timer'
import { SOCK } from "../utils/Constants";
import NoConnection from "../components/NoConnection";
import Toasts from "../components/Toasts";

class Main extends Component {
	constructor(props) {
		super(props)
		this.idleTimer = null;
		this.handleOnAction = this.handleOnAction.bind(this);
		this.handleOnActive = this.handleOnActive.bind(this);
		this.handleOnIdle = this.handleOnIdle.bind(this);
	}

	componentDidMount() {
		window.addEventListener('beforeunload', this.handleUnload);
	}

	handleOnAction(event) { }

	handleOnActive(event) { }

	handleOnIdle(event) {
		const { auth, socket } = this.props;
		socket.sendMess(SOCK.USER_LOGOUT, { id: auth.id });
	}

	render() {
		const { auth, common } = this.props;

		return (
			<>
				{!common.isConnected && <NoConnection />}
				{common.isConnected &&
					<>
						<IdleTimer
							ref={ref => { this.idleTimer = ref }}
							timeout={600000} // 10 minutes
							onActive={this.handleOnActive}
							onIdle={this.handleOnIdle}
							onAction={this.handleOnAction}
							debounce={250}
						/>
						<Switch location={this.props.location}>
							{!auth.isAuthenticated && <Route path={routes.base} component={Auth} />}
							{auth.isAuthenticated && <Route path={routes.base} component={Panel} />}
						</Switch>
						<Toasts />
					</>
				}
			</>
		);
	}
}

export default withRouter(inject("auth", "socket", "common")(observer(Main)));
