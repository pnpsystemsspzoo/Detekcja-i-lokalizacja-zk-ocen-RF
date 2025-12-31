import React, { Component } from "react";
import { Table } from "react-bootstrap";
import { inject, observer } from "mobx-react";
import TableHeader from "../../../containers/parts/TableHeader";
import Row from "../List/Row";
import Loading from "../../../components/Loading";
import PageWrapper from "../../../containers/PageWrapper";
import NoResults from "../../../components/NoResults";
import Pagination from "../../../components/Pagination";
import SingleAlert from "../Single/SingleAlert";
import { withTranslation } from "react-i18next";

class Alerts extends Component {
	constructColumnList = (t) => {
		return [
			{ label: "#" },
			{
				label: t('alert_id'),
				sortable: true,
				sort: "id"
			},
			{
				label: t('date'),
				sortable: true,
				sort: "date"
			},
			{ label: t('description') },
			{ label: t('status') },
			{ label: t('delete') }
		];
	}

	render() {
		const { alerts, t } = this.props;

		return (
			<PageWrapper title={t('alerts_title')}
				desc={t('alerts_desc')}
				isLoading={alerts.isLoading}
			>
				<div className="padding">
					{
						alerts.ready && <div className="loading-container">
							<Loading show={alerts.isLoading} fixed bg />

							<Table className="table-theme table-theme--small v-middle">
								<TableHeader columns={this.constructColumnList(t)} store={alerts} />
								<tbody>
									{
										alerts.items.map((alert, index) => {
											return <Row key={index} index={alerts.getIndex(index)} alert={alert} />
										})
									}
								</tbody>
								<tfoot>

								</tfoot>
							</Table>

							<Pagination {...alerts.pagination} onClick={alerts.getPage} />
						</div>
					}

					{alerts.empty && <NoResults label={t('no_alerts')} />}

					<SingleAlert />
				</div>
			</PageWrapper>
		);
	}
}


export default inject("alerts", "socket", "toasts")(withTranslation()(observer(Alerts)));