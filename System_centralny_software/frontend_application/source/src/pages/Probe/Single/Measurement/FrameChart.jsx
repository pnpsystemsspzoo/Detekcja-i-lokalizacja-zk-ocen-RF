import React, { Component } from "react";
import { inject } from "mobx-react";
import { XAxis, YAxis, FlexibleXYPlot, AreaSeries } from 'react-vis';


class FrameChart extends Component {
    data;

    componentDidUpdate() {
        this.data = this.props.data.map((val) => {
            return { x: val[0], y: val[1] };
        });
    }
    constructor(props) {
        super(props);
        this.data = this.props.data.map((val) => {
            return { x: val[0], y: val[1] };
        });
    }

    render() {
        const { xDomainStart, xDomainEnd } = this.props;
        return (
            <div style={{ height: "50%" }}>
                <div style={{ height: "100%" }}>
                    <FlexibleXYPlot yDomain={[-100, 5]} xDomain={[xDomainStart, xDomainEnd]} >
                        <XAxis tickValues={[xDomainStart, xDomainEnd]} />
                        <YAxis tickValues={[-100, -80, -60, -20, 5]} />
                        <AreaSeries
                            className="area-series-example"
                            curve="curveMonotoneX"
                            data={this.data}
                            fill='#448BFF'
                            stroke='#448BFF'
                        />
                    </FlexibleXYPlot>
                </div>
            </div>


        );
    }
}

export default inject("common")(FrameChart);