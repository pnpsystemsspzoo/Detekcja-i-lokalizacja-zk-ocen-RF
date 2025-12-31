import React, { Component } from "react";
import { Field, Form, Formik } from "formik";
import { Button, Card, Col, Row, Spinner } from "react-bootstrap";
import { InputField } from "../../components/forms";
import { inject, observer } from "mobx-react";
import { withTranslation } from 'react-i18next'
import * as yup from "yup";

class ChangeFirstPasswordForm extends Component {
    render() {
        const { auth, t } = this.props;

        return (
            <Formik initialValues={auth.initialValues.firstPasswordSet}
                validationSchema={
                    yup.object().shape({
                        first_password: yup.string()
                            .label(t('new_password'))
                            .required(t('field_is_required')),
                        first_password_repeat: yup.string()
                            .label(t('repeat_new_password'))
                            .oneOf([yup.ref("password")], t('password_not_the_same'))
                            .required(t('field_is_required'))
                    })
                }
                onSubmit={this.props.action}
            >
                <Form>
                    <Card className="p-4 loading-container">
                        <h5>{t('first_password_change')}</h5>
                        <p className="text-muted mb-3">{t('first_password_change_info')}</p>

                        <Row>
                            <Col lg={12}>
                                <Field component={InputField}
                                    label={t('new_password')}
                                    placeholder={t('new_password')}
                                    name="first_password"
                                    type="password"
                                />
                            </Col>
                            <Col lg={12}>
                                <Field component={InputField}
                                    label={t('repeat_new_password')}
                                    placeholder={t('repeat_new_password')}
                                    type="password"
                                    name="first_password_repeat"
                                />
                            </Col>
                        </Row>

                        <div className="text-right mt-3">
                            <Button variant="primary" type="submit" disabled={auth.isLoading}>
                                {auth.isLoading && <Spinner animation="border" size="sm" className="mr-2" />}
                                {t('change_password')}
                            </Button>
                        </div>
                    </Card>
                </Form>
            </Formik>
        );
    }
}

export default inject("auth")(observer(withTranslation()(observer(ChangeFirstPasswordForm))));