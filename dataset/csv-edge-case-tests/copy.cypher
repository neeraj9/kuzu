COPY `bom-and-data` FROM "dataset/csv-edge-case-tests/bom-and-data.csv";
COPY `bom-and-header` FROM "dataset/csv-edge-case-tests/bom-and-header.csv" (HEADER=TRUE);
COPY `bom` FROM "dataset/csv-edge-case-tests/bom.csv";
COPY `carriage-return-then-eof` FROM "dataset/csv-edge-case-tests/carriage-return-then-eof.csv";
COPY `delimiter-then-eof` FROM "dataset/csv-edge-case-tests/delimiter-then-eof.csv";
COPY `empty-first-line` FROM "dataset/csv-edge-case-tests/empty-first-line.csv";
COPY `empty-lines-multiple-columns` FROM "dataset/csv-edge-case-tests/empty-lines-multiple-columns.csv";
COPY `empty-lines-single-column` FROM "dataset/csv-edge-case-tests/empty-lines-single-column.csv";
COPY `empty` FROM "dataset/csv-edge-case-tests/empty.csv";
COPY `empty-with-header` FROM "dataset/csv-edge-case-tests/empty.csv" (HEADER=TRUE);
COPY `eof-after-unquote` FROM "dataset/csv-edge-case-tests/eof-after-unquote.csv";
COPY `escapes-in-quote` FROM "dataset/csv-edge-case-tests/escapes-in-quote.csv";
COPY `escapes-out-of-quote` FROM "dataset/csv-edge-case-tests/escapes-out-of-quote.csv";
COPY `mixed-empty-lines-multiple-columns` FROM "dataset/csv-edge-case-tests/mixed-empty-lines-multiple-columns.csv";
COPY `mixed-empty-lines-single-column` FROM "dataset/csv-edge-case-tests/mixed-empty-lines-single-column.csv";
COPY `mixed-newlines` FROM "dataset/csv-edge-case-tests/mixed-newlines.csv";
COPY `quoted-values` FROM "dataset/csv-edge-case-tests/quoted-values.csv";
COPY `tab-as-delim` FROM "dataset/csv-edge-case-tests/tab-as-delim.csv" (DELIM='\\t');
COPY `unquote-escape` FROM "dataset/csv-edge-case-tests/unquote-escape.csv" (ESCAPE='"');