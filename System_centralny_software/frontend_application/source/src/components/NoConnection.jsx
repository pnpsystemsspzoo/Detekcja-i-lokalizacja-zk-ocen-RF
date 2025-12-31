import React, { Component } from "react";
import { observer } from "mobx-react";
import Loading from "./Loading";

class NoConnection extends Component {
	render() {
		return (
			<Loading bg fixed show={true} />
		);
	}
}

export default (observer(NoConnection));