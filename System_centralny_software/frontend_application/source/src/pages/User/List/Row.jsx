import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import { decorate, observable } from "mobx";
import { Link, withRouter } from "react-router-dom";
import routes from "../../../config/routes";
import { reverse } from "named-urls";
import { nameToGradient } from "../../../utils/Helpers";
import { withTranslation } from "react-i18next";

class Row extends Component {
	dropdownIsOpen = false;

	toggle = () => {
		this.dropdownIsOpen = !this.dropdownIsOpen;
	};
	handleRedirect = () => {
		const { user, history } = this.props;
		history.push(reverse(routes.user.base, { id: user.id }));
	};

	render() {
		const { user, users, index, t } = this.props;

		return (
			<tr onDoubleClick={this.handleRedirect}>
				<td style={{ width: "10%" }}><input type="checkbox" onChange={() => users.select(user)} checked={users.selected.has(user.id)} value={user.id} /></td>

				<td className="text-muted">{index}</td>
				<td>{user.id}</td>
				<td>
					<Link to={reverse(routes.user.base, { id: user.id })} className="d-flex">
						<figure className="w-40 mr-3 mb-0 avatar gd-primary" {...nameToGradient(user.name)}>
							{user.initial}
							{user.is_logged && <span className="avatar-status on b-white avatar-right" />}
						</figure>
						<span>
							{user.name}
							<br />
							<span className="text-muted">{user.email}</span>
						</span>
					</Link>
				</td>
				<td>{user.lastUpdate}</td>
				<td>{user.role}</td>
				<td>{user.blocked === 'yes' ? t('yes') : t('no')}</td>
			</tr>
		);
	}
}

decorate(Row, {
	dropdownIsOpen: observable
});

export default inject("users")(withTranslation()(withRouter(observer(Row))));