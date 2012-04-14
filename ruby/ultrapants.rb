#!/usr/bin/env ruby
# encoding: utf-8

require 'ffi'

module Ultrapants
  extend FFI::Library

  ffi_lib 'ultrapants'

  attach_function :ultrapants, [ :string, :int ], :string
end

input = <<EOS
<p>Hello... I am Denis--Denis Defreyne.</p>

<img src="blah--2.png" title="Something... blahhy">
EOS

puts Ultrapants.ultrapants(input, input.size)
