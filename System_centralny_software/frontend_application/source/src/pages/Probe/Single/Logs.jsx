import React, { Component } from "react";
import { decorate, observable } from "mobx";
import { inject, observer } from "mobx-react";
import { withRouter } from "react-router-dom";
import Form from 'react-bootstrap/Form';
import { withTranslation } from "react-i18next";

class Logs extends Component {
	isLoading = false;
	options = [];
	filename = "";

	componentDidMount() {
		const { api, match: { params } } = this.props;

		api.probe.logs({ id: params.id })
			.then(res => {
				this.log = res.data.logs;
			})
			.catch((err) => {
				console.log('err', err)
			})
	}

	render() {
		const { t } = this.props;

		return <div className="probe-logs">
			<Form>
				<Form.Label><h6>{t('activity')}</h6></Form.Label>
				<Form.Control readOnly value={this.log} as="textarea" rows="20" />
			</Form>

		</div>
	}
}

decorate(Logs, {
	isLoading: observable,
	options: observable,
	filename: observable,
	log: observable
});

export default inject("api", "probes")(withTranslation()(withRouter(observer(Logs))));