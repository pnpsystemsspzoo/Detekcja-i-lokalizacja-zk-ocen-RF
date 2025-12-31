import React from 'react'
import { Table } from "react-bootstrap";
import { inject, observer } from "mobx-react";
import TableHeader from "../../../containers/parts/TableHeader";
import Row from "../List/Row";
import Loading from "../../../components/Loading";
import PageWrapper from "../../../containers/PageWrapper";
import NoResults from "../../../components/NoResults";
import Pagination from "../../../components/Pagination";
import { withTranslation } from 'react-i18next';

class SignaturesList extends React.Component {
    componentDidMount() {
        this.props.signatures.getAll();
    }

    render() {
        const { signatures, t } = this.props;

        return (
            <PageWrapper title={t('signatures_title')}
                desc={t('signatures_desc')}
                isLoading={signatures.isLoading}
            >
                <div className="padding">
                    {
                        signatures.ready && <div className="loading-container">
                            <Loading show={signatures.isLoading} fixed bg />

                            <Table responsive className="table-theme table-theme--small v-middle my-custom-table">
                                <TableHeader columns={signatures.columns} store={signatures} />
                                <tbody>
                                    {
                                        signatures.items.map((signature, index) => {
                                            return <Row key={index} index={signatures.getIndex(index)} signature={signature} />
                                        })
                                    }
                                </tbody>
                            </Table>

                            <Pagination {...signatures.pagination} onClick={signatures.getPage} />

                            <div className="small__info">
                                {t('signatures_info')}
                            </div>
                        </div>
                    }

                    {signatures.empty && <NoResults label={t('no_signatures')} />}
                </div>
            </PageWrapper>
        );
    }
}

export default inject("signatures")(withTranslation()(observer(SignaturesList)));