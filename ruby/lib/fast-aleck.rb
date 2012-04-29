#!/usr/bin/env ruby
# encoding: utf-8

require 'ffi'

module FastAleck

  VERSION = '0.1'

  extend FFI::Library

  ffi_lib FFI::Library::LIBC
  ffi_lib 'fast-aleck'

  class Config < ::FFI::Struct
    layout :wrap_amps,   :char,
           :wrap_quotes, :char
  end

  attach_function :_fast_aleck, :fast_aleck, [ Config.by_value, :string, :int ], :pointer
  attach_function :_free,       :free,       [ :pointer                       ], :void

  def self.process(config, t)
    ptr = _fast_aleck(config, t, t.size)
    str = ptr.read_string_to_null
    _free(ptr)
    str
  end

end
