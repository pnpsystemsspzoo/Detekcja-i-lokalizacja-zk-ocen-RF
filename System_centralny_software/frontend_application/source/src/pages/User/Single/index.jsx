import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import { Card, CardBody } from "reactstrap";
import InlineSVG from "react-inlinesvg";
import routes from "../../../config/routes";
import { Link, withRouter } from "react-router-dom";
import CardHeader from "reactstrap/es/CardHeader";
import { nameToGradient } from "../../../utils/Helpers";
import { Button } from "reactstrap";
import PageWrapper from "../../../containers/PageWrapper";
import Form from 'react-bootstrap/Form';
import { decorate, observable } from "mobx";
import Edit from "../List/Forms/Edit";
import Delete from "../List/Forms/Delete";
import CloseSession from "../List/Forms/CloseSession";
import Block from "../List/Forms/Block";
import Unblock from "../List/Forms/Unblock";
import { withTranslation } from "react-i18next";

class Single extends Component {
	log = "";
	sessionTimeString = "0h 0min 0sec";

	state = {
		permissionsLoading: true,
		permissions: {
			map: false,
			manage: false,
			probes: false,
			users: false,
			cs: false,
			reports_list: false,
			generate_report: false,
			signatures_list: false,
			permissions: false,
			alerts: false,
		}
	}

	constructor(props) {
		super(props);
		const { users, api, match: { params } } = this.props;
		users.get();
		users.setCurrent(params.id);

		api.user.logs.get({ id: params.id })
			.then(res => {
				this.log = res.data.logs;
			})
			.catch((err) => {
				console.log('err', err)
			})
	}

	componentDidUpdate() {
		const { users } = this.props;
		const user = users.getCurrent;

		if (user && !user.is_logged) this.sessionTimeString = "0h 0min 0sec";
	}

	componentDidMount() {
		const { users, common, api, t } = this.props;
		this.isComponentMounted = true;

		common.setTitle(t('user_preview'));
		this.getSessionTime();

		let intervalId = setInterval(() => {
			const user = users.getCurrent;
			if (user) {
				clearInterval(intervalId);

				api.user.permissions.get({ id: user.id })
					.then(res => {
						if (res.data.permissions.userid === user.id) {
							delete res.data.permissions.id; delete res.data.permissions.userid;

							res.data.permissions.alerts = res.data.permissions.alerts === "yes" ? true : false;
							res.data.permissions.manage = res.data.permissions.manage === "yes" ? true : false;
							res.data.permissions.probes = res.data.permissions.probes === "yes" ? true : false;
							res.data.permissions.users = res.data.permissions.users === "yes" ? true : false;
							res.data.permissions.cs = res.data.permissions.cs === "yes" ? true : false;
							res.data.permissions.reports_list = res.data.permissions.reports_list === "yes" ? true : false;
							res.data.permissions.generate_report = res.data.permissions.generate_report === "yes" ? true : false;
							res.data.permissions.signatures_list = res.data.permissions.signatures_list === "yes" ? true : false;
							res.data.permissions.permissions = res.data.permissions.permissions === "yes" ? true : false;
							res.data.permissions.map = res.data.permissions.map === "yes" ? true : false;

							if (this.isComponentMounted) {
								this.setState({ permissions: res.data.permissions });
								this.setState({ permissionsLoading: false });
							}
						}

					})
					.catch(err => {
						console.log('err', err)
					})
			}
		}, 1000);
	}

	componentWillUnmount() {
		this.isComponentMounted = false;
	}

