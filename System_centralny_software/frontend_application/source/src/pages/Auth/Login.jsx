import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import { SOCK } from "../../utils/Constants";
import Loading from "../../components/Loading";
import LanguageSwitch from "../../LanguageSwitch";
import { withTranslation } from 'react-i18next';
import LoginForm from "./LoginForm";
import ChangeFirstPasswordForm from "./ChangeFirstPasswordForm";
import ChangePasswordForm from "./ChangePasswordForm";

class Login extends Component {
	logIn = (values) => {
		const { socket } = this.props;
		socket.sendMess(SOCK.USER_LOGIN, values);
	}
	setPasswordFirstTime = (values) => {
		const { auth } = this.props;
		auth.setPasswordFirstTime(values);
	}
	setPassword = (values) => {
		const { auth } = this.props;
		auth.setPassword(values);
	}

	render() {
		const { auth, t } = this.props;

		return (
			<>
				<LanguageSwitch />
				{auth.token === null && !auth.hasToChangeFirstPassword && !auth.hasToChangePassword && <LoginForm action={this.logIn} />}
				{auth.token === null && auth.hasToChangeFirstPassword && <ChangeFirstPasswordForm action={this.setPasswordFirstTime} />}
				{auth.token === null && auth.hasToChangePassword && <ChangePasswordForm action={this.setPassword} />}
				{auth.token !== null && <Loading fixed bg show={true} text={t('loading_dots')} />}
			</>
		);
	}
}

export default inject("auth", "socket")(withTranslation()(observer(Login)));