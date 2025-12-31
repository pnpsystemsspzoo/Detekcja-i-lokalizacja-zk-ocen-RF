import React, { Component } from "react";
import classNames from "classnames";
import { inject, observer } from "mobx-react";
import { withTranslation } from "react-i18next";

class StatusSummary extends Component {

	render() {
		const { common, t } = this.props;

		const hardwareHealth = classNames("status status--large", {
			"status--danger": common.system.hardwareHealth === 0,
			"status--warning": common.system.hardwareHealth === 1,
			"status--active": common.system.hardwareHealth === 2,
		});

		const softwareHealth = classNames("status status--large", {
			"status--danger": common.system.softwareHealth === 0,
			"status--warning": common.system.softwareHealth === 1,
			"status--active": common.system.softwareHealth === 2,
		});

		const communication = classNames("status status--large", {
			"status--danger": common.system.communication === 0,
			// "status--warning": common.system.communication === 1,
			"status--active": common.system.communication === 1,
		});

		return <ul className="mb-0">
			<li>
				<figure className={hardwareHealth} />
				<span>{t('hardware')}</span>
			</li>
			<li>
				<figure className={softwareHealth} />
				<span>{t('system')}</span>
			</li>
			<li>
				<figure className={communication} />
				<span>{t('communication')}</span>
			</li>
		</ul>

	}
}

export default inject("common")(withTranslation()(observer(StatusSummary)));