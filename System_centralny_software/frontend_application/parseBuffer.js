// var Parser = require("binary-parser-encoder").Parser;
const Parser = require("./Parser");


const parseBuffer = (buffer, logger) => {
    const check = Parser.parsers.BASE.parse(buffer);
    const name = Parser.getName(check.frame_ID);

    if (name) {
        const parser = Parser.parsers[name];

        try {
            const result = parser.parse(buffer);
            result.system_date = new Date();
            result.trace_type = "success";
            logger.trace(result);

            if (buffer.length > result.parserOffset) {
                const _buffer = new Buffer(buffer.length - result.parserOffset);
                buffer.copy(_buffer, 0, result.parserOffset);
                parseBuffer(_buffer, logger);
            }
        } catch (e) {
            console.error('ERROR - ', e.message);
            e.system_date = new Date();
            e.trace_type = "ERROR!";
            logger.trace(e.message);
        }
    }
    else {
        check.system_date = new Date();
        check.trace_type = "ELSE!";
        logger.trace(check);
    }
}

module.exports = parseBuffer;
