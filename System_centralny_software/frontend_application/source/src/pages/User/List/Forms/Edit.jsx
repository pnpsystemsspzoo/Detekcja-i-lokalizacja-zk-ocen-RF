import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import { Field, Form as FormikForm, Formik } from "formik";
import { Button, Row, Col, Modal, Spinner } from "react-bootstrap";
import { InputField } from "../../../../components/forms";
import { withTranslation } from "react-i18next";
import * as yup from 'yup';

class Edit extends Component {
	handleOnSubmit = () => {
		const { users } = this.props;
		users.editUser();
	};

	render() {
		const { users, t } = this.props;
		const { userToEdit } = users;

		return (
			<Modal show={users.modals.edit}
				onHide={() => users.modalToggle("edit")}
				centered>
				<Formik
					initialValues={users.userToEdit}
					validationSchema={
						yup.object().shape({
							firstName: yup.string()
								.label(t('firstname'))
								.required(t('field_is_required')),
							lastName: yup.string()
								.label(t('lastname'))
								.required(t('field_is_required')),
							email: yup.string()
								.email(t('not_valid_email'))
								.label(t('email_address'))
								.required(t('field_is_required')),
						})
					}
					onSubmit={this.handleOnSubmit}>

					<FormikForm>
						<Modal.Header closeButton>
							<Modal.Title>{t('editing_user')}</Modal.Title>
						</Modal.Header>

						<Modal.Body>
							<Row>
								<Col lg={6}>
									<Field component={InputField}
										label={t('firstname')}
										name="firstName"
										value={userToEdit.firstName}
										onChange={(e) => { userToEdit.firstName = e.target.value }}
									/>
								</Col>

								<Col lg={6}>
									<Field component={InputField}
										label={t('lastname')}
										name="lastName"
										value={userToEdit.lastName}
										onChange={(e) => { userToEdit.lastName = e.target.value }}
									/>
								</Col>
							</Row>

							<Row>
								<Col lg={6}>
									<Field component={InputField}
										label={t('email_address')}
										type="email"
										name="email"
										value={userToEdit.email}
										onChange={(e) => { userToEdit.email = e.target.value }}
									/>
								</Col>
							</Row>
						</Modal.Body>

						<Modal.Footer>
							<Button variant="secondary" type="submit" disabled={users.processing.edit}>
								{users.processing.edit && <Spinner animation="border" size="sm" className="mr-2" />}
								{t('save')}
							</Button>
						</Modal.Footer>
					</FormikForm>
				</Formik>
			</Modal>
		);
	}
}

export default inject("users")(withTranslation()(observer(Edit)));