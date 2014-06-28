#ifndef AWL_ASSERT_H
#define AWL_ASSERT_H

#define AWLASSERT(args, cond, fmt, ...) \
    if (!(cond)) { \
        awlval* err = awlval_err(fmt, __VA_ARGS__); \
        awlval_del(args); \
        return err; \
    }

#define AWLASSERT_TYPE(args, i, expected, fname) \
    AWLASSERT(args, (args->cell[i]->type == expected), \
            "function '%s' passed incorrect type for arg %i; got %s, expected %s", \
            fname, i, awlval_type_name(args->cell[i]->type), awlval_type_name(expected));

#define AWLASSERT_ISNUMERIC(args, i, fname) \
    AWLASSERT(args, (ISNUMERIC(args->cell[i]->type)), \
            "function '%s' passed incorrect type for arg %i; got %s, expected numeric type", \
            fname, i, awlval_type_name(args->cell[i]->type));

#define AWLASSERT_ISCOLLECTION(args, i, fname) \
    AWLASSERT(args, (ISCOLLECTION(args->cell[i]->type)), \
            "function '%s' passed incorrect type for arg %i; got %s, expected collection type", \
            fname, i, awlval_type_name(args->cell[i]->type));

#define AWLASSERT_ISEXPR(args, i, fname) \
    AWLASSERT(args, (ISEXPR(args->cell[i]->type)), \
            "function '%s' passed incorrect type for arg %i; got %s, expected expression type", \
            fname, i, awlval_type_name(args->cell[i]->type));

#define AWLASSERT_ARGCOUNT(args, expected, fname) \
    AWLASSERT(args, (args->count == expected), \
            "function '%s' takes exactly %i argument(s); %i given", fname, expected, args->count);

#define AWLASSERT_MINARGCOUNT(args, min, fname) \
    AWLASSERT(args, (args->count >= min), \
            "function '%s' takes %i or more arguments; %i given", fname, min, args->count);

#define AWLASSERT_NONEMPTY(args, awlval, fname) \
    AWLASSERT(args, (awlval->count != 0), "function '%s' passed {}", fname);

#endif
