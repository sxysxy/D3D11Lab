#encoding: utf-8
require './libcore.rb'
HFWindow.new("我就是叫紫妈怎么了aldjajaogfoiadfas", 500, 500) { 
	show
	set_handler(:on_resized) {set_title("我就是叫紫妈怎么了"+(Array.new(15) {rand(97..122).chr}).join) }
	set_handler(:on_closed) {exit_process 0}
}
DX::messageloop {sleep(0.01)}