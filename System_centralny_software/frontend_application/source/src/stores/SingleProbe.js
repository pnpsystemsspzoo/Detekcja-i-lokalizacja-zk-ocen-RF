import { decorate, observable } from "mobx";

class SingleProbe {
	ifMounted = false; // test
}

decorate(SingleProbe, {
	ifMounted: observable,
});

export default new SingleProbe();