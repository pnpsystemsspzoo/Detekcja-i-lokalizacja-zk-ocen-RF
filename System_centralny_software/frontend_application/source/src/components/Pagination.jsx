/* eslint-disable jsx-a11y/anchor-is-valid */
import React, { Component } from "react";
import { ButtonDropdown, DropdownItem, DropdownMenu, DropdownToggle, Pagination as PaginationC, PaginationItem, PaginationLink } from "reactstrap";
import { decorate, observable } from "mobx";
import { observer } from "mobx-react";
import { withTranslation } from "react-i18next";

class Pagination extends Component {
	dropdownIsOpen = false;
	perPages = [10, 20, 50, 100];

	toggle = () => {
		this.dropdownIsOpen = !this.dropdownIsOpen;
	};

	render() {
		const { current, perPage, total, pages, show, onClick, changePages, t } = this.props;
		let start = current - Math.floor(show / 2);
		start = start < 1 ? 1 : start;
		let from = current * perPage - perPage + 1;
		let to = current * perPage;
		to = to > total ? total : to;

		return (
			pages > 1 && <div className="fixed-table-pagination">
				<div className="float-left pagination-detail">
					{
						(total > this.perPages[0]) && <ButtonDropdown className="mr-3" direction="up" isOpen={this.dropdownIsOpen} toggle={this.toggle}>
							<DropdownToggle caret color="white" size="sm">
								{perPage} {t('rows_per_page')}
							</DropdownToggle>
							<DropdownMenu>
								{
									this.perPages.map(perPage =>
										(perPage <= total) && <DropdownItem key={perPage} onClick={() => changePages(perPage)}>
											{perPage}
										</DropdownItem>
									)
								}
							</DropdownMenu>
						</ButtonDropdown>
					}
					<span className="pagination-info text-muted">
						{t('showing', { from, to, total })}
					</span>
				</div>

				<div className="float-right pagination">
					<PaginationC>
						<PaginationItem disabled={current === 1}>
							<PaginationLink previous onClick={() => onClick(current - 1)} />
						</PaginationItem>

						{
							Array(show).fill(start).map((n, index) =>
								(n + index) <= pages && <PaginationItem key={index} active={current === n + index}>
									<PaginationLink onClick={() => onClick(n + index)}>
										{n + index}
									</PaginationLink>
								</PaginationItem>
							)
						}

						<PaginationItem disabled={current === pages}>
							<PaginationLink next onClick={() => onClick(current + 1)} />
						</PaginationItem>
					</PaginationC>
				</div>
			</div>
		);
	}
}

Pagination.defaultProps = {
	current: 1,
	pages: 1,
	show: 7
};

decorate(Pagination, {
	dropdownIsOpen: observable
});

export default withTranslation()(observer(Pagination));
