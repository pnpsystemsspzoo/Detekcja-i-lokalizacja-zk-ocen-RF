import React, { Component } from "react";
import Logo from '../../assets/logo_pnpsystems.png';

class Hero extends Component {
	render() {
		const { title, desc } = this.props;

		return (
			<div className="page-hero" id="page-hero">
				<div className="padding d-flex justify-content-between align-items-center">
					<div style={{ display: 'flex', justifyContent: 'space-between', width: '100%' }}>
						<div>
							<h2 className="text-md text-highlight">{title}</h2>
							{desc && <small className="text-muted">{desc}</small>}
						</div>
						<div>
							<img src={Logo} width="160" height="auto" alt="logo" />
						</div>
					</div>
					{this.props.children && <div>{this.props.children}</div>}
				</div>
			</div>
		);
	}
}

Hero.defaultProps = {
	title: "",
	desc: null
};

export default Hero;