import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import { decorate, observable } from "mobx";
import { withRouter } from "react-router-dom";
import routes from "../../../config/routes";
import { reverse } from "named-urls";
import moment from 'moment';

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
        const { signature } = this.props;

        return (
            <>
                <tr>
                    <td>{signature.id}</td>
                    <td>{moment(signature.date).format("DD-MM-YYYY")}</td>
                    <td>{signature.time}</td>
                    <td>{signature.time_var}</td>
                    <td>{signature.latitude_first}</td>
                    <td>{signature.latitude_last}</td>
                    <td>{signature.longitude_first}</td>
                    <td>{signature.longitude_last}</td>
                    <td>{signature.height_first}</td>
                    <td>{signature.height_last}</td>
                    <td>{signature.range_last}</td>
                    <td>{signature.speed_max}</td>
                    <td>{signature.heading_last}</td>

                    <td>{signature.signal_period}</td>
                    <td>{signature.signal_type}</td>
                    <td>{signature.signal_id}</td>
                    <td>{signature.pulse_rise_time}</td>
                    <td>{signature.time_to_return}</td>
                    <td>{signature.rise_rate_slope}</td>
                    <td>{signature.single_pulse_duration}</td>
                    <td>{signature.double_pulse_duration}</td>
                    <td>{signature.duty_cycle}</td>
                    <td>{signature.f0}</td>
                    <td>{signature.f0_var}</td>
                    <td>{signature.band}</td>
                    <td>{signature.band_var}</td>
                    <td>{signature.j}</td>
                    <td>{signature.j_var}</td>
                </tr>
            </>
        );
    }
}

decorate(Row, {
    dropdownIsOpen: observable
});

export default inject("signatures")(withRouter(observer(Row)));