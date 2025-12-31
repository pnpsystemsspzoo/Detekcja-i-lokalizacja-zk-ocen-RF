import React, { Component } from "react";
import { withTranslation } from "react-i18next";
import { Table } from "reactstrap";
import { degreesToMinutes } from "../../../utils/Helpers";

class Information extends Component {
	render() {
		const { probe, t } = this.props;

		return <div className="probe-information">
			<h5>{t('info_probe')}</h5>
			<Table>
				<tbody>
					<tr>
						<th scope="row">{t('device_version')}</th>
						<td>{probe.version}</td>
					</tr>
					<tr>
						<th scope="row" width={200}>{t('mac')}</th>
						<td>{probe.macAddress}</td>
					</tr>
					<tr>
						<th scope="row">{t('ip')}</th>
						<td>{probe.ipAddress}</td>
					</tr>
					<tr>
						<th scope="row">{t('geo_position')}</th>
						<td>{degreesToMinutes(probe.latitude, probe.longitude)}</td>
					</tr>

					<tr>
						<th scope="row">{t('height')}</th>
						<td>{probe.height}m</td>
					</tr>
				</tbody>
			</Table>
		</div>
	}
}

export default withTranslation()(Information);