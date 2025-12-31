import React from "react";
import { inject, observer } from "mobx-react";
import { withRouter } from "react-router-dom";
import Table from '@material-ui/core/Table';
import TableBody from '@material-ui/core/TableBody';
import TableCell from '@material-ui/core/TableCell';
import TableContainer from '@material-ui/core/TableContainer';
import TableHead from '@material-ui/core/TableHead';
import TableRow from '@material-ui/core/TableRow';
// import TablePagination from '@material-ui/core/TablePagination';
import Paper from '@material-ui/core/Paper';
import { withTranslation } from "react-i18next";

const ReportTable = ({ generateExtendedReport, reports, t }) => {
    const { checkedOptionsNames, checkedOptionsColumns, tableData, selectReportRowToBeInPdfReport } = reports;

    return (

        <div className="table__data">
            {tableData && tableData.length === 0 && <>
                <p>{t('report_no_data')}</p>
            </>}
            {tableData && tableData.length > 0 && <>
                <TableContainer component={Paper}>
                    <Table sx={{ minWidth: 700 }} size="small">
                        <TableHead>
                            <TableRow>
                                <TableCell key="Edit" align="right">{t('generate_extended_report')}</TableCell>
                                <TableCell key="More" align="right">{t('more')}</TableCell>
                                {checkedOptionsNames.map((option) => (
                                    <TableCell className={option.className} key={option.id} align="right">{t(option.column_name)}</TableCell>
                                ))}
                            </TableRow>
                        </TableHead>
                        <TableBody>
                            {tableData.map((interference) => (
                                <TableRow key={interference._id}>
                                    <TableCell key={"report"} align="right">
                                        <input name={interference._id} id={interference._id} type="checkbox"
                                            checked={interference.generateReport} onChange={() => { selectReportRowToBeInPdfReport(interference.id) }} />
                                    </TableCell>
                                    <TableCell key={"extendedreport"} align="right">
                                        <button className="btn btn-xs btn-outline-secondary" onClick={() => generateExtendedReport(interference.id)}>{t('more')}</button>
                                    </TableCell>
                                    {checkedOptionsColumns.map((option) => (
                                        <TableCell key={option} align="right">{interference[option]}</TableCell>
                                    ))}
                                </TableRow>
                            ))}
                        </TableBody>
                    </Table>
                    {/* <TablePagination rowsPerPageOptions={[10, 50, { value: -1, label: 'All' }]} /> */}
                </TableContainer>
            </>}
        </div>


    );
}
export default inject("reports")(withTranslation()(withRouter(observer(ReportTable))));