#encoding: utf-8
require './libcore.rb'

HFWindow.new("恋恋！", 500, 500) { 
    #set_fixed true
    #move_to 200, 200
    show
    
    device = DX::D3DDevice.new(DX::D3DDevice::HARDWARE_DEVICE);
    swap_chain = DX::SwapChain.new(device, self, true)
    
    set_handler(:on_resized) {swap_chain.resize(width, height)}
    set_handler(:on_closed) {exit_mainloop}
    
    texture1 = DX::D3DTexture2D.new(device, 100, 100)
    texture2 = DX::D3DTexture2D.new(device, "../CommonFiles/300px-Komeiji Koishi.jpg");
    context = DX::D3DDeviceContext.new(device)
    
    timer = FPSTimer.new(60)
    messageloop {
        #装作这里进行了一堆渲染任务 (...)
        device.immcontext.exec_command_list(context.get_command_list)
        context.clear_command_list
    
        swap_chain.present(DX::SwapChain::VSYNC_NO);
        timer.await
    }
}



