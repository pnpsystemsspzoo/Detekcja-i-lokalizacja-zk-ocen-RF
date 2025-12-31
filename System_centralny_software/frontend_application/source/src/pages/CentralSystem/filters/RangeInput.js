import React from "react";

const RangeInput = ({ placeholder, value, handleOnChange }) => {
    return (
        <div className="calendar_component__container">
            <div>
                <input type="number" className="range_input__input" placeholder={placeholder} value={value} onChange={handleOnChange} />
            </div>
        </div>
    );
};

export default RangeInput;