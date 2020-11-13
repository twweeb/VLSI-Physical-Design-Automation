#!/usr/bin/ruby
# Code::      genTestcase.rb
# Purpose::   Generate HW2 testcases
# Author::    Yu-Hsiang Cheng (zz68720492@gmail.com)
# Version::   2.0, 03/06/2017
# Copyright:: Copyright (c) 2017 VLSIPDA

puts "Usage: ruby #{$0} [filename] [#cells] [maxCellSize] [#nets] [maxDegree]" unless ARGV.size == 5
bar = "="*10
puts bar + " Start Generating " + bar
puts "filename: " + filename = ARGV[0] || "test"
puts "#cells: " + (num_cells = (ARGV[1] || rand(2..50000)).to_i).to_s
puts "maxCellSize: " + (max_cell_size = (ARGV[2] || rand(2..100)).to_i).to_s
puts "#nets: " + (num_nets = (ARGV[3] || rand(1..100000)).to_i).to_s
puts "#maxDegree: " + (max_degree = (ARGV[4] || rand(2..30)).to_i).to_s
arr_cells = (1..num_cells).to_a.shuffle
open(filename + ".cells", "w") do |file|
    num_cells.times do |i| 
        file.puts "c#{arr_cells[i]} #{rand(1..max_cell_size)}"
    end
end
open("#{filename}.nets", "w") do |file|
    (1..num_nets).each do |i|
        file.print "NET n#{i} { "
        deg = rand(2..max_degree)
        arr = arr_cells.sample(deg)
        deg.times do |i| file.print "c#{arr[i]} " end
        file.puts "}"
    end
end
puts bar + "  End Generating  " + bar
