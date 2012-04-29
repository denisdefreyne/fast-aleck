# encoding: utf-8

$LOAD_PATH.unshift(File.expand_path('../lib/', __FILE__))
require 'fast-aleck'

Gem::Specification.new do |s|
  s.name        = 'fast-aleck'
  s.version     = FastAleck::VERSION
  s.homepage    = 'http://stoneship.org/software/fast-aleck/' # TODO CREATE A WEB SITE YOU SILLY PERSON
  s.summary     = 'a fast typographical improvement library'
  s.description = 'Fast Aleck lets your text use proper ellipses, smart quotes, proper dashes and more.'

  s.author = 'Denis Defreyne'
  s.email  = 'denis.defreyne@stoneship.org'

  s.files              = Dir['[A-Z]*'] +
                         Dir['{lib,test}/**/*'] +
                         [ 'fast-aleck.gemspec' ]
  s.require_paths      = [ 'lib' ]

  s.rdoc_options     = [ '--main', 'README.md' ]
  s.extra_rdoc_files = [ 'ChangeLog', 'LICENSE', 'README.md', 'NEWS.md' ]
end
