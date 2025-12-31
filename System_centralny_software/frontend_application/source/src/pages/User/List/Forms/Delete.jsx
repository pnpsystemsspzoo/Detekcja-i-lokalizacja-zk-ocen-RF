import React, { Component } from "react";
import { Form as FormikForm, Formik } from "formik";
import { Button, Row, Col, Modal, Spinner } from "react-bootstrap";
import { inject, observer } from "mobx-react";
import { withTranslation } from "react-i18next";

class Delete extends Component {
    onSubmit = (event) => {
        const { users } = this.props;
        users.processing.delete = true;
        users.delete();
        users.clearSelected();
    };

    render() {
        const { users, t } = this.props;
        return (
            <Modal show={users.modals.delete}
                onHide={() => users.modalToggle("delete", -1)}
                centered>
                <Formik onSubmit={this.onSubmit}>
                    <FormikForm>
                        <Modal.Header closeButton>
                            <Modal.Title>{t('deleting_user')}</Modal.Title>
                        </Modal.Header>

                        <Modal.Body>
                            <Row>
                                <Col lg={12}>
                                    <p>{t('delete_user_warning')}</p>
                                    {users.selectedItems.map((user) => <li key={user.id}>{user.firstName} {user.lastName}</li>)}
                                </Col>
                            </Row>
                        </Modal.Body>

                        <Modal.Footer>
                            <Button variant="secondary" type="submit" disabled={!users.processing.delete && users.selected.size === 0}>
                                {users.processing.delete && <Spinner animation="border" size="sm" className="mr-2" />}
                                {t('delete')}
                            </Button>
                        </Modal.Footer>
                    </FormikForm>
                </Formik>
            </Modal>
        );
    }
}

export default inject("users")(withTranslation()(observer(Delete)));