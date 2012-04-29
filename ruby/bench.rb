require 'benchmark'

require 'fast-aleck'
require 'rubypants'

alice = File.read('../input/alice.txt')

Benchmark.bmbm do |r|
  r.report('alice-rubypants') do 
    1000.times do
      RubyPants.new(alice).to_html
    end
  end
  r.report('alice-fast-aleck') do 
    1000.times do
      config = FastAleck::Config.new
      config[:wrap_amps] = 1
      FastAleck.process(config, alice)
    end
  end
end

