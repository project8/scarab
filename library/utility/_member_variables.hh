/*
 * _member_variables.hh
 *
 *  Created on: Jan 6, 2016
 *      Author: nsoblath
 */

#include "macros.hh"

/**
 * Templates for macros for class member variables
 *
 * Types of variables:
 * - "normal"
 * - referrable (i.e. accessed by reference)
 * - pointers
 * - std::shared_ptr
 * - std::atomic
 *
 * Variations:
 * - no set (non-const) accessor
 * - static
 * - mutable
 *
 * How to use this file:
 *
 * 1. Create the include file that will be used (see e.g. member_variables.hh)
 * 2. Add include guards
 * 3. Define set_prefix, get_prefix, var_prefix, and static_prefix
 * 4. #include this file
 *
 * Macros that must be defined to use this file:
 * - set_prefix: the prefix for set accessor functions (e.g. set_)
 * - get_prefix: the prefix for get accessor functions (e.g. get_)
 * - var_prefix: the prefix for variables (e.g. f_)
 * - static_prefix: the prefix for static variables (e.g. s_)
 *
 * Accessors for normal variables, pointers, and atomics are of the form [set_prefix][variable name] and [get_prefix][variable_name].
 * Accessors for referrable variables and shared_ptr's are of the form [variable name], both const and non-const
 *
 */


#define get_fcn( x_variable ) PASTE( get_prefix, x_variable )
#define set_fcn( x_variable ) PASTE( set_prefix, x_variable )
#define var_name( x_variable ) PASTE( var_prefix, x_variable )
#define static_var_name( x_variable ) PASTE( static_prefix, x_variable )

//**********
// normal
//**********

#define mv_accessible_noset( x_type, x_variable )\
    protected:\
        x_type var_name( x_variable );\
    public:\
        x_type get_fcn( x_variable )() const\
        {\
            return var_name( x_variable );\
        }

#define mv_accessible( x_type, x_variable )\
    mv_accessible_noset( x_type, x_variable )\
    public:\
        void set_fcn( x_variable )( x_type p_variable )\
        {\
            var_name( x_variable ) = p_variable;\
            return;\
        }

#define mv_accessible_static_noset( x_type, x_variable )\
    public:\
        static x_type get_fcn( x_variable )()\
        {\
            return static_var_name( x_variable );\
        }\
    protected:\
        static x_type static_var_name( x_variable );

#define mv_accessible_static( x_type, x_variable )\
    public:\
        static void set_fcn( x_variable )( x_type p_variable )\
        {\
            static_var_name( x_variable ) = p_variable;\
            return;\
        }\
        mv_accessible_static_noset( x_type, x_variable )

#define mv_accessible_mutable_noset( x_type, x_variable )\
    public:\
        x_type get_fcn( x_variable )() const\
        {\
            return var_name( x_variable );\
        }\
    protected:\
        mutable x_type var_name( x_variable );

#define mv_accessible_mutable( x_type, x_variable )\
    public:\
        void set_fcn( x_variable )( x_type p_variable ) const\
        {\
            var_name( x_variable ) = p_variable;\
            return;\
        }\
        mv_accessible_mutable_noset( x_type, x_variable )


//**************
// referrable
//**************

#define mv_referrable_const( x_type, x_variable )\
    public:\
        const x_type& x_variable() const\
        {\
            return var_name( x_variable );\
        }\
    protected:\
        x_type var_name( x_variable );

#define mv_referrable( x_type, x_variable )\
    public:\
        x_type& x_variable()\
        {\
            return var_name( x_variable );\
        }\
        mv_referrable_const( x_type, x_variable )

#define mv_referrable_static( x_type, x_variable )\
    public:\
        static x_type& x_variable()\
        {\
            return static_var_name( x_variable );\
        }\
    protected:\
        static x_type static_var_name( x_variable );

#define mv_referrable_mutable( x_type, x_variable )\
    public:\
        x_type& x_variable() const\
        {\
            return var_name( x_variable );\
        }\
    protected:\
        mutable x_type var_name( x_variable );


//***********
// pointer
//***********

#define mv_assignable_noset( x_type, x_variable )\
    public:\
        x_type* get_fcn( x_variable )() const\
        {\
            return var_name( x_variable );\
        }\
    protected:\
        x_type* var_name( x_variable );

