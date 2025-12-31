import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import "leaflet/dist/leaflet.css";
import * as L from "leaflet";

class ExtendedReportMap extends Component {

    componentDidMount() {
        const { map, api } = this.props;

        // query from the server has a secret key, react gets it from URL - if key is valid, send mount request 
        // const queryParams = new URLSearchParams(window.location.search);
        // let key = queryParams.get("key");
        // if (key == "secret") {
        map.instanceExtendedReport = L.map("map").setView([map.config.lat, map.config.lng], map.config.zoom);
        map.initExtendedReport();

        api.report.extended.get_table({})
            .then(res => {
                map.setExtendedReportData(res.data.json);
                map.handleInterferenceExtendedReportLines();
            })
            .catch(err => {
                console.log('err', err)
            });
        // }
    }

    componentWillUnmount() {
        const { map } = this.props;

        if (map.instanceExtendedReport) {
            map.instanceExtendedReport.remove();
            map.instanceExtendedReport = null;
        }
    }

    render() {
        const { map } = this.props;
        return (
            <div className="map d-flex">
                <div className="map__container">
                    <div id="map" />
                </div>
                <div style={{ visibility: 'visible' }} id="reportextendeddiv"></div>
            </div>
        );
    }
}

export default inject("map", "api")(observer(ExtendedReportMap));