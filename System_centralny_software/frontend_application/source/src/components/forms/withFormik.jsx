import React, {Component} from "react";
import has from "lodash.has";
import get from "lodash.get";
import {FormFeedback, FormGroup, InputGroup, InputGroupAddon, Label} from "reactstrap";


function withFormik(WrappedComponent) {
	class WithFormik extends Component {
		handleChange = event => {
			const {field, form} = this.props;
			if (event.target) {
				field.onChange(event);
			} else {
				form.setFieldValue(field.name, event);
			}
		};
		
		renderInput = () => {
			const {
				field,
				form,
				form: {touched, errors},
				label,
				isHorizontal,
				withoutWrapper,
				...restProps
			} = this.props;
			
			const {name} = field;
			const hasError = has(touched, name) && has(errors, name);
			
			return <WrappedComponent
				id={`field_${name}`}
				invalid={hasError}
				{...field}
				onChange={this.handleChange}
				{...restProps}
			/>
		};
		
		render() {
			const {
				field,
				form: {touched, errors},
				label,
				withoutWrapper,
				append,
				prepend,
			} = this.props;
			
			const {name} = field;
			const hasError = has(touched, name) && has(errors, name);
			let input = this.renderInput();
			
			if (append || prepend) {
				input = <InputGroup>
					{prepend && <InputGroupAddon addonType="prepend">{prepend}</InputGroupAddon>}
					{input}
					{append && <InputGroupAddon addonType="append">{append}</InputGroupAddon>}
					{hasError && <FormFeedback>{get(errors, name)}</FormFeedback>}
				</InputGroup>
			}
			
			if (!withoutWrapper) {
				input = <FormGroup>
					{label && <Label for={`field_${name}`}>{label}</Label>}
					{input}
					{!(append || prepend) && hasError && <FormFeedback>{get(errors, name)}</FormFeedback>}
				</FormGroup>
			}
			
			return input;
		}
	}
	
	WithFormik.defaultProps = {
		withoutWrapper: false,
		prepend: null,
		append: null
	};
	
	// WithFormik.displayName = `withFormik(${getDisplayName(WrappedComponent)})`;
	return WithFormik;
}

export default withFormik;
