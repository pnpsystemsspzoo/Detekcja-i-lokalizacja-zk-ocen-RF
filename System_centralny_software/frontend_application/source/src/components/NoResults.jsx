import React from "react";
import {Card, CardBody} from "reactstrap";

const NoResults = ({label}) => {
	return (
		<Card>
			<CardBody>{label}</CardBody>
		</Card>
	);
};

NoResults.defaultProps = {
	label: "Brak wyników"
};

export default NoResults;