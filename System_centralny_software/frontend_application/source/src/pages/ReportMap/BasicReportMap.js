import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import "leaflet/dist/leaflet.css";
import * as L from "leaflet";

class BasicReportMap extends Component {

    componentDidMount() {
        const { map, api } = this.props;

        // query from the server has a secret key, react gets it from URL - if key is valid, send mount request 
        const queryParams = new URLSearchParams(window.location.search);
        let key = queryParams.get("key");
        if (key === "secret") {
            map.instanceBasicReport = L.map("map").setView([map.config.lat, map.config.lng], map.config.zoom);
            map.initBasicReport();

            api.report.basic.get_table()
                .then(res => {
                    map.setBasicReportData(res.data);
                    map.handleInterferenceReportPins();
                })
                .catch(err => {
                    console.log('err', err)
                });
        }
    }

    componentWillUnmount() {
        const { map } = this.props;

        if (map.instanceBasicReport) {
            map.instanceBasicReport.remove();
            map.instanceBasicReport = null;
        }
    }

    render() {
        const { map } = this.props;
        return (
            <div className="map d-flex">
                <div className="map__container">
                    <div id="map" />
                </div>
                <div style={{ visibility: 'visible' }} id="reportdiv"></div>
            </div>
        );
    }
}

export default inject("map", "api")(observer(BasicReportMap));