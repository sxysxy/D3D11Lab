class Client
	def mainloop(&callback)
		r = Thread.start {renderer.mainloop}
		r.abort_on_exception = true
		f = Thread.start {logicloop {callback.call}}
		f.abort_on_exception = true
		
		#Win32API.new("user32", "MessageBox", "ippi", "i").call(0, "233", "emm", 0)
		messageloop
		shutdown
		f.join
	end
end