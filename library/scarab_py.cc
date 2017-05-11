#include <boost/python.hpp>

/* include components */
#include "factory_py.hh"

BOOST_PYTHON_MODULE(scarab_py)
{
    namespace bp = boost::python;
    //actually, we want to use Nymph:: object here, perhaps call it in nymphpy
    export_factory<int, int>();
}
