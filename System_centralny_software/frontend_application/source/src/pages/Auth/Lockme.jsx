import React, { Component } from "react";
import { withTranslation } from "react-i18next";
import { nameToGradient } from "../../utils/Helpers";

class LockMe extends Component {
	render() {
		const { t } = this.props;

		return (
			<div className="h-v d-flex flex align-items-center lock-me">
				<div className="mx-auto w-xl w-auto-xs animate fadeIn text-center">
					<figure className="avatar w-96 mb-3" {...nameToGradient(auth.name)}>{auth.initial}</figure>
					<p className="mt-3 font-bold">{auth.name}</p>
				</div>
				<div className="md-form-group">
					<input type="password" className="md-input text-center" />
					<label className="d-block w-100">{t('your_password')}</label>
				</div>
				<div className="mt-3">
					<a className="btn btn-rounded btn-primary">{t('Delete')}</a>
				</div>
			</div>
		);
	}
}

export default inject("auth")(withTranslation()(observer(LockMe)));