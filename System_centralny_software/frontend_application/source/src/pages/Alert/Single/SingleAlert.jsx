import React, { Component } from "react";
import { Form as FormikForm, Formik } from "formik";
import ModalRow from './ModalRow';
import { Button, Row, Col, Modal } from "react-bootstrap";
import { inject, observer } from "mobx-react";
import { Table } from "react-bootstrap";
import TableHeader from "../../../containers/parts/TableHeader";
import routes from "../../../config/routes";
import { Nav } from "react-bootstrap";
import { NavLink } from "react-router-dom";
import { withTranslation } from "react-i18next";
import { decorate, observable } from "mobx";

class SingleAlert extends Component {
	onSubmit = event => {
		const { alerts } = this.props;
		alerts.modals.isModalOpen = false;
	};

	constructColumnModal = (t) => {
		return [
			{ label: this.props.t('alert_id') },
			{ label: this.props.t('date') },
			{ label: this.props.t('description'), },
			{ label: this.props.t('status'), }
		];
	}

	renderButtons = () => {
		const { alerts, t } = this.props;
		return (
			<>
				<Nav.Item as="span">
					<NavLink to={routes.alerts.base} activeClassName="active">
						<Button type="submit" onClick={() => alerts.closeModal()} component={routes.alerts.base} variant="secondary">{t('alerts_title')}</Button>
					</NavLink>
				</Nav.Item>
			</>
		)
	}

	render() {
		const { alerts, t } = this.props;
		return (
			<>
				<Modal show={this.props.ifShow}
					onHide={() => alerts.modalToggle("isModalOpen")}
					centered>
					<Formik onSubmit={this.onSubmit}>
						<FormikForm>
							<Modal.Header closeButton>
								<Modal.Title>{t('alert')}</Modal.Title>
							</Modal.Header>

							<Modal.Body>
								<Row>
									<Col lg={12}>
										<p>{t('alerts_modal')} {alerts.unread}</p>
										<p>{t('alerts_last')}</p>
										<Table className="table-theme table-theme--small v-middle">
											<TableHeader columns={this.constructColumnModal(t)} store={alerts} />
											<tbody>
												<ModalRow key={alerts.currentAlert.id} alert={alerts.currentAlert} ></ModalRow>
											</tbody>

										</Table>
									</Col>
								</Row>
							</Modal.Body>

							<Modal.Footer>
								{this.renderButtons()}
							</Modal.Footer>
						</FormikForm>
					</Formik>
				</Modal>
			</>
		);
	}
}

decorate(SingleAlert, {
	columnsModal: observable
})
export default inject("alerts", "socket")(withTranslation()(observer(SingleAlert)));