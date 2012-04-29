# encoding: utf-8

require 'fast-aleck'
require 'nanoc'

module FastAleck
  class NanocFilter < ::Nanoc::Filter

    identifier :fast_aleck

    def run(content, params={})
      config = FastAleck::Config.new
      config[:wrap_amps] = 1

      FastAleck.process(config, content)
    end

  end
end
