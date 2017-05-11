/*
 * factory_py.hh
 *
 * created on: May 11, 2017
 *     Author: laroque
 */

#ifndef FACTORY_PY_HH_
#define FACTORY_PY_HH_

#include "python_macros.hh"
#include "factory.hh"


template< class XBaseType, typename ... XArgs >
void export_factory()
{
    namespace bp = boost::python;

    bp::class_< scarab::base_registrar< XBaseType, XArgs... >, boost::noncopyable >("base_registrar", bp::no_init)
        ;

//    bp::class_<scarab::factory, boost::noncopyable>("factory", bp::init<>())
//        ;
}

#endif /* FACTORY_PY_HH_ */
