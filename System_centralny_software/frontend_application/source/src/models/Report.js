import { decorate, observable } from "mobx";
import Model from "../stores/base/Model";

/**
 * Probe object
 * @class
 */
class Report extends Model {
    /**
     * @type {number}
     */
    id;

    /**
     * @type {number}
     */
    authorId;

    /**
     * @type {String}
     */
    authorName;

    /**
     * @type {String}
     */
    authorRole;

    /**
     * @type {String}
     */
    creationDate;

    /**
         * @type {String}
         */
    creationTime;

    /**
        * @type {String}
        */
    filename;

    /**
     * `Create new instance of probe
     * @param input {Object}
     */
    constructor(input) {
        super();

        this.id = input.id;
        this.authorId = input.authorid;
        this.authorName = `${input.firstname} ${input.lastname}`;
        this.authorRole = input.role;
        this.creationDate = input.creationdate;
        this.creationTime = input.creationtime;
        this.filename = input.filename;
    }
}

decorate(Report, {
    id: observable,
    creationDate: observable,
    authorId: observable,
    authorRole: observable,
    authorName: observable,
    creationTime: observable,
    filename: observable,
});

export default Report;