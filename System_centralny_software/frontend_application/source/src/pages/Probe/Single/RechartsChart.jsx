import React from "react";
import { BarChart, Bar, XAxis, YAxis, CartesianGrid } from "recharts";
import { observer } from "mobx-react";

class RechartsChart extends React.Component {
    componentDidUpdate() {

        this.data = [
            { name: this.props.chartname, datakey: this.props.usage.current },
        ];
    }

    data = [
        { name: this.props.chartname, datakey: this.props.usage.current },
    ];

    render() {
        return (
            <div className="recharts-histogramstyle">
                <BarChart width={150} height={120} data={this.data}
                    margin={{ top: 5, right: 30, left: 20, bottom: 5 }}>
                    <CartesianGrid strokeDasharray="3 3" />
                    <XAxis dataKey="name" />
                    <YAxis domain={[0, 100]} />
                    <Bar isAnimationActive={false} dataKey="datakey" fill="#448BFF" />
                </BarChart>
            </div>
        )
    }
}

export default (observer(RechartsChart));