#encoding: utf-8
require './libcore.rb'
HFWindow.new("我就是叫紫妈怎么了aldjajaogfoiadfas", 500, 500) { 
	show
	set_handler(:on_resized) {msgbox 'resized'}
	set_handler(:on_closed) {msgbox 'closed'}
}
DX::messageloop {sleep(0.01)}