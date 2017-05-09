/*
 * python_macros.hh
 *
 * Created on: May 9, 2017
 *     Author: laroque
 */

#ifndef PYTHON_MACROS_HH_
#define PYTHON_MACROS_HH_

#define PROPERTYMEMBER( CLASS, MEMBER ) \
    .add_property( #MEMBER, &CLASS::get_##MEMBER, &CLASS::set_##MEMBER )

#define PROPERTYMEMBER_NOSET( CLASS, MEMBER ) \
    .add_property( #MEMBER, &CLASS::get_##MEMBER, boost::python::no_set )

#define PROPERTYMEMBER_RETPOLICY( CLASS, MEMBER, RETPOLICY ) \
    .add_property( #MEMBER, make_function( &CLASS::get_##MEMBER, return_value_policy<RETPOLICY>()), &CLASS::set_##MEMBER )

#define PROPERTYMEMBER_RETPOLICY_NOSET( CLASS, MEMBER, RETPOLICY ) \
    .add_property( #MEMBER, make_function( &CLASS::get_##MEMBER, return_value_policy<RETPOLICY>()), boost::python::no_set )

#endif /* PYTHON_MACROS_HH_ */
