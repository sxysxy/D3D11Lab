#include "stdafx.h"
#include "rubyext.h"

std::wstring CharToWchar(const char* c, size_t m_encode = CP_ACP){
	std::wstring str;
	int len = MultiByteToWideChar(m_encode, 0, c, strlen(c), NULL, 0);
	wchar_t*    m_wchar = new wchar_t[len + 1];
	MultiByteToWideChar(m_encode, 0, c, strlen(c), m_wchar, len);
	m_wchar[len] = '\0';
	str = m_wchar;
	delete m_wchar;
	return str;
}

std::string WcharToChar(const wchar_t* wp, size_t m_encode = CP_ACP){
	std::string str;
	int len = WideCharToMultiByte(m_encode, 0, wp, wcslen(wp), NULL, 0, NULL, NULL);
	char    *m_char = new char[len + 1];
	WideCharToMultiByte(m_encode, 0, wp, wcslen(wp), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	str = m_char;
	delete m_char;
	return str;
}

Client *client;

typedef VALUE(*rubyfunc) (...);
static VALUE init_client(VALUE self, VALUE title, VALUE w, VALUE h, VALUE fps) {
	
	auto wtitle = CharToWchar(rb_string_value_cstr(&title));

	client = new Client(wtitle, FIX2INT(w), FIX2INT(h));
	client->Initialize();
	client->SetFrameRate(FIX2INT(fps));
	client->renderer.SetFrameRate(FIX2INT(fps));

	rb_iv_set(self, "@renderer", rb_eval_string("Renderer.new"));

	Renderer2D::CreatePipelines();

	return self;
}

static VALUE client_messageloop(VALUE self) {
	client->MessageLoop();
	return Qnil;
}

static VALUE client_logicloop(VALUE self, VALUE callback) {
	client->LogicLoop([=](Renderer *) {
		rb_funcall(callback, rb_intern("call"), rb_iv_get(self, "@renderer"));
	});
	return Qnil;
}

static VALUE client_shutdown(VALUE self) {
	client->quit = true;
	return Qnil;
}

static VALUE get_renderer(VALUE self) {
	return rb_iv_get(self, "@renderer");
}

static VALUE renderer_mainloop(VALUE self) {
	client->renderer.MainloopProc();
	return Qnil;
}

static VALUE renderer_clear(VALUE self) {
	client->renderer.Clear();
	return Qnil;
}

static VALUE renderer_bind_shape2d_pipeline(VALUE self){
	client->renderer.BindPipeline(&Renderer2D::render_shape2d_pipeline);
	return Qnil;
}

static VALUE renderer_bind_texture2d_pipeline(VALUE self) {
	client->renderer.BindPipeline(&Renderer2D::render_texture2d_pipeline);
	return Qnil;
}

static VALUE system_show_console(VALUE self) {
	AllocConsole();
	freopen("CON", "W", stdout);
	return Qnil;
}

void MakeExt() {
	client = nullptr;

	rb_define_method(rb_cObject, "show_console", (rubyfunc)system_show_console, 0);

	VALUE client_class = rb_define_class("Client", rb_cObject);
	rb_define_method(client_class, "initialize", (rubyfunc)init_client, 4);
	//rb_define_method(client_class, "mainloop", (rubyfunc)client_mainloop, 1);
	rb_define_method(client_class, "messageloop", (rubyfunc)client_messageloop, 0);
	rb_define_method(client_class, "logicloop", (rubyfunc)client_logicloop, 1);
	rb_define_method(client_class, "shutdown", (rubyfunc)client_shutdown, 0);

	VALUE renderer_class = rb_define_class("Renderer", rb_cObject);
	rb_define_method(client_class, "renderer", (rubyfunc)get_renderer, 0);
	rb_define_method(renderer_class, "mainloop", (rubyfunc)renderer_mainloop, 0);
	rb_define_method(renderer_class, "clear", (rubyfunc)renderer_clear, 0);
	rb_define_method(renderer_class, "bind_shape2d_pipeline", (rubyfunc)renderer_bind_shape2d_pipeline, 0);
	rb_define_method(renderer_class, "bind_texture2d_pipeline", (rubyfunc)renderer_bind_texture2d_pipeline, 0);

	
}

void TermExt() {
	if (client)delete client;
}
