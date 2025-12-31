import React, {Component} from "react";
import {observer} from "mobx-react";
import Status from "./Status";

class Popup extends Component {
	
	render() {
		const {probe} = this.props;
		
		return (<>
				<Status probe={probe} className="d-inline-block" onlyHealth/> <strong>#{probe.id}</strong>
				<div className="text-muted">{probe.MAC}</div>
			</>
		);
		
	}
}

Popup.defaultProps = {
	large: false
};

export default observer(Popup);