import React, { Component } from "react";
import { Form, Formik, Field } from "formik";
import { Button, Card, Col, Row, Spinner } from "react-bootstrap";
import { InputField } from "../../components/forms";
import { inject, observer } from "mobx-react";
import { withTranslation } from 'react-i18next'
import * as yup from "yup";

class LoginForm extends Component {
    render() {
        const { auth, t } = this.props;

        return (
            <Formik initialValues={auth.initialValues.login}
                validationSchema={
                    yup.object().shape({
                        login: yup.string()
                            .label(this.props.t('login'))
                            .required(t('field_is_required')),
                        password: yup.string()
                            .label(this.props.t('password'))
                            .required()
                    })

                }
                onSubmit={this.props.action} // websocket login
            >
                <Form>
                    <Card className="p-4 loading-container">
                        <h5>{t('login_title')}</h5>
                        <p className="text-muted mb-3">{t('login_desc')}</p>

                        <Row>
                            <Col lg={12}>
                                <Field component={InputField}
                                    label="Login"
                                    placeholder={t('login')}
                                    name="login"
                                />
                            </Col>
                            <Col lg={12}>
                                <Field component={InputField}
                                    label={t('password')}
                                    placeholder={t('password')}
                                    type="password"
                                    name="password"
                                />
                            </Col>
                        </Row>

                        <p className="text-muted">
                            <small>
                                <strong>admin / admin</strong>
                            </small>
                        </p>
                        <p className="text-muted">
                            <small>
                                <strong>operator / operator</strong>
                            </small>
                        </p>

                        <div className="text-right mt-3">
                            <Button variant="primary" type="submit" disabled={auth.isLoading}>
                                {auth.isLoading && <Spinner animation="border" size="sm" className="mr-2" />}
                                {t('log_in')}
                            </Button>
                        </div>
                    </Card>
                </Form>
            </Formik>
        );
    }
}

export default inject("auth")(observer(withTranslation()(observer(LoginForm))));