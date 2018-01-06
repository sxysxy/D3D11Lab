require 'fiddle'
require 'win32api'
require './libcore.rb'

begin
	client = Client.new("我就是叫紫妈怎么了askdjladkasgjlkvnlsn", 500, 500, 60)
	client.mainloop {|renderer| nil}
rescue Exception => e
	Win32API.new("user32", "MessageBox", "ippi", "i").call(0, e.backtrace.to_s+e.message, "Exception", 0)
end