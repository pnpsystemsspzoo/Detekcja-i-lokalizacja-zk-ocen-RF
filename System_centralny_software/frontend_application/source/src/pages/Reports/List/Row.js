import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import { decorate, observable } from "mobx";
import { withRouter } from "react-router-dom";
import routes from "../../../config/routes";
import { reverse } from "named-urls";

class Row extends Component {
    dropdownIsOpen = false;

    toggle = () => {
        this.dropdownIsOpen = !this.dropdownIsOpen;
    };

    handleRedirect = () => {
        const { probe, history } = this.props;
        history.push(reverse(routes.probe.base, { id: probe.id }));
    };

    render() {
        const { report, reports } = this.props;

        return (
            <>
                <tr>
                    <td style={{ width: "10%" }}>
                        <input type="checkbox" onChange={() => reports.select(report)} checked={reports.selected.has(report.id)} value={report.id} />
                    </td>
                    <td>{report.id}</td>
                    <td>{report.authorId}</td>
                    <td>{report.authorName}</td>
                    <td>{report.authorRole}</td>
                    <td>{report.creationDate}</td>
                    <td>{report.creationTime}</td>
                </tr>
            </>
        );
    }
}

decorate(Row, {
    dropdownIsOpen: observable
});

export default inject("reports")(withRouter(observer(Row)));