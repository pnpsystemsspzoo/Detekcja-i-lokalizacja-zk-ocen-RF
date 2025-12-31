import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import { decorate, observable } from "mobx";
import { withRouter } from "react-router-dom";
import routes from "../../../config/routes";
import { reverse } from "named-urls";
import { withTranslation } from "react-i18next";

class Row extends Component {
	dropdownIsOpen = false;

	toggle = () => {
		this.dropdownIsOpen = !this.dropdownIsOpen;
	};

	handleRedirect = () => {
		const { alert, history } = this.props;
		history.push(reverse(routes.probe.base, { id: alert.id }));
	};


	render() {
		const { alert, alerts, index, t } = this.props;
		return (
			<tr>
				<td className="text-muted">{index}</td>
				<td>{alert.id}</td>
				<td>
					{alert.timestamp}
				</td>
				<td>
					<small>
						<span className="text-muted">{alert.message}</span>
						<br />
					</small>
				</td>
				<td>
					{!alert.read && <span className="text-muted">{t('unread')}</span>}
					{alert.read && <span className="text-muted">{t('read')}</span>}
					<br />
				</td>


				<td className="td-actions">
					<button type="button" onClick={() => alerts.delete([alert.id])} className="btn btn-sm btn-icon btn-rounded">
						<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor"
							strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
							<line x1="18" y1="6" x2="6" y2="18" />
							<line x1="6" y1="6" x2="18" y2="18" />
						</svg>
					</button>
				</td>


			</tr>
		);
	}
}

decorate(Row, {
	dropdownIsOpen: observable
});

export default inject("alerts")(withTranslation()(withRouter(observer(Row))));