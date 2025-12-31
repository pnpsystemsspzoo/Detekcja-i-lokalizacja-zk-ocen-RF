import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import { decorate, observable } from "mobx";
import { withRouter } from "react-router-dom";
import { withTranslation } from "react-i18next";

class ModalRow extends Component {
	dropdownIsOpen = false;

	toggle = () => {
		this.dropdownIsOpen = !this.dropdownIsOpen;
	};

	render() {
		const { alert, t } = this.props;

		return (
			<tr>
				<td>{alert.id}</td>
				<td>
					{/* <Status alert={alert} className="d-inline-block mr-2" onlyHealth/> */}
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
			</tr>
		);
	}
}

decorate(ModalRow, {
	dropdownIsOpen: observable
});

export default inject("alerts")(withTranslation()(withRouter(observer(ModalRow))));