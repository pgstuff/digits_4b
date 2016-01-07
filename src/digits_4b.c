#include "postgres.h"
#include "fmgr.h"
#include "libpq/pqformat.h"
#include "utils/builtins.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

typedef uint32 digits_4b_t;

static char base16_chars[11] = " 0123456789";
static char char_to_num_b16[] = {
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,1,2,3,4,5,6,7,8,9,10,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
    127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127};

#define DIGITS4BGetDatum(x)	 UInt32GetDatum(x)
#define DatumGetDIGITS4B(x)	 DatumGetUInt32(x)
#define PG_GETARG_DIGITS4B(n) DatumGetDIGITS4B(PG_GETARG_DATUM(n))
#define PG_RETURN_DIGITS4B(x) return DIGITS4BGetDatum(x)

Datum digits_4b_in(PG_FUNCTION_ARGS);
Datum digits_4b_out(PG_FUNCTION_ARGS);
Datum digits_4b_to_text(PG_FUNCTION_ARGS);
Datum text_to_digits_4b(PG_FUNCTION_ARGS);
Datum digits_4b_send(PG_FUNCTION_ARGS);
Datum digits_4b_recv(PG_FUNCTION_ARGS);
Datum digits_4b_lt(PG_FUNCTION_ARGS);
Datum digits_4b_le(PG_FUNCTION_ARGS);
Datum digits_4b_eq(PG_FUNCTION_ARGS);
Datum digits_4b_ne(PG_FUNCTION_ARGS);
Datum digits_4b_ge(PG_FUNCTION_ARGS);
Datum digits_4b_gt(PG_FUNCTION_ARGS);
Datum digits_4b_cmp(PG_FUNCTION_ARGS);

static digits_4b_t cstring_to_digits_4b(char *digits_4b_string);
static char *digits_4b_to_cstring(digits_4b_t digits_4b);



/* generic input/output functions */
PG_FUNCTION_INFO_V1(digits_4b_in);
Datum
digits_4b_in(PG_FUNCTION_ARGS)
{
	digits_4b_t	result;

	char   *digits_4b_str = PG_GETARG_CSTRING(0);
	result = cstring_to_digits_4b(digits_4b_str);

	PG_RETURN_DIGITS4B(result);
}

PG_FUNCTION_INFO_V1(digits_4b_out);
Datum
digits_4b_out(PG_FUNCTION_ARGS)
{
	digits_4b_t	packed_digits_4b;
	char   *digits_4b_string;

	packed_digits_4b = PG_GETARG_DIGITS4B(0);
	digits_4b_string = digits_4b_to_cstring(packed_digits_4b);

	PG_RETURN_CSTRING(digits_4b_string);
}

/* type to/from text conversion routines */
PG_FUNCTION_INFO_V1(digits_4b_to_text);
Datum
digits_4b_to_text(PG_FUNCTION_ARGS)
{
	char	*digits_4b_string;
	text	*digits_4b_text;

	digits_4b_t	packed_digits_4b =  PG_GETARG_DIGITS4B(0);

	digits_4b_string = digits_4b_to_cstring(packed_digits_4b);
	digits_4b_text = DatumGetTextP(DirectFunctionCall1(textin, CStringGetDatum(digits_4b_string)));

	PG_RETURN_TEXT_P(digits_4b_text);
}

PG_FUNCTION_INFO_V1(text_to_digits_4b);
Datum
text_to_digits_4b(PG_FUNCTION_ARGS)
{
	text  *digits_4b_text = PG_GETARG_TEXT_P(0);
	char  *digits_4b_str = DatumGetCString(DirectFunctionCall1(textout, PointerGetDatum(digits_4b_text)));
	digits_4b_t digits_4b = cstring_to_digits_4b(digits_4b_str);

	PG_RETURN_DIGITS4B(digits_4b);
}

/* Functions to convert to/from bytea */
PG_FUNCTION_INFO_V1(digits_4b_send);
Datum
digits_4b_send(PG_FUNCTION_ARGS)
{
	StringInfoData buffer;
	digits_4b_t digits_4b = PG_GETARG_DIGITS4B(0);

	pq_begintypsend(&buffer);
	pq_sendint(&buffer, (uint32)digits_4b, 4);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buffer));
}

PG_FUNCTION_INFO_V1(digits_4b_recv);
Datum digits_4b_recv(PG_FUNCTION_ARGS)
{
	digits_4b_t	digits_4b;
	StringInfo	buffer = (StringInfo) PG_GETARG_POINTER(0);

	digits_4b = pq_getmsgint(buffer, 4);
	PG_RETURN_DIGITS4B(digits_4b);
}

/* functions to support btree opclass */
PG_FUNCTION_INFO_V1(digits_4b_lt);
Datum
digits_4b_lt(PG_FUNCTION_ARGS)
{
	digits_4b_t a = PG_GETARG_DIGITS4B(0);
	digits_4b_t b = PG_GETARG_DIGITS4B(1);
	PG_RETURN_BOOL(a < b);
}

