We use sphinx (and deployment to RTD) for nearly all of our documentation within Project 8.
Here we collect the files and instructions needing for setting up the above; these are very language specific.

# General Notes
- In many cases, you will not actually use/include these files in a project, but will instead copy and modify them as a starting template. This means that changes to the template will *not* propagate to other packages automatically.
- The template files provided are intended as a working starting point, but modification and additions are expected. It may also be required to make changes to how builds are done on the RTD site using their advanced admin options. This action is currently restricted to the primary maintainers for each (contact Noah if you believe you need access).

# Subdirectories
Subdirectories here contain instructions (in their README.md files) and an example files needed for various languages.
Depending on the repo, scarab may or may not be expected to be an available submodule.
The cpp subdirectory also contains the actual documentation for scarab (it is an used as the example for other cpp packages)

# Generic building notes
For a complete understanding of how documentation is built on RTD, you should go look at their documentation.
For our purposes, it is usually reasonable to consider the following steps:

1. Github sends an HTTP POST on certain events (generally pushes and releases) and RTD may trigger a build, depending on the details of the event and its configuration.
1. If a build is triggered, RTD spins up their standard build container and clones the desired commit
1. RTD installs their default python environment
1. If configured to do so, RTD installs python packages using pip and a named requirements.txt file from within the repo
1. If configured to do so, RTD attempts to install the provided python package (based on a setup.py file)
1. RTD cats the conf.py file which will be used for building the webpage (this is done to ease some debugging)
1. RTD builds the sphinx documentation based on the conf.py file
1. RTD moves the build output to their hosting location so that the webpage is served.
