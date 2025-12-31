import React, { Component } from "react";
import { Line, Bar } from "react-chartjs-2";
import "chartjs-plugin-streaming";
import { observer } from "mobx-react";
import chroma from "chroma-js";

class UsageChartOld extends Component {
	data;

	componentWillMount() {
		const { color } = this.props;

		this.data = canvas => {
			const ctx = canvas.getContext("2d");
			const gradient = ctx.createLinearGradient(0, 0, 0, 80);
			gradient.addColorStop(0, chroma(color).alpha(.4));
			gradient.addColorStop(1, chroma(color).alpha(0));

			return {
				datasets: [
					{
						label: 'Rainfall',
						borderColor: color,
						backgroundColor: gradient,
						borderWidth: 1,
						lineTension: 0,
						data: [],
						// borderDash: [8, 4]
						pointRadius: 0
					}
				]

			}
		}
	}

	render() {
		const { name, unit, usage } = this.props;

		return (
			<div className="usage">

				<div className="usage__name">
					<p>{name}</p>
					<span className="text-muted">{usage.current} {unit}</span>
				</div>

				<div className="usage__chart">
					<Bar ref={usage.chart}
						data={this.data}
						width={150}
						height={50}
						options={{
							legend: {
								display: false
							},
							animation: {
								duration: 0
							},
							layout: {
								padding: 0
							},
							scales: {
								yAxes: [
									{
										ticks: {
											display: false,
											min: 0,
											max: 100
										},
										gridLines: {
											display: true,
											drawBorder: false,
											tickMarkLength: 0,

										}
									}
								],
								xAxes: [
									{
										ticks: {
											display: false,
										},
										gridLines: {
											display: false,
											drawBorder: false,
											tickMarkLength: 0,

										},
										type: "realtime",
										realtime: {
											duration: 10000,
										},
										delay: 4000,
									}
								]

							},
							plugins: {
								streaming: {
									frameRate: 60
								}
							}
						}} />
				</div>
			</div>
		);
	}
}


export default observer(UsageChartOld);