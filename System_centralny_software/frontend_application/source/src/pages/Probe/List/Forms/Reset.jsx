import React, { Component } from "react";
import { Form as FormikForm, Formik } from "formik";
import { Button, Row, Col, Modal, Spinner } from "react-bootstrap";
import { inject, observer } from "mobx-react";
import { withTranslation } from "react-i18next";

class Reset extends Component {
	onSubmit = (event) => {
		const { probes } = this.props;
		probes.processing.reset = true;
		probes.reset();
	};

	render() {
		const { probes, probe, t } = this.props;
		return (
			<Modal show={probes.modals.reset}
				onHide={() => probes.modalToggle("reset")}
				centered>
				<Formik onSubmit={this.onSubmit}>
					<FormikForm>
						<Modal.Header closeButton>
							<Modal.Title>{t('reset_probes_modal')}</Modal.Title>
						</Modal.Header>

						<Modal.Body>
							<Row>
								<Col lg={12}>
									<p>{t('reset_probes_modal_confirmation_text')}</p>
									{probes.selectedItems.map((probe) => <li key={probe.id}>{probe.id}</li>)}
								</Col>
							</Row>
						</Modal.Body>

						<Modal.Footer>
							<Button variant="secondary" type="submit" disabled={!probes.processing.reset && probes.selected.size === 0 && !probe} >
								{probes.processing.reset && <Spinner animation="border" size="sm" className="mr-2" />}
								{t('reset')}
							</Button>
						</Modal.Footer>
					</FormikForm>
				</Formik>
			</Modal>
		);
	}
}

export default inject("probes")(withTranslation()(observer(Reset)))