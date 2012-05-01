# encoding: utf-8

require 'fast-aleck'
require 'nanoc'

module FastAleck

  # A Fast Aleck filter for [nanoc](http://nanoc.stoneship.org/).
  class NanocFilter < ::Nanoc::Filter

    identifier :fast_aleck

    def run(content, params={})
      res = FastAleck.process(content, params)
      res.force_encoding('utf-8')
      res
    end

  end

end
