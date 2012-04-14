#!/usr/bin/env ruby
# encoding: utf-8

require 'ffi'

module Ultrapants
  extend FFI::Library

  ffi_lib FFI::Library::LIBC
  ffi_lib 'ultrapants'

  attach_function :_ultrapants, :ultrapants, [ :string, :int ], :pointer
  attach_function :_free,       :free,       [ :pointer      ], :void

  def self.process(t)
    ptr = _ultrapants(t, t.size)
    str = ptr.read_string_to_null
    _free(ptr)
    str
  end

end
