import React, { Component } from "react";
import { Badge, Table } from "react-bootstrap";
import { observer } from "mobx-react";
import { withTranslation } from "react-i18next";

class DynamicInformation extends Component {
	render() {
		const { probe, t } = this.props;

		return (
			<div className="probe-information-dynamic">
				<Table>
					<tbody>
						<tr>
							<th scope="row" width={200}>{t('synchro')}</th>
							<td>{probe.synchronization ? t('yes') : t('no')}</td>
						</tr>

						<tr>
							<th scope="row">{t('date_measured_by_probe')}</th>
							<td>{probe.datetime}</td>
						</tr>
					</tbody>
				</Table>
				<p><strong>{t('detected_measurements')}</strong></p>
				{
					probe.threats.length > 0 &&
					probe.threats.map((threat, index) =>
						<p key={index}>
							<Badge variant="light" className="mr-2">{threat.getType}</Badge>
							{threat.bandwidth}kHz, f0 = {threat.f0}kHz
						</p>
					)
				}
			</div>
		);
	}
}

export default (withTranslation()(observer(DynamicInformation)));