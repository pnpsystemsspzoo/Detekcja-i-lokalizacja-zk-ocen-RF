import React, { Component } from "react";
import { Form as FormikForm, Formik } from "formik";
import { Button, Row, Col, Modal, Spinner } from "react-bootstrap";
import { inject, observer } from "mobx-react";
import { SOCK } from "../../../../utils/Constants";
import Encoder from "../../../../stores/Encoder";
import * as yup from "yup";
import { withTranslation } from 'react-i18next';

class Add extends Component {
	ref = React.createRef();
	initial = { file: "" };
	schema = yup.object().shape({
		file: yup.mixed()
			.label("Plik licencyjny (*bin)")
			.required()
	});

	onSubmit = values => {
		const { socket, probes } = this.props;
		const reader = new FileReader();

		reader.onloadend = () => {
			const frame = {
				licenseKeyLength: reader.result.byteLength,
				licenseKey: [...new Uint8Array(reader.result)],

			};
			probes.processing.add = true;
			socket.send(SOCK.ADD_DEVICE_QUERY, frame, Encoder.ADD_DEVICE); // (query, data, encoder = null)
		};

		reader.readAsArrayBuffer(values.file);
	};

	handleFile = event => {
		event.preventDefault();
		this.ref.current && this.ref.current.setFieldValue(event.currentTarget.name, event.currentTarget.files[0])
	};

	render() {
		const { probes, t } = this.props;

		return (
			<Modal show={probes.modals.add}
				onHide={() => probes.modalToggle("add")}
				centered>
				<Formik initialValues={this.initial}
					validationSchema={this.schema}
					onSubmit={this.onSubmit}
					ref={this.ref}>
					<FormikForm>
						<Modal.Header closeButton>
							<Modal.Title>{t('adding_new_probe')}</Modal.Title>
						</Modal.Header>

						<Modal.Body>
							<Row>
								<Col lg={12}>
									<div className="form-group">
										<label htmlFor="file">{t('licence_file')}</label>
										<input className="form-control" onChange={this.handleFile} id="file"
											type="file" name="file" />
									</div>
								</Col>
							</Row>
						</Modal.Body>

						<Modal.Footer>
							<Button variant="secondary" type="submit" disabled={probes.processing.add}>
								{probes.processing.add && <Spinner animation="border" size="sm" className="mr-2" />}
								{t('add')}
							</Button>
						</Modal.Footer>
					</FormikForm>
				</Formik>
			</Modal>
		);
	}
}

export default inject("probes", "socket")(withTranslation()(observer(Add)));