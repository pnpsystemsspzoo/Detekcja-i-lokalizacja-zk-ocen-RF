import React from "react";
import Chart from "react-apexcharts";

export default props => {
    const options = {
        chart: {
            zoom: {
                enabled: false
            },
            animations: {
                easing: "linear",
                dynamicAnimation: {
                    speed: 500
                }
            }
        },
        stroke: {
            width: 1
        },
        tooltip: {
            x: {
                format: "yyyy/MM/dd HH:mm:ss.f"
            }
        },
        xaxis: {
            type: "datetime",
            range: props.range
        },

        yaxis: {
            min: 0,
            max: 100,
            title: { text: props.chartname }
        }
    };
    return <Chart type="line" options={options} series={props.dataList} />;
};