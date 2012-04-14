#!/usr/bin/env ruby
# encoding: utf-8

require 'ffi'

module Ultrapants

  VERSION = '0.1'

  extend FFI::Library

  ffi_lib FFI::Library::LIBC
  ffi_lib 'ultrapants'

  class Config < ::FFI::Struct
    layout :wrap_amps, :char
  end

  attach_function :_ultrapants, :ultrapants, [ Config.by_value, :string, :int ], :pointer
  attach_function :_free,       :free,       [ :pointer                       ], :void

  def self.process(config, t)
    ptr = _ultrapants(config, t, t.size)
    str = ptr.read_string_to_null
    _free(ptr)
    str
  end

end
