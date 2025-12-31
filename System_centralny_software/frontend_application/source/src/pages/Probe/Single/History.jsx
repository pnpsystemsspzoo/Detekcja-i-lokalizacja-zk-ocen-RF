import React, { Component } from "react";
import { Table } from "reactstrap";
import { decorate, observable } from "mobx";
import { inject, observer } from "mobx-react";
import moment from "moment";
import { SOCK } from "../../../utils/Constants";
import Encoder from "../../../stores/Encoder";
import { Button } from "react-bootstrap";
import Loading from "../../../components/Loading";
import { withTranslation } from "react-i18next";

class History extends Component {
	history = [];

	componentDidMount() {

		const { api, probe } = this.props;
		this.isLoading = true;

		api.probe.history({ id: probe.id })
			.then(res => {
				const resp = JSON.parse(res.data.history); // resp = { config_array: [ {}, {}, {} ] }
				resp.hasOwnProperty("config_array") ? this.history = resp.config_array : this.history = [];
				this.isLoading = false;
			})
	}

	restoreVersion = (filename, type) => {
		const { probe, socket } = this.props;

		const frame = {
			filenameLength: filename.length + 1,
			filename: filename,
			devicesLength: 1,
			devices: [probe.id]
		};

		type === "config" ? socket.send(SOCK.UPDATE_DEVICES_CONFIG_QUERY, frame, Encoder.UPDATE_DEVICES_CONFIG) :
			socket.send(SOCK.UPDATE_DEVICES_FIRMWARE_QUERY, frame, Encoder.UPDATE_DEVICES_FIRMWARE)
	}

	render() {
		const { t } = this.props;

		return <div className="probe-history" style={{ maxHeight: 450, minHeight: 450, overflowY: "scroll" }}>
			<h5>{t('config_history')}</h5>

			{this.isLoading &&
				<div className="loading-container-2">
					<Loading show={true} />
				</div>
			}

			{!this.isLoading && this.history.length > 0 &&
				<Table className="probe_history_table">
					<thead>
						<tr>
							<th>{t('loaded_date')}</th>
							<th>{t('file_name')}</th>
							<th>{t('file_type')}</th>
							<th>{t('user')}</th>
							<th>{t('restore_version')}</th>
						</tr>
					</thead>
					<tbody>
						{
							this.history.length > 0 &&
							this.history.map((item, index) =>
								<tr key={index}>
									<td>{moment(item.load_datetime).format("DD.MM.YYYY HH:mm:ss")}</td>
									<td>{item.filename}</td>
									<td>{item.type}</td>
									<td>{item.firstname} {item.lastname} (ID: {item.user_id})</td>
									<td><Button variant="secondary" size="sm" onClick={() => this.restoreVersion(item.filename, item.type)}>{t('restore')}</Button></td>
								</tr>
							)
						}
					</tbody>
				</Table>
			}
			{
				this.history.length === 0 && !this.isLoading &&
				<div>{t('no_config')}</div>
			}
		</div >
	}
}

decorate(History, {
	isLoading: observable,
	history: observable
});

export default inject("api", "socket", "probes")(withTranslation()(observer(History)));