import React, { Component } from "react";
import ChartComponent from "react-chartjs-2";
import { inject } from "mobx-react";

class SpectreChart extends Component {
	render() {
		const { measurement } = this.props;
		// const w = window.innerWidth - 100;
		// const h = window.innerHeight / 2 - 200;
		return (
			<div className="chart chart--spectre">
				<ChartComponent ref={measurement.spectogram}
					data={{
						type: "line",
						labels: [0, .1, .2, .3, .4, .5, .6, .7, .8, .9, 1],
						datasets: [
							{
								label: "Amplitude",
								type: "line",
								borderWidth: 1,
								pointRadius: 0,
								pointStyle: "circle",
								backgroundColor: "transparent",
								borderColor: "#a8a928",
								data: [],
								xAxisID: "spectre",
							}
						]
					}}

					options={{
						layout: {
							left: 0,
							right: 0,
							top: 0,
							bottom: 0
						},
						legend: {
							display: false
						},
						animation: {
							duration: 20
						},
						responsive: true,
						maintainAspectRatio: false,
						scales: {
							xAxes: [
								{
									gridLines: {
										color: "rgba(255,255,255, .2)",
										offsetGridLines: false,
									}
								},
								{
									id: "spectre",
									type: "linear",
									display: false,
									ticks: {
										min: 0,
										max: measurement.length,
										stepSize: 64,
									},

									gridLines: {
										display: false,
										drawBorder: false
									}
								}
							],
							yAxes: [
								{
									afterFit: function (scale) {
										scale.width = 60;
									},
									scaleLabel: {
										display: true,
										labelString: "dBm"
									},
									ticks: {
										padding: 5
									},
									gridLines: {
										zeroLineColor: "rgba(255,255,255, .2)",
										color: "rgba(255,255,255, .2)",
										display: true,
										drawBorder: false
									}
								}
							]
						}
					}}
				/>
			</div>
		);
	}
}

export default inject("common")(SpectreChart);