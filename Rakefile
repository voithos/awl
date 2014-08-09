task :default => :serve

## Tasks

desc 'Start the development server'
task :serve => :clean do
  sh 'jekyll serve -w -D --baseurl='
end

desc 'Clean generated files'
task :clean do
  FileUtils.rm_rf('_site')
end
