import React from "react";
import { inject, observer } from "mobx-react";
import { withRouter } from "react-router-dom";
import Table from '@material-ui/core/Table';
import TableBody from '@material-ui/core/TableBody';
import TableCell from '@material-ui/core/TableCell';
import TableContainer from '@material-ui/core/TableContainer';
import TableHead from '@material-ui/core/TableHead';
import TableRow from '@material-ui/core/TableRow';
import Paper from '@material-ui/core/Paper';
import { withTranslation } from "react-i18next";

const ExtendedReportTable = ({ tableData, reports, t }) => {
    return (
        <div className="table__data">
            {tableData && tableData.length === 0 && <>
                <p>{t('report_no_data')}</p>
            </>}
            {tableData && tableData.length > 0 && <>
                <TableContainer component={Paper}>
                    <Table id="my-table-1" sx={{ minWidth: 700 }} size="small">
                        <colgroup>
                            <col style={{ width: '10%' }} />
                            <col style={{ width: '15%' }} />
                            <col style={{ width: '10%' }} />
                        </colgroup>
                        <TableHead>
                            <TableRow>
                                {reports.options.map((option, index) => (
                                    <TableCell key={option.id} className={option.className} align="right">{t(option.column_name)}</TableCell>
                                ))}
                            </TableRow>
                        </TableHead>
                        <TableBody>
                            {tableData.map((interference) => (
                                <TableRow key={interference.id}>
                                    {reports.options.map((option) => (
                                        <TableCell key={option.id} align="right">{interference[option.column_name]}</TableCell>
                                    ))}
                                </TableRow>
                            ))}
                        </TableBody>
                    </Table>
                </TableContainer>
            </>}
        </div>


    );
}
export default inject("map", "reports")(withTranslation()(withRouter(observer(ExtendedReportTable))));