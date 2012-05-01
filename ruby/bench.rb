require 'benchmark'

require 'fast-aleck'
require 'rubypants'
require 'typogruby'

alice = File.read('../input/alice.txt')

Benchmark.bmbm do |r|
  r.report('alice-rubypants (x10)') do 
    500.times do
      RubyPants.new(alice).to_html
    end
  end
  r.report('alice-typogruby (x100)') do 
    50.times do
      Typogruby.improve(alice)
    end
  end
  r.report('alice-fast-aleck-all-options') do 
    5000.times do
      FastAleck.process(
        alice,
        :wrap_amps => true,
        :wrap_caps => true,
        :wrap_quotes => true,
        :widont => true)
    end
  end
  r.report('alice-fast-aleck-no-options') do 
    5000.times do
      FastAleck.process(alice)
    end
  end
end

