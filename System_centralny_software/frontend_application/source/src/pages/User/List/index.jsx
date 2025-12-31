import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import TableHeader from "../../../containers/parts/TableHeader";
import Row from "./Row";
import Loading from "../../../components/Loading";
import PageWrapper from "../../../containers/PageWrapper";
import NoResults from "../../../components/NoResults";
import Pagination from "../../../components/Pagination";
import { decorate, observable } from "mobx";
import { Button, Table, ButtonToolbar } from "react-bootstrap";
import Add from "./Forms/Add";
import Delete from "./Forms/Delete";
import { withTranslation } from 'react-i18next';

class Index extends Component {
	// isLoading = true;

	componentDidMount() {
		const { users } = this.props;
		users.get();
	}

	renderButtons = () => {
		const { users, t } = this.props;

		return (
			<>
				<div className="padding">
					<ButtonToolbar className="justify-content-start" aria-label="Toolbar with button groups">
						<div>
							<Button className="btn-sondy mr-2 mb-2" variant="secondary" onClick={() => users.modalToggle("add")}>{t('add_user')}</Button>
							<Button className="btn-sondy mr-2 mb-2" variant="secondary" disabled={users.selected.size === 0}
								onClick={() => users.modalToggle("delete")}>{t('delete_user')}</Button>
						</div>
					</ButtonToolbar>
				</div>
			</>
		)
	};

	constructColumns = (t) => {
		return [
			{
				label: "",
				select: true,
				isSelected: () => this.props.users.isSelectedEvent(),
				onSelect: event => this.props.users.onSelectEvent(event)
			},
			{ label: "#" },
			{
				label: t('id'),
				sortable: true,
				sort: "id"
			},
			{
				label: t('name'),
				sortable: true,
				sort: "name"
			},
			{
				label: t('last_login'),
				sortable: true,
				sort: "lastLogin"
			},
			{
				label: t('role_in_the_system'),
				sortable: true,
				sort: "role"
			},
			{
				label: t('blocked'),
				sortable: true,
				sort: "blocked"
			},
		]
	}

	render() {
		const { users, t } = this.props;

		return (
			<PageWrapper title={t('users_title')}
				desc={t('users_desc')}
				//  isLoading={users.isLoading}
				buttons={this.renderButtons()}
			>
				<div className="padding">
					{
						users.ready && <div className="loading-container">
							<Loading show={users.isLoading} absolute bg="transparent" />
							<Table className="table-theme v-middle">
								<TableHeader columns={this.constructColumns(t)} store={users} />
								<tbody>
									{
										users.items.map((user, index) =>
											<Row key={index} index={users.getIndex(index)} user={user} />
										)
									}
								</tbody>
							</Table>
							<Pagination {...users.pagination} onClick={users.getPage} />
						</div>
					}
				</div>
				{users.empty && <NoResults label={t('no_users')} />}
				<Add />
				<Delete />
			</PageWrapper>
		);
	}
}

decorate(Index, {
	isLoading: observable
});

export default inject("users")(withTranslation()(observer(Index)));