#define mv_assignable( x_type, x_variable )\
    public:\
        void set_fcn( x_variable )( x_type* p_variable )\
        {\
            delete var_name( x_variable );\
            var_name( x_variable ) = p_variable;\
            return;\
        }\
        mv_assignable_noset( x_type, x_variable )

#define mv_assignable_static_noset( x_type, x_variable )\
    public:\
        static x_type* get_fcn( x_variable )()\
        {\
            return static_var_name( x_variable );\
        }\
    protected:\
        static x_type* static_var_name( x_variable );

#define mv_assignable_static( x_type, x_variable )\
    public:\
        static void set_fcn( x_variable )( x_type* p_variable )\
        {\
            delete static_var_name( x_variable );\
            static_var_name( x_variable ) = p_variable;\
            return;\
        }\
        mv_assignable_static_noset( x_type, x_variable )

#define mv_assignable_mutable_noset( x_type, x_variable )\
    public:\
        x_type* get_fcn( x_variable )() const\
        {\
            return var_name( x_variable );\
        }\
    protected:\
        mutable x_type* var_name( x_variable );

#define mv_assignable_mutable( x_type, x_variable )\
    public:\
        void set_fcn( x_variable )( x_type* p_variable ) const\
        {\
            delete var_name( x_variable );\
            var_name( x_variable ) = p_variable;\
            return;\
        }\
        mv_assignable_mutable_noset( x_type, x_variable )


//**************
// shared_ptr
//**************

#define mv_shared_ptr_const( x_type, x_variable )\
    public:\
        const std::shared_ptr< x_type > x_variable() const\
        {\
            return var_name( x_variable );\
        }\
    protected:\
        std::shared_ptr< x_type > var_name( x_variable );

#define mv_shared_ptr( x_type, x_variable )\
    public:\
        std::shared_ptr< x_type > x_variable()\
        {\
            return var_name( x_variable );\
        }\
        mv_shared_ptr_const( x_type, x_variable )

#define mv_shared_ptr_static( x_type, x_variable )\
    public:\
        static std::shared_ptr< x_type >  x_variable()\
        {\
            return static_var_name( x_variable );\
        }\
    protected:\
        static std::shared_ptr< x_type > static_var_name( x_variable );

#define mv_shared_ptr_mutable( x_type, x_variable )\
    public:\
        std::shared_ptr< x_type > x_variable() const\
        {\
            return var_name( x_variable );\
        }\
    protected:\
        mutable std::shared_ptr< x_type > var_name( x_variable );


//**********
// atomic
//**********

#define mv_atomic_noset( x_type, x_variable )\
    public:\
        x_type get_fcn( x_variable )() const\
        {\
            return var_name( x_variable ).load();\
        }\
    protected:\
        std::atomic< x_type > var_name( x_variable );

#define mv_atomic( x_type, x_variable )\
    public:\
        void set_fcn( x_variable )( x_type p_variable )\
        {\
            var_name( x_variable ).store( p_variable );\
            return;\
        }\
        mv_atomic_noset( x_type, x_variable )

#define mv_atomic_static_noset( x_type, x_variable )\
    public:\
        static x_type get_fcn( x_variable )()\
        {\
            return static_var_name( x_variable ).load();\
        }\
    protected:\
        static std::atomic< x_type > static_var_name( x_variable );

#define mv_atomic_static( x_type, x_variable )\
    public:\
        static void set_fcn( x_variable )( x_type p_variable )\
        {\
            static_var_name( x_variable ).store( p_variable );\
            return;\
        }\
        mv_atomic_static_noset( x_type, x_variable )

#define mv_atomic_mutable_noset( x_type, x_variable )\
    public:\
        x_type get_fcn( x_variable )() const\
        {\
            return var_name( x_variable ).load();\
        }\
    protected:\
        mutable std::atomic< x_type > var_name( x_variable );

#define mv_atomic_mutable( x_type, x_variable )\
    public:\
        void set_fcn( x_variable )( x_type p_variable ) const\
        {\
            var_name( x_variable ).store( p_variable );\
            return;\
        }\
        mv_atomic_mutable_noset( x_type, x_variable )

