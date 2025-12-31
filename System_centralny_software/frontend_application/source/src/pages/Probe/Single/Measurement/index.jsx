import React, { Component } from "react";
import HistogramChart from "./HistogramChart";
import { SOCK } from "../../../../utils/Constants";
import Encoder from "../../../../stores/Encoder";
import { Badge } from "react-bootstrap";
import { inject, observer } from "mobx-react";
import FrameChart from "./FrameChart";
import { withTranslation } from "react-i18next";

class Measurement extends Component {
	intervalID = [];

	componentWillUnmount() {
		this.intervalID.forEach((interv) => {
			clearInterval(interv);
		})
	}

	handleMeasurement = event => {
		const { index, socket, channel } = this.props;
		channel.measure = event.target.checked;
		channel.reset();

		socket.send(SOCK.FFT_QUERY, {
			deviceID: channel.probe.id,
			channel: index,
			repeat: channel.measure ? 1 : 0
		}, Encoder.FFT);
	};

	render() {
		const { channel, t } = this.props;

		return (
			<div className="probe-channel">
				<div className="probe-channel__header">
					<Badge pill variant={channel.interfered ? "warning" : "secondary"} className="mr-2">{channel.name}</Badge>
					<span className="mr-2">{t('f')}: {channel.fBegin} - {channel.fEnd}kHz</span>
					<label className="md-switch ml-auto" title={t('enable_disable_measure')}>
						<input type="checkbox" defaultChecked={channel.measure} onChange={this.handleMeasurement} />
						<i className="indigo" />
					</label>
				</div>
				<div className="probe-channel__charts">
					{
						channel.available && <>
							<FrameChart data={channel.density} xDomainStart={channel.fBegin} xDomainEnd={channel.fEnd} />
							<HistogramChart measurement={channel} />
						</>
					}
					{
						!channel.available && <div className="probe-channel__no-data">
							{t('no_data')}
						</div>
					}
				</div>
			</div>
		);
	}
}

export default inject("socket", "probes")(withTranslation()(observer(Measurement)));