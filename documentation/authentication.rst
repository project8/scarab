==============
Authentication
==============

The ``authentication`` class is designed to handle authentication information for applications 
such as login details.

Structure
=========

Authentication information is handled in two parts: design and data.  
The design specifies what authentication information is expected and where it's coming from, 
while the data contains the authentication information itself.

Authentication information is divided into groups, and each group contains items.  
A group can be thought of as the login information for a given service, 
and each piece of information is an item.  

Design
======

As an example application of the authentication class, consider the login information needed for a database.  
The group might be called `database` and the two items used would be `username` and `password`.

The design for the above example might look like (in YAML format)::

    groups:
      database:
        username:
          default: a_user
          env: DB_USER
        password:
          default: 123456
          env: DB_PASSWORD
        url:
          default: my.database

Sources of information
======================

Three sources of information are used to determine the final authentication information.  
In order of precedence from low to high, they are:

1. Default values
2. Authentication file
3. Environment variables

Default values are given in the design.  As shown above, each item has a default, which is required.

Authentication files are data files (typically JSON or YAML) that specify one or more groups of 
authentication information.  If the above `database` example were provided in a YAML authentication file, 
it would look like this (note that in the case the user did not include the `url` item)::

    database:
      username: file_user
      password: file_pword

Individual items can be specified in environment variables.  The specific variable that should be checked 
for a particular item is specified in the design (see the above example.  If a user wanted to override the 
password from the above example, they could use an environment variable at runtime, e.g.::

    > DB_PASSWORD=my_real_pword run_my_app

Data
====

The data is determined by combining the three sources of information in order: the authentication file 
overrides default values, and the enviroment variables override information in the authentication file or defaults. 
Combining the above defaults, authentication file, and environment variable for the `database` example, 
the final data would be::

    database:
      username: file_user
      password: my_real_pword
      url: my.database