	toggleCheckboxesPermissions = (what) => {
		const p = { ...this.state.permissions };

		if (what !== "manage") { // jeżeli kliknięty checkbox to nie Zarządzaj:
			if (what === "map" || what === "alerts") { // jeżeli kliknięty checkbox to mapa i alerty, to ustaw ich wartość na przeciwną 
				p[what] = !p[what];
			}
			else if (what !== "map" || what !== "alerts") { // jeżeli kliknięty checkbox to probes, users, cs, reports_list, generate_report, signatures_list albo permissions 
				p[what] = !p[what];
				if (p.probes === false && p.users === false && p.cs === false && p.reports_list === false && p.generate_report === false
					&& p.signatures_list === false && p.permissions === false) p.manage = false;

				if (p.probes === true && p.users === true && p.cs === true && p.reports_list === true && p.generate_report === true
					&& p.signatures_list === true && p.permissions === true) p.manage = true;

				if (p.probes === true || p.users === true || p.cs === true || p.reports_list === true || p.generate_report === true
					|| p.signatures_list === true || p.permissions === true) p.manage = true;
			}

		}
		else if (what === "manage") { // jeżeli kliknięty checkbox Zarządzaj
			if (p.manage === false) {
				p.manage = p.probes = p.users = p.cs = p.reports_list = p.generate_report = p.signatures_list = p.permissions = true;
			}
			else {
				p.manage = p.probes = p.users = p.cs = p.reports_list = p.generate_report = p.signatures_list = p.permissions = false;
			}
		}
		this.setState({ permissions: p })
	}

	savePermissions = (e) => {
		e.preventDefault();
		const { users } = this.props;
		const user = users.getCurrent;

		e.preventDefault();
		users.savePermissions(user.id, { ...this.state.permissions });
	}

	_getSessionTime = (e) => {
		e.preventDefault();
		this.getSessionTime();
	}

	getSessionTime = () => {
		const { api, match: { params } } = this.props;
		api.user.session.get_time({ id: params.id })
			.then(res => {
				this.msToTime(res.data.sessionTimeInMs);
			})
			.catch((err) => {
				console.log('err', err)
			})
	}

	msToTime(time) {
		var ms = time % 1000;
		time = (time - ms) / 1000;
		var seconds = time % 60;
		time = (time - seconds) / 60;
		var minutes = time % 60;
		var hours = (time - minutes) / 60;

		this.sessionTimeString = hours + "h " + minutes + "min " + seconds + "sec";
	}

	clearFile = () => {
		const { api, match: { params } } = this.props;
		api.user.logs.clear({ id: params.id })
			.then(res => {
				this.log = res.data.logs;
			})
			.finally(() => {
			})
	}

	editUser = () => {
		const { users } = this.props;
		const user = users.getCurrent;

		users.setUserToEdit(user);
		users.modalToggle("edit");
	}

	closeSession = () => this.props.users.modalToggle("closeSession");
	block = () => this.props.users.modalToggle("block");
	unblock = () => this.props.users.modalToggle("unblock");

