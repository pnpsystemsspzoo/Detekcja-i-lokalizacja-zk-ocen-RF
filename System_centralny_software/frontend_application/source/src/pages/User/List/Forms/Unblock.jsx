import React, { Component } from "react";
import { Form as FormikForm, Formik } from "formik";
import { Button, Row, Col, Modal, Spinner } from "react-bootstrap";
import { inject, observer } from "mobx-react";
import { withTranslation } from "react-i18next";

class Unblock extends Component {
    onSubmit = (event) => {
        const { users } = this.props;
        const user = users.getCurrent;
        users.processing.unblock = true;
        users.unblock(user);
    };

    render() {
        const { users, t } = this.props;
        return (
            <Modal show={users.modals.unblock}
                onHide={() => users.modalToggle("unblock")}
                centered>
                <Formik onSubmit={this.onSubmit}>
                    <FormikForm>
                        <Modal.Header closeButton>
                            <Modal.Title>{t('unblock_user')}</Modal.Title>
                        </Modal.Header>

                        <Modal.Body>
                            <Row>
                                <Col lg={12}>
                                    <p>{t('unblock_user_warning')}</p>
                                </Col>
                            </Row>
                        </Modal.Body>

                        <Modal.Footer>
                            <Button variant="secondary" type="submit" disabled={users.processing.unblock}>
                                {users.processing.unblock && <Spinner animation="border" size="sm" className="mr-2" />}
                                {t('unblock_user')}
                            </Button>
                        </Modal.Footer>
                    </FormikForm>
                </Formik>
            </Modal>
        );
    }
}

export default inject("users")(withTranslation()(observer(Unblock)));