import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import classNames from "classnames";
import { decorate, observable } from "mobx";
import { Button } from "react-bootstrap";
import Logs from "./Logs";
import History from "./History";
import Information from "./Information";
import DynamicInformation from "./DynamicInformation";
import { SOCK } from "../../../utils/Constants";
import { Link, withRouter } from "react-router-dom";
import Encoder from "../../../stores/Encoder";
import PageWrapper from "../../../containers/PageWrapper";
import Measurement from "./Measurement";
import Reset from "../List/Forms/Reset";
import UsageChart from "./UsageChart";
import routes from "../../../config/routes";
import InlineSVG from "react-inlinesvg";
import InterferenceTable from "./InterferenceTable";
import { withTranslation } from "react-i18next";

class Single extends Component {
	activeTab = "1";
	log = "";

	constructor(props) {
		super(props);
		const { socket, probes, match: { params } } = this.props;

		socket.send(SOCK.DEVICES_MANAGEMENT_QUERY);

		const probe = probes.setCurrent(params.id);

		socket.send(SOCK.DEVICE_STATUS_QUERY, {
			id: params.id
		}, Encoder.DEVICE_STATUS_QUERY);


		probe.channels.forEach((channel, index) => {
			channel.measure = true;

			socket.send(SOCK.FFT_QUERY, {
				deviceID: parseInt(params.id),
				channel: index,
				repeat: 1
			}, Encoder.FFT);
		})
	}

	componentDidMount() {
		const { common, t, match: { params } } = this.props;
		common.setTitle(t('probe_panel_text', { probeid: params.id }));
	}

	componentWillUnmount() {
		const { socket, probes } = this.props;
		const probe = probes.getCurrent;

		probe.reset();

		socket.send(SOCK.STOP_DEVICE_STATUS_QUERY, {
			deviceID: probe.id
		}, Encoder.STOP_DEVICE_STATUS);

		probe.channels.forEach((channel, index) => {
			if (channel.measure) {
				channel.measure = false;
			}
		})

		socket.send(SOCK.FFT_QUERY, {
			deviceID: probe.id,
			channel: 255,
			repeat: 0
		}, Encoder.FFT);

	}


	toggle = tab => {
		if (this.activeTab !== tab) this.activeTab = tab;
	};

	render() {
		const { probes, alerts, t } = this.props;
		const probe = probes.getCurrent;

		return (
			<PageWrapper isLoading={probes.isLoading} className="page-container--full">
				{
					!probes.isLoading && <>
						<div className="probe">
							<div className="probe__header bg-light">
								<div className="probe__header__left">
									<div className="toolbar mb-2">
										<Link to={routes.probes.base} className="btn btn-sm btn-white">
											<InlineSVG src="/svg/back.svg" />&nbsp; {t('back_to_list')}
										</Link>
									</div>

									<h2 className="text-md d-flex align-items-center">

										{t('probeX', { probeid: probe.id })}
										<small className="text-muted ml-2">{probe.connected ? t('connected') : t('not_connected')}</small>
										<Button className="ml-2"
											variant="secondary" size="sm" onClick={() => alerts.modalToggle("reset")}>{t('reset')}</Button>
									</h2>

									<ul>
										<li>
											<button className={classNames({ active: this.activeTab === "1" })} onClick={() => {
												this.toggle("1");
											}}>
												{t('informations')}
											</button>
										</li>
										<li>
											<button className={classNames({ active: this.activeTab === "2" })} onClick={() => {
												this.toggle("2");
											}}>
												{t('status_and_interferences')}
											</button>
										</li>
										<li>
											<button className={classNames({ active: this.activeTab === "3" })} onClick={() => {
												this.toggle("3");
											}}>
												{t('interference')}
											</button>
										</li>
									</ul>
								</div>



								<div className="probe__header__right">

									<div className="usage usage--text">
										<div className="usage__name">
											<p>{t('processes')}</p>
											<p>{probe.tasksNumber}</p>
										</div>
										<div className="usage__name">
											<p>rxTransfer</p>
											<p>{probe.usage.rxTransfer.current} KB</p>
										</div>
										<div className="usage__name">
											<p>txTransfer</p>
											<p>{probe.usage.txTransfer.current} KB</p>
										</div>
									</div>

									<div style={{ display: 'flex', marginRight: 20 }}>
										<UsageChart usage={probe.currentHddUsage} chartname="HDD %" color="blue" />
										<UsageChart usage={probe.currentCpuUsage} chartname="CPU %" color="red" />
										<UsageChart usage={probe.currentRamUsage} chartname="RAM %" color="yellow" />
									</div>

								</div>
							</div>

							<div className="probe__content bg-white">
								<div className={classNames("tab", { active: this.activeTab === "1" })}>
									<div className="probe_information">
										<div className="flex_el_1">
											<Information probe={probe} />
										</div>
										<div className="flex_el_2">
											<History probe={probe} />
										</div>
									</div>
									<div className="probe_information">
										<div className="flex_el_3">
											<Logs onSubmit={this.onSubmitLogs} probe={probe} />
										</div>
									</div>
								</div>

								<div className={classNames("tab", { active: this.activeTab === "2" })}>

									<div className="probe-status-flex">

										<div className="probe-flex-1">
											<div className="flex-item">
												<DynamicInformation probe={probe} />
											</div>
										</div>
										<div className="probe-flex-2">
											{
												probe.channels.length > 0 && probe.channels.map((channel, index) =>
													<div key={index} className="flex-item">
														<Measurement probe={probe} index={index} channel={channel} />
													</div>
												)
											}
										</div>
									</div>
								</div>

								<div className={classNames("tab", { active: this.activeTab === "3" })}>
									<div className="probe_information">
										<div className="probe-flex-1">
											<div className="flex-item">
												<InterferenceTable probe={probe} />
											</div>
										</div>

									</div>
								</div>
							</div>
						</div>
					</>
				}
				<Reset probe={probe.id} />
			</PageWrapper>
		);
	}
}

decorate(Single, {
	activeTab: observable,
});

export default inject("socket", "common", "probes", "alerts")(withTranslation()(withRouter(observer(Single))));