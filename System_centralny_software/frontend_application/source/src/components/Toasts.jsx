import React, { Component } from "react";
import { inject, observer } from "mobx-react";
import { Toast } from "react-bootstrap";

class Toasts extends Component {
	render() {
		const { toasts } = this.props;

		return <div
			style={{
				position: "absolute",
				bottom: 10,
				right: 10,
				zIndex: 999999999999999,
			}}
		>
			{
				toasts.list.map((toast, index) =>
					<Toast key={index} onClose={() => toasts.hide(toast.id)}
						show={toast.show}
						delay={5000} autohide>
						<Toast.Header>
							<strong className="mr-auto">{toast.title}</strong>
						</Toast.Header>
						{toast.content && <Toast.Body>{toast.content}</Toast.Body>}
					</Toast>
				)
			}
		</div>
	}
}

export default inject("toasts")(observer(Toasts));