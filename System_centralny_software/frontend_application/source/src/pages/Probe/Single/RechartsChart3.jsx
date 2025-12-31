import React from "react";
import RealtimeLineChart from "./RealtimeLineChart";
import { inject, observer } from "mobx-react";
import { decorate, observable } from "mobx";
import moment from 'moment';

const TIME_RANGE_IN_MILLISECONDS = 5 * 1000;
const ADDING_DATA_INTERVAL_IN_MILLISECONDS = 1000;
const ADDING_DATA_RATIO = 0.8;

const RechartsChart3 = (props) => {
    const nameList = ["hdd"];

    const defaultDataList = nameList.map(name => ({
        name: name,
        data: []
    }));

    const [dataList, setDataList] = React.useState(defaultDataList);

    React.useEffect(() => {
        const addData = data => {
            let tab = [];
            tab = [
                ...data,
                {
                    x: new Date(),
                    y: props.usage === undefined ? 0 : props.usage
                }
            ];
            if (data.length > 10) {
                data = data.slice(1);
            }
            return tab;
        };

        setDataList(
            dataList.map(val => {
                return {
                    name: val.name,
                    data: addData(val.data)
                };
            })
        );
    }, [props.usage]);

    return (
        <div>
            <RealtimeLineChart
                dataList={dataList}
                range={TIME_RANGE_IN_MILLISECONDS}
                chartname={props.chartname}
            />
        </div>
    );
};

export default inject("common")(observer(RechartsChart3));
