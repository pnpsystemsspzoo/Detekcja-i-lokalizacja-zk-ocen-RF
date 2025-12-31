import React, { Component } from "react";
import { Form as FormikForm, Formik } from "formik";
import { Button, Row, Col, Modal, Spinner } from "react-bootstrap";
import { inject, observer } from "mobx-react";
import { withTranslation } from "react-i18next";

class CloseSession extends Component {
    onSubmit = (event) => {
        const { users } = this.props;
        const user = users.getCurrent;
        users.processing.closeSession = true;
        users.closeSession(user);
    };

    render() {
        const { users, t } = this.props;
        return (
            <Modal show={users.modals.closeSession}
                onHide={() => users.modalToggle("closeSession")}
                centered>
                <Formik onSubmit={this.onSubmit}>
                    <FormikForm>
                        <Modal.Header closeButton>
                            <Modal.Title>{t('end_user_session')}</Modal.Title>
                        </Modal.Header>

                        <Modal.Body>
                            <Row>
                                <Col lg={12}>
                                    <p>{t('end_user_session_warning')}</p>
                                </Col>
                            </Row>
                        </Modal.Body>

                        <Modal.Footer>
                            <Button variant="secondary" type="submit" disabled={users.processing.closeSession}>
                                {users.processing.closeSession && <Spinner animation="border" size="sm" className="mr-2" />}
                                {t('end')}
                            </Button>
                        </Modal.Footer>
                    </FormikForm>
                </Formik>
            </Modal>
        );
    }
}

export default inject("users")(withTranslation()(observer(CloseSession)));