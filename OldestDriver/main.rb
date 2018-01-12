#encoding: utf-8
require './libcore.rb'
HFWindow.new("恋恋！", 500, 500) { 
    set_fixed true; show
    device = DX::D3DDevice.new(DX::D3DDevice::HARDWARE_DEVICE);
    swap_chain = DX::SwapChain.new(device, self, true)
    set_handler(:on_resized) {swap_chain.resize(width, height)}
    set_handler(:on_closed) {exit_mainloop}
    texture = DX::D3DTexture2D.new(device, "../CommonFiles/300px-Komeiji Koishi.jpg");
    context = DX::D3DDeviceContext.new(device)
    context.bind_pipeline(DX::RenderPipeline.new {
        set_vshader DX::Shader.load_hlsl(device, "texture_vs.shader", DX::VertexShader)
        set_pshader DX::Shader.load_hlsl(device, "texture_ps.shader", DX::PixelShader)
        set_input_layout device, ["POSITION", "TEXCOORD"], [DX::RenderPipeline::R32G32_FLOAT]*2
    })
    messageloop {
        device.immcontext.exec_command_list(context)  
        swap_chain.present(DX::SwapChain::VSYNC_2_BLANK);
    }
}



