Param System
============

Purpose: 

Types
-----

* `param`: Null type; defines the basic interface
* `param_value`: Boolean, numeric, or string value
* `param_array`: Array-like structure
* `param_node`: Map- or dict-like structure

Basic Interface
---------------

We'll use this structure as an example in this section (specified here in YAML format)::

    ---
    a_string: "Hello world"
    an_int2: 5
    an_array: [1.1, 2, "three"]
    a_node:
        value1: "one point one"
        value2: 2.0
        value3: 3


Building a Structure
####################


Accessing a Structure
#####################


Codecs: Tranlating to and from Param Structures
-----------------------------------------------
