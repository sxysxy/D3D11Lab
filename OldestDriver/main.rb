require 'win32api'
Win32API.new("user32", "MessageBox", "ippi", "i").call 0, "2", "h", 0