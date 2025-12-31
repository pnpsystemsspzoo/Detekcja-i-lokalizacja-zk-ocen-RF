import React, { Component } from "react";
import { Button, Table, ButtonToolbar } from "react-bootstrap";
import { inject, observer } from "mobx-react";
import TableHeader from "../../../containers/parts/TableHeader";
import Row from "./Row";
import Loading from "../../../components/Loading";
import PageWrapper from "../../../containers/PageWrapper";
import NoResults from "../../../components/NoResults";
import Pagination from "../../../components/Pagination";
import { Field, Form, Formik } from "formik";
import { InputField } from "../../../components/forms";
import { SOCK } from "../../../utils/Constants";
import Delete from "./Forms/Delete";
import Reset from "./Forms/Reset";
import Add from "./Forms/Add";
import FirmwareUpload from "./Forms/FirmwareUpload";
import ConfigUpload from "./Forms/ConfigUpload";
import Disable from "./Forms/Disable";
import { withTranslation } from 'react-i18next';

class Index extends Component {
	/**
	 * Reference to HTML checkbox
	 * @type {React.RefObject<unknown>}
	 */
	bulkSelect = React.createRef();

	constructColumns = (t) => {
		return [
			{
				label: "",
				select: true,
				isSelected: () => this.props.probes.isSelectedEvent(),
				onSelect: event => this.props.probes.onSelectEvent(event)
			},
			{
				label: t('probe_id'),
				sortable: true,
				sort: "id",
			},
			{ label: t('hardware') },
			{ label: t('system') },
			{ label: t('communication') },
			{ label: t('probe_panel') },
			{ label: t('delete_probe') }
		]
	}

	componentDidMount() {
		const { probes, socket } = this.props;
		probes.isLoading = false;
		socket.send(SOCK.ADMIN_INTERFACE_QUERY);
		socket.send(SOCK.DEVICES_MANAGEMENT_QUERY);
	}

	renderButtons = () => {
		const { probes, t } = this.props;

		return (
			<div className="padding">
				<ButtonToolbar className="justify-content-start" aria-label="Toolbar with button groups">
					<div>
						<Button className="btn-sondy mr-2 mb-2" variant="secondary" onClick={() => probes.modalToggle("add")}>{t('add_probe')}</Button>
						<Button className="btn-sondy mr-2 mb-2" variant="secondary" disabled={probes.selected.size === 0} onClick={() => probes.modalToggle("delete")}>{t('delete_probes')}</Button>
						<Button className="btn-sondy mr-2 mb-2" variant="secondary" disabled={probes.selected.size === 0} onClick={() => probes.modalToggle("reset")}>{t('reset_probes')}</Button>
						<Button className="btn-sondy mr-2 mb-2" variant="secondary" disabled={probes.selected.size === 0} onClick={() => probes.modalToggle("disable")}>{t('disable_probes')}</Button>
					</div>
					<div>
						<Button className="btn-sondy mr-2 mb-2" variant="secondary" disabled={probes.selected.size === 0} onClick={() => probes.modalToggle("configUpload")}>{t('change_config')}</Button>
						<Button className="btn-sondy mr-2 mb-2" variant="secondary" disabled={probes.selected.size === 0} onClick={() => probes.modalToggle("firmwareUpload")}>{t('add_firmware')}</Button>
						<Button className="btn-sondy mr-2 mb-2" onClick={probes.toggleFiltering} type="button" variant="secondary" size="sm">{t('filter_find_probes')}</Button>
					</div>
				</ButtonToolbar>
			</div>
		)
	};


