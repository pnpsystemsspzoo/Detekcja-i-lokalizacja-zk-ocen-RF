import React from "react";
import classNames from "classnames";

const TableHeader = ({ columns, store, store: { sort } }) => {
	return (<>
		{
			columns.length > 0 && <thead>
				<tr>
					{
						columns.map((column, index) => {
							const classes = classNames("th-inner", {
								sortable: column.sortable,
								asc: sort.order === "asc" && sort.column === index,
								desc: sort.order === "desc" && sort.column === index
							});

							return <th className="text-muted" key={index} colSpan={column.colspan}
								onClick={() => column.sortable && store.setSort(index, column.sort)}
							>
								<span className={classes}>
									{column.label}
									{column.select && <input type="checkbox" checked={column.isSelected()} onChange={column.onSelect} />}
								</span>
							</th>
						})
					}
				</tr>
			</thead>
		}
	</>
	);
};

TableHeader.defaultProps = {
	columns: [],
	sort: {
		order: undefined,
		by: undefined
	}
};

export default TableHeader;