import React, { Component } from "react";
import Loading from "../components/Loading";
import Hero from "./parts/Hero";
import { inject } from "mobx-react";
import classNames from "classnames";

class PageWrapper extends Component {

	componentDidMount() {
		const { common } = this.props;
		common.setTitle(this.props.title);
	}

	render() {
		const {
			// isLoading, 
			title, desc, children, className, showTitle, buttons } = this.props;
		const classPage = classNames("page-container", className);

		return (
			<div className={classPage}>
				<Loading
					// show={isLoading} 
					fixed bg />


				{showTitle && title && <Hero title={title} desc={desc}></Hero>}

				<div className="page-content">
					{buttons}
					{children}
				</div>
			</div>
		);
	}
}

PageWrapper.defaultProps = {
	showTitle: true,
	mapVisible: false,
	desc: ""
};

export default inject("common")(PageWrapper);