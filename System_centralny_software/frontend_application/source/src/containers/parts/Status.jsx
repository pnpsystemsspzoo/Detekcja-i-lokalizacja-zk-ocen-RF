import React, { Component } from "react";
import classNames from "classnames";
import { observer } from "mobx-react";
import { UncontrolledTooltip } from "reactstrap";

class Status extends Component {

	render() {
		const { probe, large, className, status, statusDesc } = this.props;

		let probeClassname;
		if (status === "communication") {
			probeClassname = classNames("status mr-2", {
				"status--large": large,
				"status--danger": probe[status] === 0,
				// "status--warning": probe[status] === 1,
				"status--active": probe[status] === 1,
			});
		}
		else {
			probeClassname = classNames("status mr-2", {
				"status--large": large,
				"status--danger": probe[status] === 0,
				"status--warning": probe[status] === 1,
				"status--active": probe[status] === 2,
			});
		}

		return (<div className={className}>
			<figure className={probeClassname} id={`tooltip-${status}-${probe.id}`} />
			<UncontrolledTooltip placement="top" target={`tooltip-${status}-${probe.id}`}>
				{statusDesc}
			</UncontrolledTooltip>
		</div>
		);

	}
}

Status.defaultProps = {
	large: false
};

export default observer(Status);