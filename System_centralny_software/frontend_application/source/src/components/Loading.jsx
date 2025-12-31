import React from "react";
import classNames from "classnames";

const Loading = ({ show, absolute, fixed, bg, text = null }) => {
	const classes = classNames("spinner", {
		"spinner--show": show,
		"spinner--absolute": absolute,
		"spinner--fixed": fixed,
		"spinner--bg": bg,
		"spinner--bg-transparent": bg === "transparent"
	});

	return (
		<div className={classes}>
			<figure>
				<div style={{ display: 'flex', alignItems: 'center', justifyContent: 'center', flexDirection: 'column' }}>
					<svg width="30px" height="30px" viewBox="0 0 66 66" xmlns="http://www.w3.org/2000/svg">
						<circle className="path" fill="none" strokeWidth="6" strokeLinecap="round" cx="33" cy="33" r="30" />
					</svg>
					<div>{text}</div>
				</div>

			</figure>

		</div>
	);
};

Loading.defaultProps = {
	show: false,
	absolute: false,
	fixed: false,
	bg: false
};

export default Loading;

