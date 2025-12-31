import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import { decorate, observable } from "mobx";
import { withRouter } from "react-router-dom";
import routes from "../../../config/routes";
import { reverse } from "named-urls";
import classNames from "classnames";
import Status from "../../../containers/parts/Status";
import { degreesToMinutes } from "../../../utils/Helpers";
import { withTranslation } from "react-i18next";

class Row extends Component {
	dropdownIsOpen = false;

	toggle = () => {
		this.dropdownIsOpen = !this.dropdownIsOpen;
	};

	handleRedirect = () => {
		const { probe, history } = this.props;
		history.push(reverse(routes.probe.base, { id: probe.id }));
	};

	renderLastRow = (probe) => {
		if (probe.cards) {
			if (probe.cards.length === 0) {
				return (
					<tr>
						<td></td>
						<td></td>
						<td></td>
						<td></td>
						<td></td>
						<td></td>
						<td></td>
					</tr>
				)
			}

			else if (probe.cards.length === 1) {
				return (
					<tr>
						<td></td>
						<td>cards[0]: {probe.cards[0].cardName}</td>
						<td></td>
						<td></td>
						<td></td>
						<td></td>
						<td></td>
					</tr>
				)
			}
			else if (probe.cards.length === 2) {
				return (
					<>
						<tr>
							<td></td>
							<td>cards[0]: {probe.cards[0].cardName}</td>
							<td>cards[1]: {probe.cards[1].cardName}</td>
							<td></td>
							<td></td>
							<td></td>
							<td></td>
						</tr>
					</>
				)
			}
			else if (probe.cards.length === 3) {
				return (
					<>
						<tr>
							<td></td>
							<td>cards[0]: {probe.cards[0].cardName}</td>
							<td>cards[1]: {probe.cards[1].cardName}</td>
							<td>cards[2]: {probe.cards[2].cardName}</td>
							<td></td>
							<td></td>
							<td></td>
						</tr>
					</>
				)
			}
			else if (probe.cards.length === 4) {
				return (
					<>
						<tr>
							<td></td>
							<td>cards[0]: {probe.cards[0].cardName}</td>
							<td>cards[1]: {probe.cards[1].cardName}</td>
							<td>cards[2]: {probe.cards[2].cardName}</td>
							<td>cards[3]: {probe.cards[3].cardName}</td>
							<td></td>
							<td></td>
						</tr>
					</>
				)
			}
			else if (probe.cards.length === 5) {
				return (
					<>
						<tr>
							<td></td>
							<td>firstMess: {probe.firstMess}</td>
							<td>cards[0]: {probe.cards[0].cardName}</td>
							<td>cards[1]: {probe.cards[1].cardName}</td>
							<td></td>
							<td></td>
							<td></td>
						</tr>
						<tr>
							<td></td>
							<td>cards[2]: {probe.cards[2].cardName}</td>
							<td>cards[3]: {probe.cards[3].cardName}</td>
							<td>cards[4]: {probe.cards[4].cardName}</td>
							<td></td>
							<td></td>
							<td></td>
						</tr>
					</>
				)
			}
		}
	}

	render() {
		const { probe, probes, t } = this.props;

		const classes = classNames({
			"no-connected": !probe.connected,
		});


		return (
			<>
				<tr className={classes}
					onDoubleClick={this.handleRedirect}
				>
					<td style={{ width: "10%" }}><input type="checkbox" onChange={() => probes.select(probe)} checked={probes.selected.has(probe.id)} value={probe.id} /></td>
					<td style={{ width: "20%" }} onClick={this.toggle}>{probe.id}</td>
					<td style={{ width: "20%" }} onClick={this.toggle}><Status status={"hardwareHealth"} statusDesc={t('hardware')} probe={probe} className="d-inline-block" /></td>
					<td style={{ width: "20%" }} onClick={this.toggle}><Status status={"softwareHealth"} statusDesc={t('system')} probe={probe} className="d-inline-block" /></td>
					<td style={{ width: "20%" }} onClick={this.toggle}><Status status={"communication"} statusDesc={t('communication')} probe={probe} className="d-inline-block" /></td>
					<td style={{ width: "30%" }}><button onClick={this.handleRedirect} className="btn">{t('open')}</button></td>
					<td style={{ width: "10%" }}>
						<button onClick={() => probes.modalToggle("delete", probe.id)} className="btn btn-sm btn-icon btn-rounded">
							<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor"
								strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
								<line x1="18" y1="6" x2="6" y2="18" />
								<line x1="6" y1="6" x2="18" y2="18" />
							</svg>
						</button>
					</td>
				</tr>

				{this.dropdownIsOpen === true ? (
					<>
						<tr>
							<td></td>
							<td>{t('mac')} {probe.macAddress}</td>
							<td>{t('ip')} {probe.ipAddress}</td>
							<td>{t('position')} {degreesToMinutes(probe.latitude, probe.longitude)}</td>
							<td>{t('height')} {probe.height}m</td>
							<td></td>
							<td></td>
						</tr>
						<tr>
							<td></td>
							<td>{t('data_port')} {probe.dataPort}</td>
							<td>{t('control_port')} {probe.controlPort}</td>
							<td>{t('who')} {probe.who}</td>
							<td>{t('creation_date')} {probe.creationDate}</td>
							<td></td>
							<td></td>
						</tr>
						<tr>
							<td></td>
							<td>{t('expiration_date')} {probe.expirationDate}</td>
							<td>{t('code')} {probe.code}</td>
							<td>{t('firstmess')}: {probe.firstMess}</td>
							<td></td>
							<td></td>
							<td></td>
						</tr>

						{this.renderLastRow(probe)}
					</>

				) : (
					<tr></tr>
				)}
			</>


		);
	}
}

decorate(Row, {
	dropdownIsOpen: observable
});

export default inject("probes")(withTranslation()(withRouter(observer(Row))));