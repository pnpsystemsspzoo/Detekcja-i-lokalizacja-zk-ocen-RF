import axios from "axios";
import config from "../config/main";


class Request {
	call = (method = "get", endpoint, data = null, responseType, params = {}) => {
		return new Promise((resolve, reject) => {
			if (method === "get") {
				axios
					.get(`${config.URL}${endpoint}`, {
						headers: { Authorization: `Bearer ${localStorage.getItem("token")}` },
						responseType,
						params: data
					})
					.then(res => {
						resolve(res)
					})
					.catch(err => {
						console.log('err', err)
					})
			}
			else if (method === "post") {
				axios
					.post(`${config.URL}${endpoint}`, data, {
						headers: { Authorization: `Bearer ${localStorage.getItem("token")}` },
						responseType,
						params
					})
					.then(res => {
						resolve(res)
					})
					.catch(err => {
						console.log('err', err.status)
					})
			}
		});
	};
}

export default new Request();