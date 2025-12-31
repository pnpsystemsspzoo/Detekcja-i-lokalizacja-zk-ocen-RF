import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import InlineSVG from "react-inlinesvg";
import classNames from "classnames";
import { degreesToMinutes, timeConversion } from "../utils/Helpers";
import { OverlayTrigger, Popover } from "react-bootstrap";
import { withTranslation } from "react-i18next";

class Interference extends Component {

	renderPopover = () => {
		const { interference, t } = this.props;

		return <Popover>
			<Popover.Content>
				<ul className="interference-details">
					<li>
						<strong className="text-primary">
							{degreesToMinutes(interference.latitude, interference.longitude)}
						</strong>
					</li>
					<li>
						<span className="text-muted">{t('height')} </span>
						<strong className="text-primary">{interference.height}m</strong>
					</li>
					<li>
						<span className="text-muted">{t('direction')} </span>
						<figure className="d-inline-block m-0" style={{ transform: `rotate(${interference.heading}deg)` }}>
							<InlineSVG src="/svg/arrow-up.svg" />
						</figure>
						<strong className="text-primary">{interference.heading}&deg;</strong>
					</li>
					<li>
						<span className="text-muted">{t('speed')} </span>
						<strong className="text-primary">{interference.speed}km/h</strong>
					</li>
					<li>
						<span className="text-muted">{t('range')} </span>
						<strong className="text-primary">{interference.range}m</strong>
					</li>
				</ul>
			</Popover.Content>
		</Popover>
	};

	render() {
		const { map, interference } = this.props;

		const classes = classNames("interference", {
			"interference--extinguished": interference.extinguished
		});

		return (
			<OverlayTrigger trigger={["hover", "focus"]} placement="right" overlay={this.renderPopover()} rootClose={true}>
				<div className={classes}
					id={`interference-${interference.id}`}
					// onMouseEnter={interference.onMouseEnter}
					// onMouseLeave={interference.onMouseLeave}
					onClick={() => map.centerInterference(interference)}
				>
					<div>
						{!interference.extinguished && <span className="status" style={{ background: interference.color }} />}
						{
							interference.extinguished && <button className="btn btn-link p-0" onClick={interference.delete}>
								<InlineSVG src="/svg/trash.svg" />
							</button>
						}
						<span>&nbsp;<strong>#{interference.id}</strong></span>
					</div>
					<div>
						<span className="badge badge-light">
							{timeConversion(interference.time)}
						</span>
						<span className="badge badge-primary">{interference.system}</span>
					</div>
				</div>

			</OverlayTrigger>
		);
	}
}

export default inject('map')(withTranslation()(observer(Interference)));