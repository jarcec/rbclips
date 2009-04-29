require 'mkmf'

# Clips should be linked into ./clipssrc/ directory
#find_library('clips', '', './clipssrc/')
#find_header('clips.h', './clipssrc/')

# Create my new makefile
create_makefile('rbclips')
