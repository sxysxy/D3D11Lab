require 'win32api'

def msgbox(msg)
	Win32API.new("user32", "MessageBox", "ippi", "i").call(0, msg.to_s, "Message", 0)
end

def show_console
	Win32API.new("kernel32", "AllocConsole", "", "i").call
	STDOUT.reopen("CON")
end

class Client
	def mainloop(&callback)
		r = Thread.start {renderer.mainloop}
		r.abort_on_exception = true
		f = Thread.start {logicloop(proc {callback.call})}
		f.abort_on_exception = true
		
		messageloop
		r.join
		f.join
	end
end