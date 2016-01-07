\set ECHO none
\i sql/digits_4b.sql
\set ECHO all

CREATE TABLE digits_4bs(id serial primary key, digits_4b digits_4b unique);
INSERT INTO digits_4bs(digits_4b) VALUES('123456789');
INSERT INTO digits_4bs(digits_4b) VALUES('');
INSERT INTO digits_4bs(digits_4b) VALUES('0');
INSERT INTO digits_4bs(digits_4b) VALUES('10');
INSERT INTO digits_4bs(digits_4b) VALUES('100');
INSERT INTO digits_4bs(digits_4b) VALUES('000');
INSERT INTO digits_4bs(digits_4b) VALUES('010');
INSERT INTO digits_4bs(digits_4b) VALUES('000000000'); -- test limits
INSERT INTO digits_4bs(digits_4b) VALUES('999999999');

SELECT * FROM digits_4bs ORDER BY digits_4b;

SELECT MIN(digits_4b) AS min FROM digits_4bs;
SELECT MAX(digits_4b) AS max FROM digits_4bs;

-- index scan
TRUNCATE digits_4bs;
INSERT INTO digits_4bs(digits_4b) SELECT '4'||id FROM generate_series(5678, 8000) id;
SELECT id,digits_4b::text FROM digits_4bs WHERE digits_4b = '48000';

SET enable_seqscan = false;
SELECT id,digits_4b::text FROM digits_4bs WHERE digits_4b = '46000';
SELECT id,digits_4b FROM digits_4bs WHERE digits_4b >= '47000' LIMIT 5;
SELECT count(id) FROM digits_4bs;
SELECT count(id) FROM digits_4bs WHERE digits_4b <> ('46500'::text)::digits_4b;
RESET enable_seqscan;

-- operators and conversions
SELECT '0'::digits_4b < '0'::digits_4b;
SELECT '0'::digits_4b > '0'::digits_4b;
SELECT '0'::digits_4b < '1'::digits_4b;
SELECT '0'::digits_4b > '1'::digits_4b;
SELECT '0'::digits_4b <= '0'::digits_4b;
SELECT '0'::digits_4b >= '0'::digits_4b;
SELECT '0'::digits_4b <= '1'::digits_4b;
SELECT '0'::digits_4b >= '1'::digits_4b;
SELECT '0'::digits_4b <> '0'::digits_4b;
SELECT '0'::digits_4b <> '1'::digits_4b;
SELECT '0'::digits_4b = '0'::digits_4b;
SELECT '0'::digits_4b = '1'::digits_4b;

-- COPY FROM/TO
TRUNCATE digits_4bs;
COPY digits_4bs(digits_4b) FROM STDIN;

999999999
\.
COPY digits_4bs TO STDOUT;

-- clean up --
DROP TABLE digits_4bs;

-- errors
SELECT ''::digits_4b;
SELECT '1000000000'::digits_4b;
SELECT '!'::digits_4b;
