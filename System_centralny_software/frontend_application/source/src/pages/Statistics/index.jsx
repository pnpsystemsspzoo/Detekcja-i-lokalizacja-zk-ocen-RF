import React, {Component} from "react";
import {observer} from "mobx-react";

class Statistics extends Component {
	render() {
		return (
			<div>
			
			</div>
		);
	}
}

export default inject("common")(observer(Statistics));