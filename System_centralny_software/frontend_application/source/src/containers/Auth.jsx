import React, { Component } from "react";
import { Route, Switch } from "react-router-dom";
import { inject, observer } from "mobx-react";
import routes from "../config/routes";
import Login from "../pages/Auth/Login";

/**
 *
 */
class Auth extends Component {
	render() {

		return (
			<main className="page-auth">
				<div className="w-xl">
					<Switch location={this.props.location}>
						<Route path={routes.auth.login} component={Login} />
					</Switch>
				</div>
			</main>
		);
	}
}

export default inject("auth")(observer(Auth));