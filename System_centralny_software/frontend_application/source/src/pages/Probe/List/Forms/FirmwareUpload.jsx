import React, { Component } from "react";
import { Form, Formik } from "formik";
import { inject, observer } from "mobx-react";
import * as yup from "yup";
import { Button, Row, Col, Spinner, Modal } from "react-bootstrap";
import { SOCK } from "../../../../utils/Constants";
import Encoder from "../../../../stores/Encoder";
import { withTranslation } from 'react-i18next';

class FirmwareUpload extends Component {
	ref = React.createRef();
	initial = { file: "" };
	schema = yup.object().shape({
		file: yup.mixed()
			.label("Plik oprogramowania")
			.required()
	});

	handleFile = event => {
		event.preventDefault();
		this.ref.current.setFieldValue(event.currentTarget.name, event.currentTarget.files[0])
	};

	onSubmit = values => {
		const { api, probes, auth, socket } = this.props;

		let probesArray = [];
		probes.selectedItems.forEach((probe) => probesArray.push(probe.id));

		const userid = auth.id;

		const formData = new FormData();
		formData.append("filetoupload", values.file);
		formData.append("probesArray", probesArray);
		formData.append("userid", userid);

		probes.processing.firmwareUpload = true;
		api.probe.config.upload(formData)
			.then((res) => {
				probes.processing.firmware = true;
				probes.processing.firmwareUpload = false;
				probes.modals.firmwareUpload = false;

				const frame = {
					filenameLength: res.data.filename.length + 1,
					filename: res.data.filename,
					devicesLength: res.data.devices.length,
					devices: res.data.devices,
					userID: res.data.user_ID
				};

				probes.clearSelected();
				socket.send(SOCK.UPDATE_DEVICES_FIRMWARE_QUERY, frame, Encoder.UPDATE_DEVICES_FIRMWARE)
			})
			.catch((err) => {
				console.log('err', err)
			})
	};

	render() {
		const { probes, t } = this.props;

		return (
			<Modal show={probes.modals.firmwareUpload}
				onHide={() => probes.modalToggle("firmwareUpload")}
				centered>
				<Formik initialValues={this.initial}
					validationSchema={this.schema}
					onSubmit={this.onSubmit}
					ref={this.ref}>
					<Form>
						<Modal.Header closeButton>
							<Modal.Title>{t('upload_firmware_file')}</Modal.Title>
						</Modal.Header>

						<Modal.Body>
							<Row>
								<Col lg={12}>
									<div className="form-group">
										<label htmlFor="file">{t('firmware_file')}</label>
										<input className="form-control" onChange={this.handleFile} id="file" type="file" name="file" />
									</div>
								</Col>
							</Row>
						</Modal.Body>

						<Modal.Footer>
							<Button variant="secondary" type="submit" disabled={probes.processing.firmwareUpload}>
								{probes.processing.firmwareUpload && <Spinner animation="border" size="sm" className="mr-2" />}
								{t('add')}
							</Button>
						</Modal.Footer>
					</Form>
				</Formik>
			</Modal>
		);
	}
}

export default inject("api", "probes", "common", "auth", "socket")(withTranslation()(observer(FirmwareUpload)));