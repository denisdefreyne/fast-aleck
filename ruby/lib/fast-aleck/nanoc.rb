# encoding: utf-8

require 'fast-aleck'
require 'nanoc'

module FastAleck

  # A Fast Aleck filter for [nanoc](http://nanoc.stoneship.org/).
  class NanocFilter < ::Nanoc::Filter

    identifier :fast_aleck

    def run(content, params={})
      FastAleck.process(content, params)
    end

  end

end
