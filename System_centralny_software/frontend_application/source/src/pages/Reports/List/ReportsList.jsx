import React from 'react'
import { Table, ButtonToolbar, Button } from "react-bootstrap";
import { inject, observer } from "mobx-react";
import TableHeader from "../../../containers/parts/TableHeader";
import Row from "../List/Row";
import Loading from "../../../components/Loading";
import PageWrapper from "../../../containers/PageWrapper";
import NoResults from "../../../components/NoResults";
import Pagination from "../../../components/Pagination";
import { withTranslation } from 'react-i18next';

class ReportsList extends React.Component {
    columns = [
        {
            label: "",
            select: true,
            isSelected: () => this.props.reports.isSelectedEvent(),
            onSelect: event => this.props.reports.onSelectEvent(event)
        },
        {
            label: this.props.t('report_id'),
            sortable: true,
            sort: "id"
        },
        {
            label: this.props.t('author_id'),
            sortable: true,
            sort: "authorid"
        },
        {
            label: this.props.t('author_name'),
            sortable: true,
            sort: "authorname"
        },
        {
            label: this.props.t('author_role'),
            sortable: true,
            sort: "authorrole"
        },
        {
            label: this.props.t('report_creation_date'),
            sortable: true,
            sort: "creationdate"
        },
        {
            label: this.props.t('report_creation_time'),
            sortable: true,
            sort: "creationtime"
        },
    ];

    componentDidMount() {
        this.props.reports.getAll();
    }

    renderButtons = () => {
        const { reports, t } = this.props;
        return (
            <div className="padding">
                <ButtonToolbar className="justify-content-start" aria-label="Toolbar with button groups">
                    <div>
                        <Button className="btn-sondy mr-2 mb-2" variant="secondary"
                            disabled={reports.selected.size === 0} onClick={() => reports.download()}>{t('download_selected_reports')}</Button>
                    </div>
                </ButtonToolbar>
            </div>
        )
    };

    render() {
        const { reports, t } = this.props;
        return (
            <PageWrapper title={t('reports_list_title')}
                desc={t('reports_list_desc')}
                isLoading={reports.isLoading}
                buttons={this.renderButtons()}
            >
                <div className="padding">
                    {
                        reports.ready && <div className="loading-container">
                            <Loading show={reports.isLoading} fixed bg />

                            <Table className="table-theme table-theme--small v-middle">
                                <TableHeader columns={this.columns} store={reports} />
                                <tbody>
                                    {
                                        reports.items.map((report, index) => {
                                            return <Row key={index} index={reports.getIndex(index)} report={report} />
                                        })
                                    }
                                </tbody>
                                <tfoot>

                                </tfoot>
                            </Table>

                            <Pagination {...reports.pagination} onClick={reports.getPage} />

                            <div className="small__info">
                                {t('reports_info')}
                            </div>
                        </div>
                    }

                    {reports.empty && <NoResults label={t('no_reports')} />}
                </div>
            </PageWrapper>
        );
    }
}

export default inject("reports")(withTranslation()(observer(ReportsList)));