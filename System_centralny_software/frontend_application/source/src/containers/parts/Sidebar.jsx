import React, { Component } from "react";
import { TabContent, TabPane } from "reactstrap";
import { Dropdown, Nav } from "react-bootstrap";
import { NavLink, withRouter } from "react-router-dom";
import SVG from "react-inlinesvg"
import { inject, observer } from "mobx-react";
import routes from "../../config/routes";
import StatusSummary from "./StatusSummary";
import Logo from "./Logo";
import Interference from "../../components/Interference";
import { decorate, observable } from "mobx";
import classNames from "classnames";
import { SOCK } from "../../utils/Constants";
import LanguageSwitch from "../../LanguageSwitch";
import { withTranslation } from "react-i18next";

class Sidebar extends Component {
	activeTab = "1";

	toggle = tab => {
		if (this.activeTab !== tab) this.activeTab = tab;
	};

	getNavLinkClass = (path) => {
		return this.props.location.pathname === path ? "active" : "";
	};

	logout = () => {
		const { socket, auth } = this.props;
		socket.sendMess(SOCK.USER_LOGOUT, { id: auth.id });
	}

	render() {
		const { auth, common, map, alerts, t } = this.props;

		return (
			<div className="sidenav bg-light">
				<div className="sidenav__user">

					<div className="pr-2">
						<p className="text-muted mb-0" >{t('logged_in_as')}</p>
						<span style={{ fontSize: "18px" }}>{auth.name} (ID: {auth.id})</span>
					</div>
					<button className="btn btn-s btn-outline-secondary mr-2" onClick={this.logout}>
						{t('logout')}
					</button>
				</div>

				<div className="sidenav__status">
					<StatusSummary />
				</div>

				<div className="sidenav__nav">
					<div className="nav-active-text-primary">
						<Nav as="ul">
							{auth.permissions.map && <Nav.Item as="li" className={this.getNavLinkClass(routes.home)}>
								<NavLink to={routes.home} activeClassName="active">
									<span className="nav-icon"><SVG src="/svg/map.svg" /></span>
									<span className="nav-text">{t('link_map')}</span>
								</NavLink>
							</Nav.Item>}

							{auth.permissions.manage && <Dropdown as="li" drop="right" className={this.getNavLinkClass(routes.probes.base)}>
								<Dropdown.Toggle as="a" className="nav-item">
									<span className="nav-icon">
										<SVG src={`/svg/settings.svg`} />
									</span>
									<span className="nav-text">{t('link_manage')}</span>
								</Dropdown.Toggle>
								<Dropdown.Menu>
									{auth.permissions.probes && <Dropdown.Item as={NavLink} to={routes.probes.base}>
										{t('probes')}
									</Dropdown.Item>}
									{auth.permissions.users && <Dropdown.Item as={NavLink} to={routes.users.base}>
										{t('users')}
									</Dropdown.Item>}
									{auth.permissions.cs && <Dropdown.Item as={NavLink} to={routes.centralsystem.base}>
										{t('central_system')}
									</Dropdown.Item>}
									{auth.permissions.reports_list && <Dropdown.Item as={NavLink} to={routes.reports.base}>
										{t('reports_list')}
									</Dropdown.Item>}
									{auth.permissions.generate_report && <Dropdown.Item as={NavLink} to={routes.centralsystembasicreport.base}>
										{t('generate_report')}
									</Dropdown.Item>}
									{auth.permissions.signatures_list && <Dropdown.Item as={NavLink} to={routes.signatures.base}>
										{t('signatures_list')}
									</Dropdown.Item>}
								</Dropdown.Menu>
							</Dropdown>}

							{auth.permissions.alerts && <Nav.Item as="li" className={this.getNavLinkClass(routes.alerts.base)}>
								<NavLink to={routes.alerts.base} activeClassName="active">
									<span className="nav-icon"><SVG src="/svg/map.svg" /></span>
									<span className="nav-text">{t('link_alerts')} {alerts.unread}</span>
								</NavLink>
							</Nav.Item>}
						</Nav>
					</div>
				</div>

				{auth.role !== "service" &&
					<div className="sidenav__interferences">

						<div className="nav-header">
							<p className="text-muted mb-0">{t('interferences')}</p>
						</div>
						<Nav variant="pills" className="p-2">
							<Nav.Item>
								{/* eslint-disable-next-line*/}
								<a href="#"
									className={classNames("nav-link btn-xs", { active: this.activeTab === "1" })}
									onClick={() => {
										this.toggle("1");
									}}
								>
									{t('active')} ({map.active.length})
								</a>
							</Nav.Item>
							<Nav.Item>
								{/* eslint-disable-next-line*/}
								<a href="#"
									className={classNames("nav-link btn-xs", { active: this.activeTab === "2" })}
									onClick={() => {
										this.toggle("2");
									}}
								>
									{t('not_active')} ({map.inactive.length})
								</a>
							</Nav.Item>
						</Nav>
						<div className="sidenav__interferences__inner">
							<TabContent activeTab={this.activeTab} className="p-2">
								<TabPane tabId="1">
									{
										map.active.map((interference, index) => {
											return <Interference key={index} map={map} interference={interference} />
										})
									}
								</TabPane>
								<TabPane tabId="2">
									<button className="btn btn-xs btn-outline-secondary" onClick={map.removeExtinguished}>{t('remove_all')}</button>

									{
										map.inactive.map((interference, index) => {
											return <Interference key={index} map={map} interference={interference} />
										})
									}
								</TabPane>
							</TabContent>
						</div>
					</div>}


				<div className="sidenav__footer">
					<LanguageSwitch />
					<div className="p-3 d-flex align-items-center justify-content-between">
						<Logo />

						<label className="md-switch">
							<input type="checkbox" defaultChecked={common.isDarkTheme} onChange={common.changeTheme} />
							<i className="indigo" />
						</label>
					</div>
				</div>
			</div>

		);
	}
}

decorate(Sidebar, {
	activeTab: observable,
});

export default withRouter(inject("common", "auth", "map", "alerts", "socket")(withTranslation()(observer(Sidebar))));