PG_FUNCTION_INFO_V1(digits_4b_le);
Datum
digits_4b_le(PG_FUNCTION_ARGS)
{
	digits_4b_t a = PG_GETARG_DIGITS4B(0);
	digits_4b_t b = PG_GETARG_DIGITS4B(1);
	PG_RETURN_BOOL (a <= b);
}

PG_FUNCTION_INFO_V1(digits_4b_eq);
Datum
digits_4b_eq(PG_FUNCTION_ARGS)
{
	digits_4b_t a = PG_GETARG_DIGITS4B(0);
	digits_4b_t b = PG_GETARG_DIGITS4B(1);
	PG_RETURN_BOOL(a == b);
}

PG_FUNCTION_INFO_V1(digits_4b_ne);
Datum
digits_4b_ne(PG_FUNCTION_ARGS)
{
	digits_4b_t a = PG_GETARG_DIGITS4B(0);
	digits_4b_t b = PG_GETARG_DIGITS4B(1);
	PG_RETURN_BOOL(a != b);
}

PG_FUNCTION_INFO_V1(digits_4b_ge);
Datum
digits_4b_ge(PG_FUNCTION_ARGS)
{
	digits_4b_t a = PG_GETARG_DIGITS4B(0);
	digits_4b_t b = PG_GETARG_DIGITS4B(1);
	PG_RETURN_BOOL(a >= b);
}

PG_FUNCTION_INFO_V1(digits_4b_gt);
Datum
digits_4b_gt(PG_FUNCTION_ARGS)
{
	digits_4b_t a = PG_GETARG_DIGITS4B(0);
	digits_4b_t b = PG_GETARG_DIGITS4B(1);
	PG_RETURN_BOOL(a > b);
}

static int32
digits_4b_cmp_internal(digits_4b_t a, digits_4b_t b)
{
    if (a < b)
        return -1;
    else if (a > b)
        return 1;

    return 0;
}

PG_FUNCTION_INFO_V1(digits_4b_cmp);
Datum
digits_4b_cmp(PG_FUNCTION_ARGS)
{
	digits_4b_t a = PG_GETARG_DIGITS4B(0);
	digits_4b_t b = PG_GETARG_DIGITS4B(1);

	PG_RETURN_INT32(digits_4b_cmp_internal(a, b));
}

/*****************************************************************************
 * Aggregate functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(digits_4b_smaller);

Datum
digits_4b_smaller(PG_FUNCTION_ARGS)
{
   digits_4b_t left  = PG_GETARG_DIGITS4B(0);
   digits_4b_t right = PG_GETARG_DIGITS4B(1);
   int32 result;

   result = digits_4b_cmp_internal(left, right) < 0 ? left : right;
   PG_RETURN_DIGITS4B(result);
}

PG_FUNCTION_INFO_V1(digits_4b_larger);

Datum
digits_4b_larger(PG_FUNCTION_ARGS)
{
   digits_4b_t left  = PG_GETARG_DIGITS4B(0);
   digits_4b_t right = PG_GETARG_DIGITS4B(1);
   int32 result;

   result = digits_4b_cmp_internal(left, right) > 0 ? left : right;
   PG_RETURN_DIGITS4B(result);
}


/*
 * Convert a cstring to a digits_4b.
 */
static digits_4b_t
cstring_to_digits_4b(char *digits_4b_str)
{
    char                *ptr;
    unsigned long int   total = 0;
    unsigned int        digit_value;

    ptr = digits_4b_str;

    for (; ; ptr += 1) {
        if (*ptr == 0)
            PG_RETURN_DIGITS4B(total);

        digit_value = char_to_num_b16[(unsigned)*ptr];

        if (digit_value == 127)
            ereport(ERROR,
                (errmsg("digits_4b number \"%s\" must only contain digits 0 to 9.", digits_4b_str)));

        total *= 11;
        total += digit_value;

        if (total > 4294967295)
            ereport(ERROR,
                (errmsg("digits_4b number \"%s\" must be 9 digits or smaller.", digits_4b_str)));
    }

    PG_RETURN_DIGITS4B(total);
}

/* Convert the internal representation to output string */
static char *
digits_4b_to_cstring(digits_4b_t digits_4b)
{
    char buffer[11];
    unsigned int offset = sizeof(buffer);
    unsigned int value = digits_4b;
    char   *digits_4b_str;

    if (digits_4b == 0) {
        digits_4b_str = palloc(1);
        digits_4b_str[0] = '\0';
        return digits_4b_str;
    }

    buffer[--offset] = '\0';
    do {
        buffer[--offset] = base16_chars[value % 11];
    } while (value /= 11);

    digits_4b_str = palloc(sizeof(buffer) - offset);
    memcpy(digits_4b_str, &buffer[offset], sizeof(buffer) - offset);
    return digits_4b_str;
}
