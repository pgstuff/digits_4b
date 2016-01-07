/*
 * Author: The maintainer's name
 * Created at: Wed Oct 14 23:12:59 -0400 2015
 *
 */

SET client_min_messages = warning;

-- SQL definitions for USAEIN type
CREATE TYPE digits_4b;

-- basic i/o functions
CREATE OR REPLACE FUNCTION digits_4b_in(cstring) RETURNS digits_4b AS '$libdir/digits_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION digits_4b_out(digits_4b) RETURNS cstring AS '$libdir/digits_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION digits_4b_send(digits_4b) RETURNS bytea AS '$libdir/digits_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION digits_4b_recv(internal) RETURNS digits_4b AS '$libdir/digits_4b'
LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE digits_4b (
	input = digits_4b_in,
	output = digits_4b_out,
	send = digits_4b_send,
	receive = digits_4b_recv,
	internallength = 4,
	passedbyvalue
);

-- functions to support btree opclass
CREATE OR REPLACE FUNCTION digits_4b_lt(digits_4b, digits_4b) RETURNS bool AS '$libdir/digits_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION digits_4b_le(digits_4b, digits_4b) RETURNS bool AS '$libdir/digits_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION digits_4b_eq(digits_4b, digits_4b) RETURNS bool AS '$libdir/digits_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION digits_4b_ne(digits_4b, digits_4b) RETURNS bool AS '$libdir/digits_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION digits_4b_ge(digits_4b, digits_4b) RETURNS bool AS '$libdir/digits_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION digits_4b_gt(digits_4b, digits_4b) RETURNS bool AS '$libdir/digits_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION digits_4b_cmp(digits_4b, digits_4b) RETURNS int4 AS '$libdir/digits_4b'
LANGUAGE C IMMUTABLE STRICT;

-- to/from text conversion
CREATE OR REPLACE FUNCTION digits_4b_to_text(digits_4b) RETURNS text AS '$libdir/digits_4b'
LANGUAGE C IMMUTABLE STRICT;
CREATE OR REPLACE FUNCTION text_to_digits_4b(text) RETURNS digits_4b AS '$libdir/digits_4b'
LANGUAGE C IMMUTABLE STRICT;

-- operators
CREATE OPERATOR < (
	leftarg = digits_4b, rightarg = digits_4b, procedure = digits_4b_lt,
	commutator = >, negator = >=,
	restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR <= (
	leftarg = digits_4b, rightarg = digits_4b, procedure = digits_4b_le,
	commutator = >=, negator = >,
	restrict = scalarltsel, join = scalarltjoinsel
);
CREATE OPERATOR = (
	leftarg = digits_4b, rightarg = digits_4b, procedure = digits_4b_eq,
	commutator = =, negator = <>,
	restrict = eqsel, join = eqjoinsel,
	merges
);
CREATE OPERATOR <> (
	leftarg = digits_4b, rightarg = digits_4b, procedure = digits_4b_ne,
	commutator = <>, negator = =,
	restrict = neqsel, join = neqjoinsel
);
CREATE OPERATOR > (
	leftarg = digits_4b, rightarg = digits_4b, procedure = digits_4b_gt,
	commutator = <, negator = <=,
	restrict = scalargtsel, join = scalargtjoinsel
);
CREATE OPERATOR >= (
	leftarg = digits_4b, rightarg = digits_4b, procedure = digits_4b_ge,
	commutator = <=, negator = <,
	restrict = scalargtsel, join = scalargtjoinsel
);

-- aggregates
CREATE OR REPLACE FUNCTION digits_4b_smaller(digits_4b, digits_4b)
RETURNS digits_4b
AS '$libdir/digits_4b'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION digits_4b_larger(digits_4b, digits_4b)
RETURNS digits_4b
AS '$libdir/digits_4b'
    LANGUAGE C IMMUTABLE STRICT;

CREATE AGGREGATE min(digits_4b)  (
    SFUNC = digits_4b_smaller,
    STYPE = digits_4b,
    SORTOP = <
);

CREATE AGGREGATE max(digits_4b)  (
    SFUNC = digits_4b_larger,
    STYPE = digits_4b,
    SORTOP = >
);

-- btree operator class
CREATE OPERATOR CLASS digits_4b_ops DEFAULT FOR TYPE digits_4b USING btree AS
	OPERATOR 1 <,
	OPERATOR 2 <=,
	OPERATOR 3 =,
	OPERATOR 4 >=,
	OPERATOR 5 >,
	FUNCTION 1 digits_4b_cmp(digits_4b, digits_4b);

-- cast from/to text
CREATE CAST (digits_4b AS text) WITH FUNCTION digits_4b_to_text(digits_4b) AS ASSIGNMENT;
CREATE CAST (text AS digits_4b) WITH FUNCTION text_to_digits_4b(text) AS ASSIGNMENT;
