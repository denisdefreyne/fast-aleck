#!/usr/bin/env ruby
# encoding: utf-8

require 'ffi'

module Ultrapants
  extend FFI::Library

  ffi_lib 'ultrapants'

  attach_function :_ultrapants, :ultrapants, [ :string, :int ], :string

  def self.process(t)
    _ultrapants(t, t.size)
  end

end

####################

input = <<EOS
<p>Hello... I am Denis--Denis Defreyne.</p>

<img src="blah--2.png" title="Something... blahhy">
EOS

puts Ultrapants.process(input)
