import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import { Button, Row, Col, UncontrolledTooltip } from "reactstrap";
import { Field } from "formik";
import { Form as FormikForm, Formik } from "formik";
import { InputField } from "../../../../components/forms";
import InlineSVG from "react-inlinesvg";
import { Modal } from "react-bootstrap";
import generator from "generate-password";
import { decorate, observable } from "mobx";
import { Spinner } from "react-bootstrap";
import { withTranslation } from 'react-i18next';
import * as yup from 'yup';

class Add extends Component {
	showPassword = true;
	formRef = React.createRef();

	componentDidMount() {
		const { users } = this.props;
		users.isLoading = false;
	}

	handleShowPassword = e => {
		e.preventDefault();
		this.showPassword = !this.showPassword;
	};

	handleGeneratePassword = e => {
		e.preventDefault();

		const password = generator.generate({
			length: 10,
			numbers: true
		});

		this.formRef.current.setValues({
			password: password,
			password_repeat: password
		});

		this.showPassword = false;
	};

	handleOnSubmit = (values) => {
		const { users } = this.props;
		if (!values.type) values.type = "operator"; // default will be operator
		users.add(values);
	};

	render() {
		const { users, t } = this.props;

		return (
			<Modal show={users.modals.add}
				onHide={() => users.modalToggle("add")}
				centered>
				<Formik
					initialValues={users.initialValues.useradd}
					validationSchema={
						yup.object().shape({
							firstName: yup.string()
								.label(t('firstname'))
								.required(t('field_is_required')),
							lastName: yup.string()
								.label(t('lastname'))
								.required(t('field_is_required')),
							type: yup.string()
								.label(t('type'))
								.required(t('field_is_required'))
								.default('operator'),
							email: yup.string()
								.email(t('not_valid_email'))
								.label(t('email_address'))
								.required(t('field_is_required')),
							login: yup.string()
								.label(t('login'))
								.required(t('field_is_required')),
							password: yup.string()
								.label(t('password'))
								.required(t('field_is_required'))
								.min(8, t('password_to_short'))
								.matches(
									/^(?=.*[A-Za-z])(?=.*\d)(?=.*[@$!%^*#?&])[A-Za-z\d@$!%^*#?&]{8,}$/,
									t('password_restrictions')
								),
							password_repeat: yup.string()
								.label(t('repeat_password'))
								.oneOf([yup.ref("password")], t('password_not_the_same'))
								.required(t('field_is_required'))
						})
					}
					onSubmit={this.handleOnSubmit}
					ref={this.formRef}
				>
					<FormikForm>
						<Modal.Header closeButton>
							<Modal.Title>{t('adding_new_user')}</Modal.Title>
						</Modal.Header>
						<Modal.Body>
							<Row>
								<Col lg={6}>
									<Field component={InputField}
										label={t('firstname')}
										name="firstName"
									/>
								</Col>

								<Col lg={6}>
									<Field component={InputField}
										label={t('lastname')}
										name="lastName"
									/>
								</Col>
							</Row>

							<Row>
								<Col lg={6}>
									<Field component={InputField}
										label={t('type')}
										type="select"
										name="type"
									>
										<option value="operator">{t('operator')}</option>
										<option value="admin">{t('administrator')}</option>
										<option value="service">{t('service')}</option>
									</Field>
								</Col>
							</Row>

							<Row>
								<Col lg={6}>
									<Field component={InputField}
										label={t('login')}
										name="login"
									/>
								</Col>
								<Col lg={6}>
									<Field component={InputField}
										label={t('email_address')}
										type="email"
										name="email"
										autoComplete="new-password"
									/>
								</Col>
							</Row>

							<Row className="d-flex">
								<Col lg={6}>
									<Field component={InputField}
										type={this.showPassword ? "text" : "password"}
										label={t('password')}
										name="password"
										autoComplete="new-password"
									/>
								</Col>

								<Col lg={6}>
									<Field component={InputField}
										type={this.showPassword ? "text" : "password"}
										label={t('repeat_password')}
										name="password_repeat"
										append={<>
											<Button color="white" id="show-password" onClick={this.handleShowPassword}>
												<InlineSVG src="/svg/eye.svg" />
											</Button>
											<UncontrolledTooltip placement="top" target="show-password">{t('show_password')}</UncontrolledTooltip>
										</>}
									/>
								</Col>
							</Row>
						</Modal.Body>
						<Modal.Footer>

							<Button variant="primary" type="submit" disabled={users.isLoading}>
								{users.isLoading && <Spinner animation="border" size="sm" className="mr-2" />}
								{t('add')}
							</Button>

						</Modal.Footer>
					</FormikForm>
				</Formik>
			</Modal>
		);
	}
}

decorate(Add, {
	showPassword: observable,
});

export default inject("users")(withTranslation()(observer(Add)));