	render() {
		const { probes, t } = this.props;

		return (
			<PageWrapper title={t('probes_title')}
				desc={t('probes_desc')}
				isLoading={probes.isLoading}
				buttons={this.renderButtons()}
			>
				<div className="padding">
					{
						!probes.empty && probes.ready && <div className="loading-container">
							<Loading show={probes.isLoading} fixed bg />

							{probes.isFilteringOpen === true ? (
								<>
									<Formik initialValues={{
										id: "", mac: "", ip: "",
										card0: "", card1: "", card2: "", card3: "",
										expirationDateStart: "", expirationDateEnd: "",
										creationDateStart: "", creationDateEnd: "",
									}}
										onSubmit={probes.setFilters}>
										<Form className="form-inline d-flex justify-content-between">
											<div className="d-flex flex-wrap">
												<Field component={InputField} type="number" name="id" bsSize="sm" className="mr-2 mb-2" placeholder={t('probe_id')} />
												<Field component={InputField} type="text" name="mac" bsSize="sm" className="mr-2 mb-2" placeholder={t('mac')} />
												<Field component={InputField} type="text" name="ip" bsSize="sm" className="mr-2 mb-2" placeholder={t('ip')} />

												<Field component={InputField} type="select" className="mr-2 mb-2" name="card0" bsSize="sm">
													<option value={""}>{t('card1')}</option>
													<option value={"ILS1"}>ILS 100 (ILS1)</option>
													<option value={"ILS2"}>ILS 330 (ILS2)</option>
													<option value={"TETRA"}>TETRA</option>
													<option value={"L1"}>GNSS (L1)</option>
												</Field>
												<Field component={InputField} type="select" className="mr-2 mb-2" name="card1" bsSize="sm">
													<option value={""}>{t('card2')}</option>
													<option value={"ILS1"}>ILS 100 (ILS1)</option>
													<option value={"ILS2"}>ILS 330 (ILS2)</option>
													<option value={"TETRA"}>TETRA</option>
													<option value={"L1"}>GNSS (L1)</option>
												</Field>
												<Field component={InputField} type="select" className="mr-2 mb-2" name="card2" bsSize="sm">
													<option value={""}>{t('card3')}</option>
													<option value={"ILS1"}>ILS 100 (ILS1)</option>
													<option value={"ILS2"}>ILS 330 (ILS2)</option>
													<option value={"TETRA"}>TETRA</option>
													<option value={"L1"}>GNSS (L1)</option>
												</Field>
												<Field component={InputField} type="select" className="mr-2 mb-2" name="card3" bsSize="sm">
													<option value={""}>{t('card4')}</option>
													<option value={"ILS1"}>ILS 100 (ILS1)</option>
													<option value={"ILS2"}>ILS 330 (ILS2)</option>
													<option value={"TETRA"}>TETRA</option>
													<option value={"L1"}>GNSS (L1)</option>
												</Field>

											</div>
											<div className="d-flex flex-wrap">
												<Field component={InputField} type="date" name="expirationDateStart" bsSize="sm" className="mr-2 mb-2" />
												<Field component={InputField} type="date" name="expirationDateEnd" bsSize="sm" className="mr-2 mb-2" />

												<Field component={InputField} type="date" name="creationDateStart" bsSize="sm" className="mr-2 mb-2" />
												<Field component={InputField} type="date" name="creationDateEnd" bsSize="sm" className="mr-2 mb-2" />
											</div>

											<Button type="submit" variant="secondary" size="sm">{t('apply')}</Button>
										</Form>
									</Formik>
								</>
							) : (
								<div></div>
							)}

							<Table className="table-theme table-theme--small v-middle text-center">
								<TableHeader columns={this.constructColumns(t)} store={probes} />
								<tbody>
									{
										probes.items.map((probe, index) =>
											<Row key={index} index={probes.getIndex(index)} probe={probe} />
										)
									}
								</tbody>
								<tfoot>
								</tfoot>
							</Table>

							<Pagination {...probes.pagination} onClick={probes.getPage} />
						</div>
					}

					{probes.empty && <NoResults label={t('no_probes')} />}

					<Delete />
					<Add />
					<Reset />
					<ConfigUpload />
					<FirmwareUpload />
					<Disable />
				</div>
			</PageWrapper>
		);
	}
}

export default inject("probes", "socket", "toasts", "alerts")(withTranslation()(observer(Index)));