import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import "leaflet/dist/leaflet.css";
import PageWrapper from "../containers/PageWrapper";
import * as L from "leaflet";
import InlineSVG from "react-inlinesvg";
import { decorate, observable } from "mobx";
import { Field, Form, Formik } from "formik";
import { InputField } from "../components/forms";
import classNames from "classnames";
import { Row, Col, Modal, OverlayTrigger, Popover, Button } from "react-bootstrap";
import { withTranslation } from "react-i18next";

class Map extends Component {
	modalOpened = false;
	formRef = React.createRef();
	target = React.createRef();

	toggle = () => {
		this.modalOpened = !this.modalOpened;
	};

	onSubmit = values => {
		const { map } = this.props;
		map.saveConfig(values.lat, values.lng, values.zoom, values.opacity);
		this.toggle();
	};

	componentDidMount() {
		const { map, probes, auth } = this.props;
		// probes.isLoading = true;

		map.instance = L.map("map").setView([map.config.lat, map.config.lng], map.config.zoom);
		map.init();

		map.wmsLayer.on("load", () => {
			probes.loadMarkers(map.instance);
		});

		map.loadInterferences();

		auth.timerId = setInterval(() => {
			probes.clearDetectedProbes();
		}, 2000);
	}

	componentWillUnmount() {
		const { map, auth } = this.props;
		if (map.instance) {
			map.instance.remove();
			map.instance = null;
		}

		auth.clearTimer();
	}

	renderLayersPopover = () => {
		const { map } = this.props;

		return <Popover id="popover-basic">
			<Popover.Title as="h3">Popover right</Popover.Title>
			<Popover.Content class="map__layers">
				{
					map.layers.map(group => {
						return <React.Fragment key={group.title}>
							<p>{group.title}</p>
							<div className="btn-group">
								{
									group.layers.map(layer => {
										const classes = classNames("btn btn-secondary", {
											active: layer.active
										});

										return <button key={layer.name} className={classes} onClick={() => map.toggleLayer(layer)}>
											{layer.title}
										</button>
									})
								}
							</div>
						</React.Fragment>
					})
				}
			</Popover.Content>
		</Popover>
	};


	render() {
		const { map, probes, t } = this.props;

		return (
			<PageWrapper title={t('map_title')} className="page-container--full" isLoading={probes.isLoading} showTitle={false}>
				<div className="map d-flex">
					<div className="map__container">
						<div className="map-controls">
							<div className="map-controls__container">
								<OverlayTrigger trigger="click" placement="right" overlay={this.renderLayersPopover()} rootClose={true}>
									<button id="map-layers" title={t('layers')}><InlineSVG src="svg/layers.svg" /></button>
								</OverlayTrigger>

								<button onClick={map.center} title={t('center_map')}><InlineSVG src="svg/center.svg" /></button>
							</div>
							<div className="map-controls__container">
								<button onClick={this.toggle} title={t('map_settings')}><InlineSVG src="svg/settings.svg" /></button>
							</div>
						</div>
						<div id="map" />
					</div>
				</div>

				<Modal show={this.modalOpened}
					onHide={() => (this.modalOpened = false)}
					centered>
					<Formik initialValues={map.config}
						onSubmit={this.onSubmit}
						ref={this.formRef}
					>
						<Form>
							<Modal.Header closeButton>
								<Modal.Title>{t('map_settings')}</Modal.Title>
							</Modal.Header>

							<Modal.Body>
								<Row>
									<Col lg={12}>
										<p><strong>{t('base_map_position')}</strong></p>
									</Col>
									<Col lg={4}>
										<Field component={InputField}
											label="Latitude"
											name="lat"
										/>
									</Col>

									<Col lg={4}>
										<Field component={InputField}
											label="Longitude"
											name="lng"
										/>
									</Col>
									<Col lg={4}>
										<Field component={InputField}
											label="Zoom"
											name="zoom"
										/>
									</Col>
								</Row>

								<Row>
									<Col lg={12}>
										<Field component={InputField}
											label={t('transparency_of_inactive_interferences')}
											type="range"
											min={0.01}
											max={1}
											step={0.01}
											name="opacity"
										/>
										<Field component={InputField}
											type="number"
											min={0.01}
											max={1}
											step={0.01}
											name="opacity"
										/>
									</Col>
								</Row>
							</Modal.Body>

							<Modal.Footer>
								<Button variant="secondary" type="submit">{t('save')}</Button>
							</Modal.Footer>
						</Form>
					</Formik>
				</Modal>
			</PageWrapper>
		);
	}
}

decorate(Map, {
	modalOpened: observable
});

export default inject("common", "probes", "map", "socket", "auth")(withTranslation()(observer(Map)));