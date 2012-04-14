# encoding: utf-8

require 'ultrapants'
require 'nanoc'

module Ultrapants
  class NanocFilter < ::Nanoc::Filter

    identifier :ultrapants

    def run(content, params={})
      config = Ultrapants::Config.new
      config[:wrap_amps] = 1

      Ultrapants.process(config, content)
    end

  end
end