	render() {
		const { users, auth, t } = this.props;
		const user = users.getCurrent;

		return (
			<PageWrapper isLoading={users.isLoading}>
				<div className="padding">
					{
						!users.isLoading && user && <>
							<div className="toolbar mb-2">
								<Link to={routes.users.base} className="btn btn-sm btn-white">
									<InlineSVG src="/svg/back.svg" /> &nbsp; {t('back_to_list')}
								</Link>
							</div>

							<Card>
								<CardHeader className="p-4 d-sm-flex align-items-center no-shrink">
									<figure className="avatar w-96 mr-3 mb-md-0 mb-sm-4 text-md" {...nameToGradient(user.name)}>
										{user.initial}
										{user.is_logged && <span className="avatar-status on b-white avatar-right" />}
									</figure>
									<div>
										<h2 className="text-md">{user.firstName} {user.lastName}</h2>
										<div className="text-muted">{user.email}</div>
										<p>{t('user_role_in_system')} {user.role} {user.blocked === 'yes' && <span>{t('user_blocked')}</span>}</p>
									</div>
									<div className="align-self-start ml-auto">
										{user.blocked === 'no' && <Button className="mr-2" onClick={() => this.block()} variant="secondary" type="submit">{t('block_user')}</Button>}
										{user.blocked === 'yes' && <Button className="mr-2" onClick={() => this.unblock()} variant="secondary" type="submit">{t('unblock_user')}</Button>}
										<Button className="mr-2" onClick={() => this.closeSession()} variant="secondary" type="submit" disabled={!user.is_logged}>{t('end_user_session')}</Button>
										<Button className="mr-2" onClick={() => this.editUser()} variant="secondary" type="submit">{t('edit_user')}</Button>
										<Button className="mr-2" onClick={() => this.clearFile()} variant="secondary" type="submit">{t('clear_logs')}</Button>
									</div>
								</CardHeader>
								<CardBody>

									<Form>
										{user && user.is_logged && <div style={{ marginBottom: 20, borderBottom: "1px solid #F1F3F5" }}>
											<Form.Label><h6>{t('session_duration')} {this.sessionTimeString}
												<button className="btn btn-link" style={{ marginTop: -4, marginLeft: 10 }} onClick={(e) => this._getSessionTime(e)}>{t('refresh')}</button>
											</h6></Form.Label>
										</div>}


										{auth.permissions.permissions &&
											<div style={{ marginBottom: 20, paddingBottom: 20, borderBottom: "1px solid #F1F3F5" }}>
												<h6>{t('permissions_to')}</h6>
												{this.state.permissionsLoading && <div>{t('loading_user_permissions')}</div>}
												{!this.state.permissionsLoading &&
													<div>
														<div className="permissionsBox">
															<div><label><input type="checkbox" value={this.state.permissions.map} checked={this.state.permissions.map}
																onChange={() => { this.toggleCheckboxesPermissions("map") }} />Mapa</label>
															</div>
														</div>
														<div className="permissionsBox">
															<div><label><input type="checkbox" value={this.state.permissions.manage} checked={this.state.permissions.manage}
																onChange={() => { this.toggleCheckboxesPermissions("manage") }} />Zarządzaj</label>
															</div>
														</div>
														<div className="permissionsBox">
															<div><label><input type="checkbox" value={this.state.permissions.probes} checked={this.state.permissions.probes}
																onChange={() => { this.toggleCheckboxesPermissions("probes") }} />Sondy</label>
															</div>
															<div><label><input type="checkbox" value={this.state.permissions.users} checked={this.state.permissions.users}
																onChange={() => { this.toggleCheckboxesPermissions("users") }} />Użytkownicy</label>
															</div>
															<div><label><input type="checkbox" value={this.state.permissions.cs} checked={this.state.permissions.cs}
																onChange={() => { this.toggleCheckboxesPermissions("cs") }} />System centralny</label>
															</div>
															<div><label><input type="checkbox" value={this.state.permissions.reports_list} checked={this.state.permissions.reports_list}
																onChange={() => { this.toggleCheckboxesPermissions("reports_list") }} />Lista raportów</label>
															</div>
															<div><label><input type="checkbox" value={this.state.permissions.generate_report} checked={this.state.permissions.generate_report}
																onChange={() => { this.toggleCheckboxesPermissions("generate_report") }} />Generuj raport</label>
															</div>
															<div><label><input type="checkbox" value={this.state.permissions.signatures_list} checked={this.state.permissions.signatures_list}
																onChange={() => { this.toggleCheckboxesPermissions("signatures_list") }} />Lista sygnatur</label>
															</div>
															<div><label><input type="checkbox" value={this.state.permissions.permissions} checked={this.state.permissions.permissions}
																onChange={() => { this.toggleCheckboxesPermissions("permissions") }} />Uprawnienia</label>
															</div>
														</div>
														<div className="permissionsBox">
															<div><label><input type="checkbox" value={this.state.permissions.alerts} checked={this.state.permissions.alerts}
																onChange={() => { this.toggleCheckboxesPermissions("alerts") }} />Alerty</label>
															</div>
														</div>
														<Button className="mr-2" onClick={(e) => this.savePermissions(e)} variant="secondary">{t('save')}</Button>

													</div>
												}
											</div>}

										<Form.Label><h6>{t('activity')}</h6></Form.Label>
										<Form.Control readOnly value={this.log} as="textarea" rows="30" />
									</Form>
								</CardBody>
							</Card>
						</>
					}
				</div>
				<Edit />
				<Delete />
				<CloseSession />
				<Block />
				<Unblock />
			</PageWrapper>
		);
	}
}

decorate(Single, {
	log: observable,
	sessionTimeString: observable,
	permissions: observable
});

export default inject("users", "common", "api", "auth")(withTranslation()(withRouter(observer(Single))));
