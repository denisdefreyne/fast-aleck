# encoding: utf-8

$LOAD_PATH.unshift(File.expand_path('../lib/', __FILE__))
require 'ultrapants'

Gem::Specification.new do |s|
  s.name        = 'ultrapants'
  s.version     = Ultrapants::VERSION
  s.homepage    = 'http://stoneship.org/software/ultrapants/' # TODO CREATE A WEB SITE YOU SILLY PERSON
  s.summary     = 'a fast typographical improvement library'
  s.description = 'Ultrapants lets your text use proper ellipses, smart quotes, proper dashes and more.'

  s.author = 'Denis Defreyne'
  s.email  = 'denis.defreyne@stoneship.org'

  s.files              = Dir['[A-Z]*'] +
                         Dir['{lib,test}/**/*'] +
                         [ 'ultrapants.gemspec' ]
  s.require_paths      = [ 'lib' ]

  s.rdoc_options     = [ '--main', 'README.md' ]
  s.extra_rdoc_files = [ 'ChangeLog', 'LICENSE', 'README.md', 'NEWS.md' ]
end
