import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import { decorate, observable } from "mobx";
import { Card, CardBody } from "reactstrap";
import CardHeader from "reactstrap/es/CardHeader";
import Form from 'react-bootstrap/Form';
import { Button } from "reactstrap";
import { withRouter } from "react-router-dom";
import PageWrapper from "../../containers/PageWrapper";
import UsageChart from "../Probe/Single/UsageChart";
import { withTranslation } from "react-i18next";

class CentralSystem extends Component {
	componentDidMount() {
		const { common, api } = this.props;
		common.setTitle("Panel systemu centralnego");

		api.centralsystem.logs.get()
			.then(res => {
				this.log = res.data.logs;
			})
			.catch((err) => {
				console.log('err', err)
			})
	}

	clearFile = () => {
		const { api } = this.props;
		api.centralsystem.logs.clear()
			.then(res => {
				this.log = res.data.logs;
			})
			.catch((err) => {
				console.log('err', err)
			})
	}

	render() {
		const { common, t } = this.props;
		return (
			<PageWrapper title={t('cs_title')}>
				<div className="padding">

					<Card>
						<CardHeader className="p-4 d-sm-flex align-items-center no-shrink">
							<div>
								<h2 className="text-md">{t('data_usage')}</h2>
							</div>
						</CardHeader>
						<CardBody>
							<div className="probe__header__right">

								<div className="usage usage--text">
									<div className="usage__name">
										<p>{t('processes')}</p>
										<p>{common.system.tasksNumber}</p>
									</div>
								</div>

								<UsageChart usage={common.system.currentHddUsage} chartname="HDD %" color="blue" />
								<UsageChart usage={common.system.currentCpuUsage} chartname="CPU %" color="red" />
								<UsageChart usage={common.system.currentRamUsage} chartname="RAM %" color="yellow" />

							</div>
						</CardBody>
					</Card>


					<Card>
						<CardHeader className="p-4 d-sm-flex align-items-center no-shrink">
							<div>
								<h2 className="text-md">{t('cs_logs')}</h2>
							</div>
							<div className="align-self-start ml-auto">
								<Button onClick={() => this.clearFile()} variant="secondary" type="submit">{t('clear_logs')}</Button>
							</div>
						</CardHeader>
						<CardBody>

							<Form>
								<Form.Label><h6>{t('activity')}</h6></Form.Label>
								<Form.Control readOnly value={this.log} as="textarea" rows="30" />
							</Form>
						</CardBody>
					</Card>
				</div>
			</PageWrapper>
		);
	}
}

decorate(CentralSystem, {
	log: observable
});

export default inject("common", "api")(withTranslation()(withRouter(observer(CentralSystem))));