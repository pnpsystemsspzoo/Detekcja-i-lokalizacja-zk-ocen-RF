import React, { Component } from "react";
import { Form as FormikForm, Formik } from "formik";
import { Button, Row, Col, Modal, Spinner } from "react-bootstrap";
import { inject, observer } from "mobx-react";
import { withTranslation } from "react-i18next";

class Block extends Component {
    onSubmit = (event) => {
        const { users } = this.props;
        const user = users.getCurrent;
        users.processing.block = true;
        users.block(user);
    };

    render() {
        const { users, t } = this.props;
        return (
            <Modal show={users.modals.block}
                onHide={() => users.modalToggle("block")}
                centered>
                <Formik onSubmit={this.onSubmit}>
                    <FormikForm>
                        <Modal.Header closeButton>
                            <Modal.Title>{t('block_user')}</Modal.Title>
                        </Modal.Header>

                        <Modal.Body>
                            <Row>
                                <Col lg={12}>
                                    <p>{t('block_user_warning')}</p>
                                </Col>
                            </Row>
                        </Modal.Body>

                        <Modal.Footer>
                            <Button variant="secondary" type="submit" disabled={users.processing.block}>
                                {users.processing.block && <Spinner animation="border" size="sm" className="mr-2" />}
                                {t('block')}
                            </Button>
                        </Modal.Footer>
                    </FormikForm>
                </Formik>
            </Modal>
        );
    }
}

export default inject("users")(withTranslation()(observer(Block)));