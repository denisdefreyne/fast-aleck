#!/usr/bin/env ruby
# encoding: utf-8

require 'ffi'

module FastAleck

  VERSION = '0.1'

  # @api private
  module C

    extend FFI::Library

    ffi_lib FFI::Library::LIBC
    ffi_lib 'fast-aleck'

    # @api private
    class Config < ::FFI::Struct
      layout :wrap_amps,   :char,
             :wrap_caps,   :char,
             :wrap_quotes, :char,
             :widont,      :char
    end

    attach_function :fast_aleck, [ Config.by_value, :string, :int, :pointer ], :pointer
    attach_function :free,       [ :pointer ], :void

  end

  # Processes the given string.
  #
  # @option params [Boolean] :wrap_amps (false) true if ampersands should be
  #   wrapped in `<span class="amp">`, false if not
  #
  # @option params [Boolean] :wrap_quotes (false) true if single quotes should
  #   be wrapped in `<span class="quo">` and double quotes in
  #   `<span class="dquo">`, false if not
  #
  # @return [String] The processed string
  def self.process(s, params={})
    config = ::FastAleck::C::Config.new
    config[:wrap_amps]   = params[:wrap_amps]   ? 1 : 0
    config[:wrap_caps]   = params[:wrap_caps]   ? 1 : 0
    config[:wrap_quotes] = params[:wrap_quotes] ? 1 : 0
    config[:widont]      = params[:widont]      ? 1 : 0

    ptr = ::FastAleck::C.fast_aleck(config, s, s.size, FFI::Pointer::NULL)
    str = ptr.read_string_to_null
    ::FastAleck::C.free(ptr)
    str
  end

end
