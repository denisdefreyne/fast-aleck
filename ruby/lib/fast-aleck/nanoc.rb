# encoding: utf-8

require 'fast-aleck'
require 'nanoc'

module FastAleck

  # A Fast Aleck filter for [nanoc](http://nanoc.stoneship.org/).
  class NanocFilter < ::Nanoc::Filter

    identifier :fast_aleck

    def run(content, params={})
      config = FastAleck::Config.new

      if params[:wrap_amps]
        config[:wrap_amps] = 1
      end

      if params[:wrap_quotes]
        config[:wrap_quotes] = 1
      end

      p config

      FastAleck.process(config, content)
    end

  end

end
