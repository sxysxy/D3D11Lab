#encoding: utf-8
require './libcore.rb'

HFWindow.new("我就是叫紫妈怎么了aldjajaogfoiadfas", 500, 500) { 
	show
  
  device = DX::D3DDevice.new(DX::D3DDevice::HARDWARE_DEVICE);
  swap_chain = DX::SwapChain.new(device, self)
	set_handler(:on_resized) {
    set_title("我就是叫紫妈怎么了"+(Array.new(15) {rand(97..122).chr}).join)
    swap_chain.resize(width, height)
  }
	set_handler(:on_closed) {exit_mainloop}
  
  messageloop {
    swap_chain.present(DX::SwapChain::VSYNC_1_BLANK);
  }
}



