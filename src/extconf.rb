require 'mkmf'

if find_library('clips', 'InitializeEnvironment', './clips/')
  create_makefile('rbclips')
else
  puts ""
  puts "You have to previously build clips in ./clips/:"
  puts "cd ./clips && make -f makefile.lib"
end
