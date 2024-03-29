#!/usr/bin/ruby

require File.expand_path(ENV['MOSYNCDIR']+'/rules/mosync_exe.rb')

work = PipeExeWork.new
work.instance_eval do
	@SOURCES = ["."]
	@EXTRA_CPPFLAGS = ' -Wno-shadow -Wno-float-equal'
	@LIBRARIES = ['mautil', 'maui']
	@NAME = 'OpenDemo2'
end

work.invoke
