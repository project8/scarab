Building Scarab
===============

Build Options
-------------

* Scarab_BUILD_PARAM 
 
  * Build the core param and application configuration classes

* Scarab_BUILD_CODEC_JSON

  * Build the JSON codec
  * Requires Scarab_BUILD_PARAM

* Scarab_BUILD_CODEC_YAML

  * Build the YAML codec
  * Requires Scarab_BUILD_PARAM

* Scarab_BUILD_AUTHENTICATION

  * Build the authentication class 
  * Requires Scarab_BUILD_PARAM and Scarab_BUILD_CODEC_JSON
