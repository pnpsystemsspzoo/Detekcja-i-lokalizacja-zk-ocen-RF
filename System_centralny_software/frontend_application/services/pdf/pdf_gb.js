const { jsPDF } = require("jspdf");
const imagecode = require('./imagecode');
const autoTable = require("jspdf-autotable");
const Roboto = require('./Roboto-normal.js');
var moment = require('moment');
var fs = require("fs");

const generatePdf_gb = async (tableData, options, details, userId, userName, userType, reportName) => {
    tableData = JSON.parse(tableData);
    options = JSON.parse(options);

    // set pdf options, fonts, colors
    const doc = new jsPDF({
        orientation: "landscape",
        unit: "mm",
    })
    doc.setFont('Roboto');
    let headStyles = { fillColor: ["#ffe70a"], fontSize: 6, font: 'Roboto', textColor: "#000000" };
    let headStyles2 = { fillColor: ["#d9d9d9"], fontSize: 6, font: 'Roboto', textColor: "#000000" };
    let styles = { font: 'Roboto', fontSize: 6 };

    // add logo
    var imgData = imagecode;
    doc.addImage(imgData, 'jpeg', 12, 15, 94, 23); // left, top, width, height

    // add title
    doc.setFontSize(20);
    doc.text(`Report containing interferences from central system`, 14, 80);

    // add subtitle
    doc.setFontSize(12);
    details.isExtended == 'false' && doc.text(`Basic report from date range: ${moment(details.startDate).format('DD-MM-YYYY')} - ${moment(details.endDate).format('DD-MM-YYYY')}`, 14, 90);
    details.isExtended == 'true' && doc.text(`Extended report from interference with ID: ${tableData[0].id}`, 14, 90);

    // add timestamp and user
    doc.setFontSize(8);
    doc.text(`Report generated ${moment().format('DD-MM-YYYY HH:mm:ss')} by user with ID ${userId} (${userName}). Account type: ${userType}`, 14, 190);

    doc.addPage();

    // add table
    tableData.forEach((table, index) => {
        let headSt;

        if (index % 2 === 0) headSt = headStyles;
        else headSt = headStyles2;


        let headerData = [], data = [];

        options.forEach((option) => {
            headerData.push(option.name);
            let value = table[option.column_name];
            data.push(value);
        })
        doc.setFontSize(8);

        // all except first table on the page
        if ((index) % 3 !== 0) {
            doc.text(`Extended data about interference with ID ${data[0]}`, 14, doc.autoTableEndPosY() + 12);
            doc.autoTable({
                styles: styles,
                headStyles: headSt,
                head: [
                    [headerData[0], headerData[1], headerData[2], headerData[3], headerData[4], headerData[5], headerData[6], headerData[7], headerData[8], headerData[9], headerData[10], headerData[11]]
                ],
                body: [
                    [data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11]],
                ],
                startY: doc.autoTableEndPosY() + 16
            })

        }
        // first table on the page
        else {
            doc.text(`Extended data about interference with ID ${data[0]}`, 14, 10);
            doc.autoTable({
                styles: styles,
                headStyles: headSt,
                head: [
                    [headerData[0], headerData[1], headerData[2], headerData[3], headerData[4], headerData[5], headerData[6], headerData[7], headerData[8], headerData[9], headerData[10], headerData[11]]
                ],
                body: [
                    [data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11]],
                ],
            })
        }


        // if (headerData.length > 13) {
        doc.autoTable({
            styles: styles,
            headStyles: headSt,
            head: [
                [headerData[12], headerData[13], headerData[14], headerData[15], headerData[16], headerData[17], headerData[18], headerData[19], headerData[20], headerData[21], headerData[22], headerData[23], headerData[24], headerData[25], headerData[26]]
            ],
            body: [
                [data[12], data[13], data[14], data[15], data[16], data[17], data[18], data[19], data[20], data[21], data[22], data[23], data[24], data[25], data[26]]
            ]
        })
        // }

        if ((index + 1) % 3 === 0) doc.addPage();
    })

    if (tableData.length % 3 !== 0) doc.addPage();

    if (userType !== "System") {
        // add interference image
        let path_url, base64Img;

        if (details.isExtended == 'false') path_url = './basic_report_screenshot.jpeg';
        else if (details.isExtended == 'true') path_url = './extended_report_screenshot.jpeg';

        base64Img = fs.readFileSync(path_url).toString('base64');
        doc.text(`Visualization of events on the map from ${moment(details.startDate)
            .format("DD-MM-YYYY")} to ${moment(details.endDate).format("DD-MM-YYYY")}:`, 14, 20);
        doc.text(`(The pins symbolize the end points of the interference)`, 14, 25);
        doc.addImage(base64Img, 'JPEG', 48, 36, 200, 160); // leftMargin, topMargin, width, height
    }

    doc.save(reportName);
}

module.exports = generatePdf_gb;