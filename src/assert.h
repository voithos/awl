#ifndef AWL_ASSERT_H
#define AWL_ASSERT_H

#define LASSERT(args, cond, fmt, ...) \
    if (!(cond)) { \
        awlval* err = awlval_err(fmt, __VA_ARGS__); \
        awlval_del(args); \
        return err; \
    }

#define LASSERT_TYPE(args, i, expected, fname) \
    LASSERT(args, (args->cell[i]->type == expected), \
            "function '%s' passed incorrect type for arg %i; got %s, expected %s", \
            fname, i, awlval_type_name(args->cell[i]->type), awlval_type_name(expected));

#define LASSERT_ISNUMERIC(args, i, fname) \
    LASSERT(args, (ISNUMERIC(args->cell[i]->type)), \
            "function '%s' passed incorrect type for arg %i; got %s, expected numeric type", \
            fname, i, awlval_type_name(args->cell[i]->type));

#define LASSERT_ISCOLLECTION(args, i, fname) \
    LASSERT(args, (ISCOLLECTION(args->cell[i]->type)), \
            "function '%s' passed incorrect type for arg %i; got %s, expected collection type", \
            fname, i, awlval_type_name(args->cell[i]->type));

#define LASSERT_ISEXPR(args, i, fname) \
    LASSERT(args, (ISEXPR(args->cell[i]->type)), \
            "function '%s' passed incorrect type for arg %i; got %s, expected expression type", \
            fname, i, awlval_type_name(args->cell[i]->type));

#define LASSERT_ARGCOUNT(args, expected, fname) \
    LASSERT(args, (args->count == expected), \
            "function '%s' takes exactly %i argument(s); %i given", fname, expected, args->count);

#define LASSERT_MINARGCOUNT(args, min, fname) \
    LASSERT(args, (args->count >= min), \
            "function '%s' takes %i or more arguments; %i given", fname, min, args->count);

#define LASSERT_NONEMPTY(args, awlval, fname) \
    LASSERT(args, (awlval->count != 0), "function '%s' passed {}", fname);

#endif
