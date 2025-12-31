import React, { useState } from "react";
import { Line } from "react-chartjs-2";
import "chartjs-plugin-streaming";
import moment from "moment";

const Chart = require("react-chartjs-2").Chart;

const chartColors = {
    red: "rgb(255, 99, 132)",
    orange: "rgb(255, 159, 64)",
    yellow: "rgb(255, 205, 86)",
    green: "rgb(75, 192, 192)",
    blue: "rgb(54, 162, 235)",
    purple: "rgb(153, 102, 255)",
    grey: "rgb(201, 203, 207)"
};

const color = Chart.helpers.color;

function UsageChart(props) {

    const [data, setData] = useState({
        datasets: [
            {
                label: props.chartname,
                backgroundColor: color(chartColors[props.color])
                    .alpha(0.5)
                    .rgbString(),
                borderColor: chartColors[props.color],
                fill: false,
                lineTension: 0,
                data: [],
                pointRadius: 0
            }
        ]
    });

    const options = {
        elements: {
            line: {
                tension: 0.5
            }
        },
        scales: {
            xAxes: [
                {
                    type: "realtime",
                    distribution: "linear",
                    realtime: {
                        onRefresh: function (chart) {
                            chart.data.datasets[0].data.push({
                                x: moment(),
                                y: props.usage
                            });
                            if (chart.data.datasets[0].data.length > 12) {
                                chart.data.datasets[0].data.shift();
                            }
                        },
                        delay: 1000,
                    },
                    ticks: {
                        displayFormats: 10,
                        maxRotation: 0,
                        minRotation: 0,
                        stepSize: 1,
                        maxTicksLimit: 30,
                        minUnit: "minute",
                        // source: "auto",
                        // autoSkip: true,
                        callback: function (value) {
                            return moment(value, "HH:mm:ss").format("HH:mm:ss");
                        }
                    },
                    time: {
                        tooltipFormat: 'YYYY-MM-DD HH:mm',
                        displayFormats: {
                            millisecond: 'HH:mm:ss.SSS',
                            second: 'HH:mm:ss',
                            minute: 'HH:mm',
                            hour: 'HH'
                        }
                    },

                }
            ],
            yAxes: [
                {
                    ticks: {
                        beginAtZero: true,
                        max: 100,
                        stepSize: 20,
                    }
                }
            ]
        }
    };

    return (
        <div className="App">
            <Line data={data} options={options} width={200} />
        </div>
    );
}

export default UsageChart;