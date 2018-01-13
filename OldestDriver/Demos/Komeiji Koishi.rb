#encoding: utf-8
require './libcore.rb'
HFWindow.new("恋恋!", 300, 300) { 
    set_fixed true    
    show
    window = self
    set_handler(:on_closed) {exit_mainloop}
    device = DX::D3DDevice.new(DX::HARDWARE_DEVICE);
    swap_chain = DX::SwapChain.new(device, window)
    context = device.immcontext.bind_pipeline(DX::RenderPipeline.new {
        set_vshader DX::Shader.load_hlsl(device, "texture_vs.shader", DX::VertexShader)
        set_pshader DX::Shader.load_hlsl(device, "texture_ps.shader", DX::PixelShader)
        set_input_layout device, ["POSITION", "TEXCOORD"], [DX::R32G32_FLOAT, DX::R32G32_FLOAT]
    }).instance_eval {
        texture = DX::D3DTexture2D.new(device, "../CommonFiles/300px-Komeiji Koishi.jpg")
        vecs = [[-1.0, -1.0], [0.0, 1.0],
                [-1.0, 1.0],  [0.0, 0.0],
                [1.0, -1.0],  [1.0, 1.0],
                [1.0, 1.0],   [1.0, 0.0]].flatten.pack("f*")
        bind_vbuffer(0, DX::D3DVertexBuffer.new(device, vecs.size, vecs), 4*4)
        bind_resource(0, texture, DX::SHADERS_APPLYTO_PSHADER)
        set_topology(DX::TOPOLOGY_TRIANGLESTRIP)
        set_viewport(HFRect.new(0, 0, window.width, window.height), 0.0, 1.0)
        set_render_target(swap_chain.back_buffer)
        self
    }
    messageloop {
        context.clear_render_target(swap_chain.back_buffer, HFColorRGBA.new(0.0, 0.0, 0.0, 0.0));
        context.draw(0, 4)
        swap_chain.present(DX::SwapChain::VSYNC_2_BLANK);
    }
}
