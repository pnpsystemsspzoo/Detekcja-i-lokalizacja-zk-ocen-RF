import { observer } from "mobx-react";
import useBeforeUnload from 'use-before-unload';
// import Auth from "./Auth";
import { inject } from "mobx-react";

const Beforeunload = (props) => {

    useBeforeUnload(evt => {

    });

    return props.children; // render Alerts anyway
};

export default inject("auth")(observer(Beforeunload));