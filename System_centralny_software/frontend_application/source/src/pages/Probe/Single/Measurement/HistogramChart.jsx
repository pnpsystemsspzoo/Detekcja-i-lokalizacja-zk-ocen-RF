import React, { Component } from "react";

class HistogramChart extends Component {
	render() {
		const { measurement } = this.props;

		return (
			<div className="chart chart--histogram">
				<div className="chart__inner">
					<div className="chart__axis chart__axis-y">
						<ul className="chart__axis__scale">
							<li>0</li>
							<li>2</li>
							<li>4</li>
							<li>6</li>
							<li>8</li>
							<li>10</li>
						</ul>
					</div>
					<div className="chart__content">
						<canvas width={measurement.length} height={40} ref={measurement.histogram} />
					</div>
				</div>
			</div>
		);
	}
}

export default HistogramChart;