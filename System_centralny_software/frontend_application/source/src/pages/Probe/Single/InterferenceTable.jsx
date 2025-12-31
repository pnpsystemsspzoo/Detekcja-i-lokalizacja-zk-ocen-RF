import React, { Component } from "react";
import { Table } from "react-bootstrap";
import { inject, observer } from "mobx-react";
import moment from 'moment';
import { withTranslation } from "react-i18next";

class InterferenceTable extends Component {
    lastData = {
        id: '-', longitude: '-', latitude: '-', height: '-', range: '-', heading: '-', speed: '-', location_variance: '-', unix_time: '-', time_var: '-',
        signal_type: '-', signal_period: '-', f0: '-', f0_var: '-', bandwidth: '-', bandwidth_var: '-', J: '-', J_var: '-', field_1: '-', field_2: '-',
        field_3: '-', field_4: '-', devicesId: [], time: '-', duty_cycle: '-', rise_rate_slope: '-'
    };

    currentData = {
        id: '-', longitude: '-', latitude: '-', height: '-', range: '-', heading: '-', speed: '-', location_variance: '-', unix_time: '-', time_var: '-',
        signal_type: '-', signal_period: '-', f0: '-', f0_var: '-', bandwidth: '-', bandwidth_var: '-', J: '-', J_var: '-', field_1: '-', field_2: '-',
        field_3: '-', field_4: '-', devicesId: [], time: '-', duty_cycle: '-', rise_rate_slope: '-'
    };

    componentDidUpdate() {
        const { probe, map } = this.props;

        if (map.active[0] && map.active[0].devicesId.includes(probe.id)) {
            this.currentData = { ...map.active[0], time: map.active[0].time }
            this.lastData = { ...map.active[0], time: map.active[0].time }
        }
    }

    render() {
        const { probe, map, t } = this.props;

        return (
            <div className="probe-information-dynamic">
                <h5>{t('info_interference')} ({map.active[0] && map.active[0].devicesId.includes(probe.id) ? t('active') : t('in_active')})</h5>

                <Table striped bordered hover className="table_active_interference">
                    <tbody>
                        <tr>
                            <td className="name">{t('interference_id')}</td>
                            <td className="value">{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.id : this.lastData.id}</td>
                        </tr>
                        <tr>
                            <td>{t('date')}</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ?
                                (this.currentData.date !== undefined ? moment(this.currentData.date).format("DD-MM-YYYY") : "-") :
                                (this.currentData.date !== undefined ? moment(this.lastData.date).format("DD-MM-YYYY") : "-")
                            }</td>
                        </tr>
                        <tr>
                            <td>{t('time')}</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.time : this.lastData.time}</td>
                        </tr>
                        <tr>
                            <td>{t('time_var')}</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.time_var : this.lastData.time_var}</td>
                        </tr>
                        <tr>
                            <td>{t('longitude_deg')}</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? (this.currentData.longitude).toString() : (this.lastData.longitude).toString()}</td>
                        </tr>
                        <tr>
                            <td>{t('latitude_deg')}</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? (this.currentData.latitude).toString() : (this.lastData.latitude).toString()}</td>
                        </tr>
                        <tr>
                            <td>{t('height')} [m]</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.height : this.lastData.height}</td>
                        </tr>
                        <tr>
                            <td>{t('location_variance')} [m^2]</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.range : this.lastData.range}</td>
                        </tr>
                        <tr>
                            <td>{t('speed')}</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.speed : this.lastData.speed}</td>
                        </tr>
                        <tr>
                            <td>{t('heading')}</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.heading : this.lastData.heading}</td>
                        </tr>
                        <tr>
                            <td>{t('signal_period')} [ms]</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.signal_period : this.lastData.signal_period}</td>
                        </tr>
                        <tr>
                            <td>{t('signal_type')}</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.signal_type : this.lastData.signal_type}</td>
                        </tr>
                        <tr>
                            <td>{t('rise_rate_slope')} [kHz/µs]</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.rise_rate_slope : this.lastData.rise_rate_slope}</td>
                        </tr>
                        <tr>
                            <td>{t('single_pulse_duration')} [µs]</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.field_1 : this.lastData.field_1}</td>
                        </tr>
                        <tr>
                            <td>{t('duty_cycle')} [%]</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.duty_cycle : this.lastData.duty_cycle}</td>
                        </tr>
                        <tr>
                            <td>{t('f0')} [MHz]</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.f0 : this.lastData.f0}</td>
                        </tr>
                        <tr>
                            <td>{t('f0_var')} [MHz^2]</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.f0_var : this.lastData.f0_var}</td>
                        </tr>
                        <tr>
                            <td>{t('band')} [kHz]</td>
                            {/* <td>{interference.bandwidth}</td> */}
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.bandwidth : this.lastData.bandwidth}</td>
                        </tr>
                        <tr>
                            <td>{t('band_var')} [kHz^2]</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.bandwidth_var : this.lastData.bandwidth_var}</td>
                        </tr>
                        <tr>
                            <td>{t('j')} [dBm]</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.J : this.lastData.J}</td>
                        </tr>
                        <tr>
                            <td>{t('j_var')} [dBm^2]</td>
                            <td>{map.active[0] && map.active[0].devicesId.includes(probe.id) ? this.currentData.J_var : this.lastData.J_var}</td>
                        </tr>

                    </tbody>
                </Table>
            </div>
        );
    }
}

export default inject("map")(withTranslation()(observer(InterferenceTable)));