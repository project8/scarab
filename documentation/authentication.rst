==============
Authentication
==============

The ``authentication`` class is designed to handle authentication information for applications 
such as login details.

Structure
=========

Authentication information is handled in two parts: specification and data.  
The specification determines what authentication information is expected and where it's coming from, 
while the data contains the authentication information itself.

Authentication information is divided into groups, and each group contains items.  
A group can be thought of as the login information for a given service, 
and each piece of information (e.g. a username or a password) is an item.  

Specification
=============

As an example application of the authentication class, consider the login information needed for a database.  
The group might be called ``database`` and the two items used would be ``username`` and ``password``.

The specification for the above example might look like (in YAML format)::

    groups:
      database:
        username:
          default: a_user
          env: DB_USER
          value: INewton
        password:
          default: 123456
          env: DB_PASSWORD
          file: in_login.txt

Both the username and password have a default value, which will typically be provided by the application that's 
using the authentication system.  The application will also specify the environment variables that the user can 
employ to set the respective values.  In this case, the application has provided the user the opportunity to 
directly override the username (e.g. from the command line) with `value` and to override the password with the contents of 
the named file with `file`.

The ``default``, ``env``, ``value``, and ``file`` parameters are available for all specification items.  
Only the ``default`` is required, and the others can be provided at the application's discretion.

Sources of information
======================

Three sources of information are used to determine the final authentication information.  
In order of precedence from low to high, they are:

1. Default values
2. Authentication file
3. Environment variables
4. File override
5. Value override

When the specification is evaluated and turned into authentication data, items with higher precedence will 
override those with lower precedence.

Default
-------

Default values are given in the specification.  A default is require for each item.

Authentication file
-------------------

Authentication files are data files (typically JSON or YAML) that specify one or more groups of 
authentication information.  If the above ``database`` example were provided in a YAML authentication file, 
it would look like this::

    database:
      username: file_user
      password: file_pword

Note that while authentication files are a convenient way of distributing authentication information 
in a small scale setup, it doesn't scale well to larger systems and isn't compatible with 
``secrets`` systems such as those employed by 
`Kubernetes <https://kubernetes.io/docs/concepts/configuration/secret/>`_ or 
`Docker Swarm <https://docs.docker.com/engine/swarm/secrets/>`_.

Environment variable
--------------------

Individual items can be specified in environment variables.  The specific variable that should be checked 
for a particular item is provided by the application in the specification.  Use of an environment variable 
for any item is optional (at the discretion of the application).  If a user wanted to override the 
password from the above example, they could use an environment variable at runtime, e.g.::

    > DB_PASSWORD=env_pword run_my_app

File override
-------------

Items can be set via the contents of a particular text file.  The use of the override file is optional, 
at the discretion of the application.  An application could, for instance, give the user the opportunity to 
specify an override file at runtime.  This enables integration with "secrets" systems such as those employed by 
`Kubernetes <https://kubernetes.io/docs/concepts/configuration/secret/>`_ or 
`Docker Swarm <https://docs.docker.com/engine/swarm/secrets/>`_.  This feature will copy the contents of a text 
file in its entirety -- beware of unintended new-lines at the end of files, for instance.

As an example of how this might be encountered from a user perspective, 
an app might provide a command-line option to specify a file that will be read in as the password 
``--pword-file``, which could be used as follows::

    > run_my_app --pword-file=in_login.txt

Value override
--------------

Items can be set directly using the value override, which will override all other sources of data.  
The use of the value override is optional, at the discretion of the application.  
An application could, for instance, give the user the opportunity to specify a value on 
the command line or in a configuration file at runtime.  Use this feature carefully 
for "secrets" information, as it can result in leaking information into a CL history, for instance, or other 
collections of plain-text information.

As an example of how this might be encountered from a user perspective,
an app might provide a command-line option to set the username with `-u`, which could be used as follows::

    > run_my_app -u INewton

Data
====

The data are determined by combining the five sources of information as laid out in the specification.  
This is done in five steps:

1. Set defaults
2. Read in the authentication file
3. Get values from environment variables
4. Read file overrides
5. Apply value overrides

The ``data`` data structure can be checked for the presence of an item with the ``has()`` function, and 
the value can be requested with the ``get()`` function.

Example: deriving data from the specification & user input
----------------------------------------------------------

In this example we'll show how the final data is determined from the inputs discussed above.

The application, ``run_my_app``, uses the database and requires database authentication information.  
The application will provide the initial specification, including the single group and the user and 
password items::

    groups:
      database:
        username:
          default: a_user
          env: DB_USER
        password:
          default: 123456
          env: DB_PASSWORD

This application takes advantage of all of ``authentication``'s user-input options: authentication file, 
file overrides, and value overrides.  The user runs the following command::

    > DB_PASSWORD=env_pword run_my_app --auth-file db_auth.yaml -u INewton --pword-file in_login.txt

In the real world, we expect a user would not use *all* of these options at the same time, since 
both the username and password end up being specified multiple times, but it serves to document 
how all of the different options interact.

The contents of db_auth.yaml are::

  database:
    username: file_user
    password: file_pword

And the contents of in_login.txt are::

  dk3j8t8jn&*fllsi32ld

The act of processing the specification into data would go through the five steps described above.  
This table shows the content of the username and password values at each step:

+----------------------------------------+--------------------------------+--------------------------------+
| Event                                  | Value of ``database.username`` | Value of ``database.password`` |
+========================================+================================+================================+ 
| 1. Defaults are set                    | ``a_user``                     | ``123456``                     | 
+----------------------------------------+--------------------------------+--------------------------------+
| 2. Auth file is read                   | ``file_user``                  | ``file_pword``                 |
+----------------------------------------+--------------------------------+--------------------------------+
| 3. Environment variables are checked   | ``file_user``                  | ``env_pword``                  |
+----------------------------------------+--------------------------------+--------------------------------+
| 4. File overrides are checked          | ``file_user``                  | ``dk3j8t8jn&*fllsi32ld``       |
+----------------------------------------+--------------------------------+--------------------------------+
| 5. Value overrides are used            | ``INewton``                    | ``dk3j8t8jn&*fllsi32ld``       |
+----------------------------------------+--------------------------------+--------------------------------+

In the end, the final data would be::

    database:
      username: INewton
      password: dk3j8t8jn&*fllsi32ld

API
===

Specification
-------------

Groups can be added directly with ``authentication::add_group()``, or as part of adding an item if the group 
doesn't already exist with ``authentication::add_item()``.  One or more groups can also be added 
with ``authenticaiton::add_groups()``, where the groups and the items they contain are described in 
a ``param_node``.

Items can be added with ``authentication::add_item()`` (as mentioned above, if the group doesn't exist, 
it will be added) or through ``authentication::add_groups()``.

If an authentication file is to be used, the filename is provided with ``authentication::set_auth_file()``.  
This function will also add groups and items if they're not already present.

File overrides and value overrides are added to the specification with ``authentication::set_file()`` and 
``authentication::set_value()``, respectively.  The item to which the override(s) are being added must already 
be present, or an exception will be thrown (``scarab::error``).

Data
----

The specification is processed into data using ``authentication::process_spec()``.

The presence of a group or item within a group in the data can be checked using ``authentication::has()``.  
The value for a given item can be extracted using ``authentication::get()``.  All values are returned as strings.


Integration with applications
=============================

The ``main_app`` class integrates authentication information that can be configured and processed at runtime. 
The authentication object is then accessible from the ``main_app`` object for any elements of an applciation that 
need the authentication information.

For more information, please see :doc:`application_building`.

